/* ============================================================
* TabManager plugin for Falkon
* Copyright (C) 2013-2017  S. Razi Alavizadeh <s.r.alavizadeh@gmail.com>
* Copyright (C)      2018 David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "tabmanagerwidget.h"
#include "ui_tabmanagerwidget.h"
#include "mainapplication.h"
#include "browserwindow.h"
#include "webtab.h"
#include "webpage.h"
#include "tabbedwebview.h"
#include "tabwidget.h"
#include "locationbar.h"
#include "bookmarkstools.h"
#include "bookmarkitem.h"
#include "bookmarks.h"
#include "tabmanagerplugin.h"
#include "tldextractor/tldextractor.h"
#include "tabmanagerdelegate.h"
#include "tabcontextmenu.h"
#include "tabbar.h"

#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QDialog>
#include <QTimer>
#include <QLabel>
#include <QMimeData>
#include <QRegularExpression>


TLDExtractor* TabManagerWidget::s_tldExtractor = nullptr;

TabManagerWidget::TabManagerWidget(BrowserWindow* mainClass, QWidget* parent, bool defaultWidget)
    : QWidget(parent)
    , ui(new Ui::TabManagerWidget)
    , m_window(mainClass)
    , m_webPage(nullptr)
    , m_isRefreshing(false)
    , m_refreshBlocked(false)
    , m_waitForRefresh(false)
    , m_isDefaultWidget(defaultWidget)
{
    if(s_tldExtractor == nullptr)
    {
        s_tldExtractor = TLDExtractor::instance();
        s_tldExtractor->setDataSearchPaths(QStringList() << TabManagerPlugin::settingsPath());
    }

    ui->setupUi(this);
    ui->treeWidget->setSelectionMode(QTreeWidget::SingleSelection);
    ui->treeWidget->setUniformRowHeights(true);
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->header()->hide();
    ui->treeWidget->header()->setStretchLastSection(false);
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->treeWidget->header()->resizeSection(1, 16);

    ui->treeWidget->setExpandsOnDoubleClick(false);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->treeWidget->installEventFilter(this);
    ui->filterBar->installEventFilter(this);

    auto* closeButton = new QPushButton(ui->filterBar);
    closeButton->setFlat(true);
    closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    ui->filterBar->addWidget(closeButton, LineEdit::RightSide);
    ui->filterBar->hide();

    ui->treeWidget->setItemDelegate(new TabManagerDelegate(ui->treeWidget));

    connect(closeButton, &QAbstractButton::clicked, this, &TabManagerWidget::filterBarClosed);
    connect(ui->filterBar, SIGNAL(textChanged(QString)), this, SLOT(filterChanged(QString)));
    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &TabManagerWidget::onItemActivated);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
    connect(ui->treeWidget, SIGNAL(requestRefreshTree()), this, SLOT(delayedRefreshTree()));
}

TabManagerWidget::~TabManagerWidget()
{
    delete ui;
}

void TabManagerWidget::setGroupType(GroupType type)
{
    m_groupType = type;
}

QString TabManagerWidget::domainFromUrl(const QUrl &url, bool useHostName)
{
    QString appendString = QL1S(":");
    QString urlString = url.toString();

    if (url.scheme() == QSL("file")) {
        return tr("Local File System:");
    }
    else if (url.scheme() == QSL("falkon") || urlString.isEmpty()) {
        return tr("Falkon:");
    }
    else if (url.scheme() == QSL("ftp")) {
        appendString.prepend(tr(" [FTP]"));
    }

    QString host = url.host();
    if (host.isEmpty()) {
        return urlString.append(appendString);
    }

    if (useHostName || QRegularExpression(QSL(R"(^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$)")).match(host).hasMatch()) {
        if (host.startsWith(QSL("www."), Qt::CaseInsensitive)) {
            host.remove(0, 4);
        }

        return host.append(appendString);
    }
    else {
        const QString registeredDomain = s_tldExtractor->registrableDomain(host);

        if (!registeredDomain.isEmpty()) {
            host = registeredDomain;
        }

        return host.append(appendString);
    }
}

void TabManagerWidget::delayedRefreshTree(WebPage* p)
{
    if (m_refreshBlocked || m_waitForRefresh) {
        return;
    }

    if (m_isRefreshing && !p) {
        return;
    }

    m_webPage = p;
    m_waitForRefresh = true;
    QTimer::singleShot(50, this, &TabManagerWidget::refreshTree);
}

void TabManagerWidget::refreshTree()
{
    if (m_refreshBlocked) {
        return;
    }

    if (m_isRefreshing && !m_webPage) {
        return;
    }

    // store selected items
    QList<QWidget*> selectedTabs;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* winItem = ui->treeWidget->topLevelItem(i);
        if (winItem->checkState(0) == Qt::Unchecked) {
            continue;
        }

        for (int j = 0; j < winItem->childCount(); ++j) {
            auto* tabItem = static_cast<TabItem*>(winItem->child(j));
            if (!tabItem || tabItem->checkState(0) == Qt::Unchecked) {
                continue;
            }
            selectedTabs << tabItem->webTab();
        }
    }

    ui->treeWidget->clear();
    ui->treeWidget->setEnableDragTabs(m_groupType == GroupByWindow);

    QTreeWidgetItem* currentTabItem = nullptr;

    if (m_groupType == GroupByHost) {
        currentTabItem = groupByDomainName(true);
    }
    else if (m_groupType == GroupByDomain) {
        currentTabItem = groupByDomainName();
    }
    else { // fallback to GroupByWindow
        m_groupType = GroupByWindow;
        currentTabItem = groupByWindow();
    }

    // restore selected items
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* winItem = ui->treeWidget->topLevelItem(i);

        for (int j = 0; j < winItem->childCount(); ++j) {
            auto* tabItem = static_cast<TabItem*>(winItem->child(j));

            if (tabItem && selectedTabs.contains(tabItem->webTab())) {
                tabItem->setCheckState(0, Qt::Checked);
            }
        }
    }

    filterChanged(m_filterText, true);
    ui->treeWidget->expandAll();

    if (currentTabItem)
        ui->treeWidget->scrollToItem(currentTabItem, QAbstractItemView::EnsureVisible);

    m_isRefreshing = false;
    m_waitForRefresh = false;
}

void TabManagerWidget::onItemActivated(QTreeWidgetItem* item, int column)
{
    auto* tabItem = static_cast<TabItem*>(item);
    if (!tabItem) {
        return;
    }

    BrowserWindow* mainWindow = tabItem->window();
    QWidget* tabWidget = tabItem->webTab();

    if (column == 1) {
        if (item->childCount() > 0)
            QMetaObject::invokeMethod(mainWindow ? mainWindow : mApp->getWindow(), "addTab");
        else if (tabWidget && mainWindow)
            mainWindow->tabWidget()->requestCloseTab(mainWindow->tabWidget()->indexOf(tabWidget));
        return;
    }

    if (!mainWindow) {
        return;
    }

    if (mainWindow->isMinimized()) {
        mainWindow->showNormal();
    }
    else {
        mainWindow->show();
    }
    mainWindow->activateWindow();
    mainWindow->raise();
    mainWindow->weView()->setFocus();

    if (tabWidget && tabWidget != mainWindow->tabWidget()->currentWidget()) {
        mainWindow->tabWidget()->setCurrentIndex(mainWindow->tabWidget()->indexOf(tabWidget));
    }
}

bool TabManagerWidget::isTabSelected()
{
    bool selected = false;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* parentItem = ui->treeWidget->topLevelItem(i);
        if (parentItem->checkState(0) != Qt::Unchecked) {
            selected = true;
            break;
        }
    }

    return selected;
}

void TabManagerWidget::customContextMenuRequested(const QPoint &pos)
{
    QMenu* menu = nullptr;

    auto* item = static_cast<TabItem*>(ui->treeWidget->itemAt(pos));

    if (item) {
        BrowserWindow* mainWindow = item->window();
        QWidget* tabWidget = item->webTab();

        if (mainWindow && tabWidget) {
            int index = mainWindow->tabWidget()->indexOf(tabWidget);

            // if items are not grouped by Window then actions "Close Other Tabs",
            // "Close Tabs To The Bottom" and "Close Tabs To The Top"
            // are ambiguous and should be hidden.
            TabContextMenu::Options options = TabContextMenu::VerticalTabs;
            if (m_groupType == GroupByWindow) {
                options |= TabContextMenu::ShowCloseOtherTabsActions;
            }
            menu = new TabContextMenu(index, mainWindow, options);
            menu->addSeparator();
        }
    }

    if (!menu)
        menu = new QMenu;

    menu->setAttribute(Qt::WA_DeleteOnClose);

    QAction* action;
    QMenu groupTypeSubmenu(tr("Group by"));
    action = groupTypeSubmenu.addAction(tr("&Window"), this, &TabManagerWidget::changeGroupType);
    action->setData(GroupByWindow);
    action->setCheckable(true);
    action->setChecked(m_groupType == GroupByWindow);

    action = groupTypeSubmenu.addAction(tr("&Domain"), this, &TabManagerWidget::changeGroupType);
    action->setData(GroupByDomain);
    action->setCheckable(true);
    action->setChecked(m_groupType == GroupByDomain);

    action = groupTypeSubmenu.addAction(tr("&Host"), this, &TabManagerWidget::changeGroupType);
    action->setData(GroupByHost);
    action->setCheckable(true);
    action->setChecked(m_groupType == GroupByHost);

    menu->addMenu(&groupTypeSubmenu);

    if (m_isDefaultWidget) {
        menu->addAction(QIcon(QSL(":/tabmanager/data/side-by-side.png")), tr("&Show side by side"), this, &TabManagerWidget::showSideBySide)->setObjectName("sideBySide");
    }

    menu->addSeparator();

    if (isTabSelected()) {
        menu->addAction(QIcon(QSL(":/tabmanager/data/tab-detach.png")), tr("&Detach checked tabs"), this, &TabManagerWidget::processActions)->setObjectName("detachSelection");
        menu->addAction(QIcon(QSL(":/tabmanager/data/tab-bookmark.png")), tr("Book&mark checked tabs"), this, &TabManagerWidget::processActions)->setObjectName("bookmarkSelection");
        menu->addAction(QIcon(QSL(":/tabmanager/data/tab-close.png")), tr("&Close checked tabs"), this, &TabManagerWidget::processActions)->setObjectName("closeSelection");
        menu->addAction(tr("&Unload checked tabs"), this, &TabManagerWidget::processActions)->setObjectName("unloadSelection");
    }

    menu->exec(ui->treeWidget->viewport()->mapToGlobal(pos));
}

void TabManagerWidget::filterChanged(const QString &filter, bool force)
{
    if (force || filter != m_filterText) {
        m_filterText = filter.simplified();
        ui->treeWidget->itemDelegate()->setProperty("filterText", m_filterText);
        if (m_filterText.isEmpty()) {
            for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
                QTreeWidgetItem* parentItem = ui->treeWidget->topLevelItem(i);
                for (int j = 0; j < parentItem->childCount(); ++j) {
                    QTreeWidgetItem* childItem = parentItem->child(j);
                    childItem->setHidden(false);
                }
                parentItem->setHidden(false);
                parentItem->setExpanded(true);
            }

            return;
        }

        const QRegularExpression filterRegExp(filter.simplified().replace(QL1C(' '), QLatin1String(".*"))
                                              .append(QLatin1String(".*")).prepend(QLatin1String(".*")),
                                              QRegularExpression::CaseInsensitiveOption);

        for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
            QTreeWidgetItem* parentItem = ui->treeWidget->topLevelItem(i);
            int visibleChildCount = 0;
            for (int j = 0; j < parentItem->childCount(); ++j) {
                auto* childItem = static_cast<TabItem*>(parentItem->child(j));
                if (!childItem) {
                    continue;
                }

                if (childItem->text(0).contains(filterRegExp) || childItem->webTab()->url().toString().simplified().contains(filterRegExp)) {
                    ++visibleChildCount;
                    childItem->setHidden(false);
                }
                else {
                    childItem->setHidden(true);
                }
            }

            if (visibleChildCount == 0) {
                parentItem->setHidden(true);
            }
            else {
                parentItem->setHidden(false);
                parentItem->setExpanded(true);
            }
        }
    }
}

void TabManagerWidget::filterBarClosed()
{
    ui->filterBar->clear();
    ui->filterBar->hide();
    ui->treeWidget->setFocusProxy(nullptr);
    ui->treeWidget->setFocus();
}

bool TabManagerWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        const QString text = keyEvent->text().simplified();

        if (obj == ui->treeWidget) {
            // switch to tab/window on enter
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
                onItemActivated(ui->treeWidget->currentItem(), 0);
                return QObject::eventFilter(obj, event);
            }

            if (!text.isEmpty() || ((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == Qt::Key_F)) {
                ui->filterBar->show();
                ui->treeWidget->setFocusProxy(ui->filterBar);
                ui->filterBar->setFocus();
                if (!text.isEmpty() && text.at(0).isPrint()) {
                    ui->filterBar->setText(ui->filterBar->text() + text);
                }

                return true;
            }
        }
        else if (obj == ui->filterBar) {
            bool isNavigationOrActionKey = keyEvent->key() == Qt::Key_Up ||
                    keyEvent->key() == Qt::Key_Down ||
                    keyEvent->key() == Qt::Key_PageDown ||
                    keyEvent->key() == Qt::Key_PageUp ||
                    keyEvent->key() == Qt::Key_Enter ||
                    keyEvent->key() == Qt::Key_Return;

            // send scroll or action press key to treeWidget
            if (isNavigationOrActionKey) {
                QKeyEvent ev(QKeyEvent::KeyPress, keyEvent->key(), keyEvent->modifiers());
                QApplication::sendEvent(ui->treeWidget, &ev);
                return false;
            }
        }
    }

    if (obj == ui->treeWidget && (event->type() == QEvent::Resize || event->type() == QEvent::Show))
        ui->treeWidget->setColumnHidden(1, ui->treeWidget->viewport()->width() < 150);

    return QObject::eventFilter(obj, event);
}

void TabManagerWidget::processActions()
{
    if (!sender()) {
        return;
    }

    m_refreshBlocked = true;

    QMultiHash<BrowserWindow*, WebTab*> selectedTabs;

    const QString &command = sender()->objectName();

    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* winItem = ui->treeWidget->topLevelItem(i);
        if (winItem->checkState(0) == Qt::Unchecked) {
            continue;
        }

        for (int j = 0; j < winItem->childCount(); ++j) {
            auto* tabItem = static_cast<TabItem*>(winItem->child(j));
            if (!tabItem || tabItem->checkState(0) == Qt::Unchecked) {
                continue;
            }

            BrowserWindow* mainWindow = tabItem->window();
            WebTab* webTab = tabItem->webTab();

            // current supported actions are not applied to pinned tabs
            if (webTab->isPinned()) {
                tabItem->setCheckState(0, Qt::Unchecked);
                continue;
            }

            selectedTabs.insert(mainWindow, webTab);
        }
        winItem->setCheckState(0, Qt::Unchecked);
    }

    if (!selectedTabs.isEmpty()) {
        if (command == QSL("closeSelection")) {
            closeSelectedTabs(selectedTabs);
        }
        else if (command == QSL("detachSelection")) {
            detachSelectedTabs(selectedTabs);
        }
        else if (command == QSL("bookmarkSelection")) {
            bookmarkSelectedTabs(selectedTabs);
        }
        else if (command == QSL("unloadSelection")) {
            unloadSelectedTabs(selectedTabs);
        }
    }

    m_refreshBlocked = false;
    delayedRefreshTree();
}

void TabManagerWidget::changeGroupType()
{
    auto* action = qobject_cast<QAction*>(sender());

    if (action) {
        int type = action->data().toInt();

        if (m_groupType != GroupType(type)) {
            m_groupType = GroupType(type);

            delayedRefreshTree();

            Q_EMIT groupTypeChanged(m_groupType);
        }
    }
}

void TabManagerWidget::closeSelectedTabs(const QMultiHash<BrowserWindow*, WebTab*> &tabsHash)
{
    if (tabsHash.isEmpty()) {
        return;
    }

    const QList<BrowserWindow*> &windows = tabsHash.uniqueKeys();
    for (BrowserWindow* mainWindow : windows) {
        const QList<WebTab*> tabs = tabsHash.values(mainWindow);

        for (WebTab* webTab : tabs) {
            mainWindow->tabWidget()->requestCloseTab(webTab->tabIndex());
        }
    }
}

static void detachTabsTo(BrowserWindow* targetWindow, const QMultiHash<BrowserWindow*, WebTab*> &tabsHash)
{
    const QList<BrowserWindow*> &windows = tabsHash.uniqueKeys();
    for (BrowserWindow* mainWindow : windows) {
        const QList<WebTab*> &tabs = tabsHash.values(mainWindow);
        for (WebTab* webTab : tabs) {
            mainWindow->tabWidget()->detachTab(webTab);

            if (mainWindow && mainWindow->tabCount() == 0) {
                mainWindow->close();
                mainWindow = nullptr;
            }

            targetWindow->tabWidget()->addView(webTab, Qz::NT_NotSelectedTab);
        }
    }
}

void TabManagerWidget::detachSelectedTabs(const QMultiHash<BrowserWindow*, WebTab*> &tabsHash)
{
    if (tabsHash.isEmpty() ||
            (tabsHash.uniqueKeys().size() == 1 &&
             tabsHash.size() == tabsHash.keys().at(0)->tabCount())) {
        return;
    }

    BrowserWindow* newWindow = mApp->createWindow(Qz::BW_OtherRestoredWindow);
    const QRect &availableGeometryForScreen = screen()->availableGeometry();
    newWindow->move(availableGeometryForScreen.topLeft() + QPoint(30, 30));

    detachTabsTo(newWindow, tabsHash);
}

bool TabManagerWidget::bookmarkSelectedTabs(const QMultiHash<BrowserWindow*, WebTab*> &tabsHash)
{
    auto* dialog = new QDialog(getWindow(), Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);
    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom, dialog);
    auto* label = new QLabel(dialog);
    auto* folderButton = new BookmarksFoldersButton(dialog);

    auto* box = new QDialogButtonBox(dialog);
    box->addButton(QDialogButtonBox::Ok);
    box->addButton(QDialogButtonBox::Cancel);
    QObject::connect(box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    QObject::connect(box, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    layout->addWidget(label);
    layout->addWidget(folderButton);
    layout->addWidget(box);

    label->setText(tr("Choose folder for bookmarks:"));
    dialog->setWindowTitle(tr("Bookmark Selected Tabs"));

    QSize size = dialog->size();
    size.setWidth(350);
    dialog->resize(size);
    dialog->exec();

    if (dialog->result() == QDialog::Rejected) {
        return false;
    }

    for (WebTab* tab : tabsHash) {
        if (!tab->url().isEmpty()) {
            auto* bookmark = new BookmarkItem(BookmarkItem::Url);
            bookmark->setTitle(tab->title());
            bookmark->setUrl(tab->url());
            mApp->bookmarks()->addBookmark(folderButton->selectedFolder(), bookmark);
        }
    }

    delete dialog;
    return true;
}

void TabManagerWidget::unloadSelectedTabs(const QMultiHash<BrowserWindow*, WebTab*> &tabsHash)
{
    if (tabsHash.isEmpty()) {
        return;
    }

    const QList<BrowserWindow*> &windows = tabsHash.uniqueKeys();
    for (BrowserWindow* mainWindow : windows) {
        const QList<WebTab*> tabs = tabsHash.values(mainWindow);

        for (WebTab* webTab : tabs) {
            mainWindow->tabWidget()->unloadTab(webTab->tabIndex());
        }
    }
}

QTreeWidgetItem* TabManagerWidget::groupByDomainName(bool useHostName)
{
    QTreeWidgetItem* currentTabItem = nullptr;

    QList<BrowserWindow*> windows = mApp->windows();
    int currentWindowIdx = windows.indexOf(getWindow());
    if (currentWindowIdx == -1) {
        // getWindow() instance is closing
        return nullptr;
    }

    QMap<QString, QTreeWidgetItem*> tabsGroupedByDomain;

    for (int win = 0; win < windows.count(); ++win) {
        BrowserWindow* mainWin = windows.at(win);

        QList<WebTab*> tabs = mainWin->tabWidget()->allTabs();

        for (int tab = 0; tab < tabs.count(); ++tab) {
            WebTab* webTab = tabs.at(tab);
            if (webTab->webView() && m_webPage == webTab->webView()->page()) {
                m_webPage = nullptr;
                continue;
            }
            QString domain = domainFromUrl(webTab->url(), useHostName);

            if (!tabsGroupedByDomain.contains(domain)) {
                auto* groupItem = new TabItem(ui->treeWidget, false, false, nullptr, false);
                groupItem->setTitle(domain);
                groupItem->setIsActiveOrCaption(true);

                tabsGroupedByDomain.insert(domain, groupItem);
            }

            QTreeWidgetItem* groupItem = tabsGroupedByDomain.value(domain);

            auto* tabItem = new TabItem(ui->treeWidget, false, true, groupItem);
            tabItem->setBrowserWindow(mainWin);
            tabItem->setWebTab(webTab);

            if (webTab == mainWin->weView()->webTab()) {
                tabItem->setIsActiveOrCaption(true);

                if (mainWin == getWindow())
                    currentTabItem = tabItem;
            }


            tabItem->updateIcon();
            tabItem->setTitle(webTab->title());
        }
    }

    ui->treeWidget->insertTopLevelItems(0, tabsGroupedByDomain.values());

    return currentTabItem;
}

QTreeWidgetItem* TabManagerWidget::groupByWindow()
{
    QTreeWidgetItem* currentTabItem = nullptr;

    QList<BrowserWindow*> windows = mApp->windows();
    int currentWindowIdx = windows.indexOf(getWindow());
    if (currentWindowIdx == -1) {
        return nullptr;
    }
    m_isRefreshing = true;

    if (!m_isDefaultWidget) {
        windows.move(currentWindowIdx, 0);
        currentWindowIdx = 0;
    }

    for (int win = 0; win < windows.count(); ++win) {
        BrowserWindow* mainWin = windows.at(win);
        auto* winItem = new TabItem(ui->treeWidget, true, false);
        winItem->setBrowserWindow(mainWin);
        winItem->setText(0, tr("Window %1").arg(QString::number(win + 1)));
        winItem->setToolTip(0, tr("Double click to switch"));
        winItem->setIsActiveOrCaption(win == currentWindowIdx);

        QList<WebTab*> tabs = mainWin->tabWidget()->allTabs();

        for (int tab = 0; tab < tabs.count(); ++tab) {
            WebTab* webTab = tabs.at(tab);
            if (webTab->webView() && m_webPage == webTab->webView()->page()) {
                m_webPage = nullptr;
                continue;
            }
            auto* tabItem = new TabItem(ui->treeWidget, true, true, winItem);
            tabItem->setBrowserWindow(mainWin);
            tabItem->setWebTab(webTab);

            if (webTab == mainWin->weView()->webTab()) {
                tabItem->setIsActiveOrCaption(true);

                if (mainWin == getWindow())
                    currentTabItem = tabItem;
            }

            tabItem->updateIcon();
            tabItem->setTitle(webTab->title());
        }
    }

    return currentTabItem;
}

BrowserWindow* TabManagerWidget::getWindow()
{
    if (m_isDefaultWidget || !m_window) {
        return mApp->getWindow();
    }
    else {
        return m_window.data();
    }
}

TabItem::TabItem(QTreeWidget* treeWidget, bool supportDrag, bool isTab, QTreeWidgetItem* parent, bool addToTree)
    : QObject()
    , QTreeWidgetItem(addToTree ? (parent ? parent : treeWidget->invisibleRootItem()) : nullptr, 1)
    , m_treeWidget(treeWidget)
    , m_window(nullptr)
    , m_webTab(nullptr)
    , m_isTab(isTab)
{
    Qt::ItemFlags flgs = flags() | (parent ? Qt::ItemIsUserCheckable : Qt::ItemIsUserCheckable | Qt::ItemIsAutoTristate);

    if (supportDrag) {
        if (isTab) {
            flgs |= Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren;
            flgs &= ~Qt::ItemIsDropEnabled;
        }
        else {
            flgs |= Qt::ItemIsDropEnabled;
            flgs &= ~Qt::ItemIsDragEnabled;
        }
    }

    setFlags(flgs);

    setCheckState(0, Qt::Unchecked);
}

BrowserWindow* TabItem::window() const
{
    return m_window;
}

void TabItem::setBrowserWindow(BrowserWindow* window)
{
    m_window = window;
}

WebTab* TabItem::webTab() const
{
    return m_webTab;
}

void TabItem::setWebTab(WebTab* webTab)
{
    m_webTab = webTab;

    if (m_webTab->isRestored())
        setIsActiveOrCaption(m_webTab->isCurrentTab());
    else
        setIsSavedTab(true);

    connect(m_webTab->webView(), &QWebEngineView::titleChanged, this, &TabItem::setTitle);
    connect(m_webTab->webView(), &QWebEngineView::iconChanged, this, &TabItem::updateIcon);

    auto pageChanged = [this](WebPage *page) {
        connect(page, &WebPage::audioMutedChanged, this, &TabItem::updateIcon);
        connect(page, &WebPage::loadFinished, this, &TabItem::updateIcon);
        connect(page, &WebPage::loadStarted, this, &TabItem::updateIcon);
    };
    pageChanged(m_webTab->webView()->page());
    connect(m_webTab->webView(), &WebView::pageChanged, this, pageChanged);
}

void TabItem::updateIcon()
{
    if (!m_webTab)
        return;

    if (!m_webTab->isLoading()) {
        if (!m_webTab->isPinned()) {
            if (m_webTab->isMuted()) {
                setIcon(0, QIcon::fromTheme(QSL("audio-volume-muted"), QIcon(QSL(":icons/other/audiomuted.svg"))));
            }
            else if (!m_webTab->isMuted() && m_webTab->webView()->page()->recentlyAudible()) {
                setIcon(0, QIcon::fromTheme(QSL("audio-volume-high"), QIcon(QSL(":icons/other/audioplaying.svg"))));
            }
            else {
                setIcon(0, m_webTab->icon());
            }
        }
        else {
            setIcon(0, QIcon(QSL(":tabmanager/data/tab-pinned.png")));
        }

        if (m_webTab->isRestored())
            setIsActiveOrCaption(m_webTab->isCurrentTab());
        else
            setIsSavedTab(true);
    }
    else {
        setIcon(0, QIcon(QSL(":tabmanager/data/tab-loading.png")));
        setIsActiveOrCaption(m_webTab->isCurrentTab());
    }
}

void TabItem::setTitle(const QString &title)
{
    setText(0, title);
    setToolTip(0, title);
}

void TabItem::setIsActiveOrCaption(bool yes)
{
    setData(0, ActiveOrCaptionRole, yes ? QVariant(true) : QVariant());

    setIsSavedTab(false);
}

void TabItem::setIsSavedTab(bool yes)
{
    setData(0, SavedRole, yes ? QVariant(true) : QVariant());
}

bool TabItem::isTab() const
{
    return m_isTab;
}

TabTreeWidget::TabTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    invisibleRootItem()->setFlags(invisibleRootItem()->flags() & ~Qt::ItemIsDropEnabled);
}

Qt::DropActions TabTreeWidget::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

#define MIMETYPE QLatin1String("application/falkon.tabs")

QStringList TabTreeWidget::mimeTypes() const
{
    QStringList types;
    types.append(MIMETYPE);
    return types;
}

QMimeData *TabTreeWidget::mimeData(const QList<QTreeWidgetItem*> &items) const
{
    auto* mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    if (items.size() > 0) {
        auto* tabItem = static_cast<TabItem*>(items.at(0));
        if (!tabItem || !tabItem->isTab())
            return nullptr;

        stream << (quintptr) tabItem->window() << (quintptr) tabItem->webTab();

        mimeData->setData(MIMETYPE, encodedData);

        return mimeData;
    }

    return nullptr;
}

bool TabTreeWidget::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
{
    if (action == Qt::IgnoreAction) {
        return true;
    }

    auto* parentItem = static_cast<TabItem*>(parent);

    if (!data->hasFormat(MIMETYPE) || !parentItem) {
        return false;
    }

    Q_ASSERT(!parentItem->isTab());

    BrowserWindow* targetWindow = parentItem->window();

    QByteArray encodedData = data->data(MIMETYPE);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    if (!stream.atEnd()) {
        quintptr webTabPtr;
        quintptr windowPtr;

        stream >> windowPtr >> webTabPtr;

        auto* webTab = (WebTab*) webTabPtr;
        auto* window = (BrowserWindow*) windowPtr;

        if (window == targetWindow) {
            if (index > 0 && webTab->tabIndex() < index)
                --index;

            if (webTab->isPinned() && index >= targetWindow->tabWidget()->pinnedTabsCount())
                index = targetWindow->tabWidget()->pinnedTabsCount() - 1;

            if (!webTab->isPinned() && index < targetWindow->tabWidget()->pinnedTabsCount())
                index = targetWindow->tabWidget()->pinnedTabsCount();

            if (index != webTab->tabIndex()) {
                targetWindow->tabWidget()->tabBar()->moveTab(webTab->tabIndex(), index);

                if (!webTab->isCurrentTab())
                    Q_EMIT requestRefreshTree();
            }
            else {
                return false;
            }
        }
        else if (!webTab->isPinned()) {
            QMultiHash<BrowserWindow*, WebTab*> tabsHash;
            tabsHash.insert(window, webTab);

            detachTabsTo(targetWindow, tabsHash);

            if (index < targetWindow->tabWidget()->pinnedTabsCount())
                index = targetWindow->tabWidget()->pinnedTabsCount();

            targetWindow->tabWidget()->tabBar()->moveTab(webTab->tabIndex(), index);
        }
    }

    return true;
}

void TabTreeWidget::setEnableDragTabs(bool enable)
{
    setDragEnabled(enable);
    setAcceptDrops(enable);
    viewport()->setAcceptDrops(enable);
    setDropIndicatorShown(enable);
}
