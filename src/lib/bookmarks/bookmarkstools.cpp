/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2014-2017 David Rosca <nowrep@gmail.com>
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
#include "bookmarkstools.h"
#include "bookmarkitem.h"
#include "bookmarks.h"
#include "mainapplication.h"
#include "iconprovider.h"
#include "enhancedmenu.h"
#include "tabwidget.h"
#include "qzsettings.h"
#include "browserwindow.h"
#include "sqldatabase.h"
#include "statusbar.h"

#include <iostream>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QStyle>
#include <QDialog>
#include <QMessageBox>

// BookmarksFoldersMenu
BookmarksFoldersMenu::BookmarksFoldersMenu(QWidget* parent)
    : QMenu(parent)
    , m_selectedFolder(nullptr)
{
    init();
}

BookmarkItem* BookmarksFoldersMenu::selectedFolder() const
{
    return m_selectedFolder;
}

void BookmarksFoldersMenu::folderChoosed()
{
    if (auto* act = qobject_cast<QAction*>(sender())) {
        BookmarkItem* folder = static_cast<BookmarkItem*>(act->data().value<void*>());
        Q_EMIT folderSelected(folder);
    }
}

void BookmarksFoldersMenu::init()
{
#define ADD_MENU(name) \
    BookmarkItem* f_##name = mApp->bookmarks()->name(); \
    QMenu* m_##name = addMenu(f_##name->icon(), f_##name->title()); \
    createMenu(m_##name, f_##name);

    ADD_MENU(toolbarFolder)
    ADD_MENU(menuFolder)
    ADD_MENU(unsortedFolder)
#undef ADD_MENU
}

void BookmarksFoldersMenu::createMenu(QMenu* menu, BookmarkItem* parent)
{
    QAction* act = menu->addAction(tr("Choose %1").arg(parent->title()));
    act->setData(QVariant::fromValue<void*>(static_cast<void*>(parent)));
    connect(act, &QAction::triggered, this, &BookmarksFoldersMenu::folderChoosed);

    menu->addSeparator();

    const auto children = parent->children();
    for (BookmarkItem* child : children) {
        if (child->isFolder()) {
            QMenu* m = menu->addMenu(child->icon(), child->title());
            createMenu(m, child);
        }
    }
}


// BookmarksFoldersButton
BookmarksFoldersButton::BookmarksFoldersButton(QWidget* parent, BookmarkItem* folder)
    : QPushButton(parent)
    , m_menu(new BookmarksFoldersMenu(this))
    , m_selectedFolder(folder ? folder : mApp->bookmarks()->lastUsedFolder())
{
    init();

    connect(m_menu, &BookmarksFoldersMenu::folderSelected, this, &BookmarksFoldersButton::setSelectedFolder);
}

BookmarkItem* BookmarksFoldersButton::selectedFolder() const
{
    return m_selectedFolder;
}

void BookmarksFoldersButton::setSelectedFolder(BookmarkItem* folder)
{
    Q_ASSERT(folder);
    Q_ASSERT(folder->isFolder());

    m_selectedFolder = folder;
    setText(folder->title());
    setIcon(folder->icon());

    if (sender()) {
        Q_EMIT selectedFolderChanged(folder);
    }
}

void BookmarksFoldersButton::init()
{
    setMenu(m_menu);
    setSelectedFolder(m_selectedFolder);
}


// BookmarksTools
bool BookmarksTools::addBookmarkDialog(QWidget* parent, const QUrl &url, const QString &title, BookmarkItem* folder)
{
    if (url.isEmpty() || title.isEmpty()) {
        return false;
    }

    auto* dialog = new QDialog(parent);
    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom, dialog);
    auto* label = new QLabel(dialog);
    auto* edit = new QLineEdit(dialog);
    auto* folderButton = new BookmarksFoldersButton(dialog, folder);

    auto* box = new QDialogButtonBox(dialog);
    box->addButton(QDialogButtonBox::Ok);
    box->addButton(QDialogButtonBox::Cancel);
    QObject::connect(box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    QObject::connect(box, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    layout->addWidget(label);
    layout->addWidget(edit);
    layout->addWidget(folderButton);
    layout->addWidget(box);

    label->setText(Bookmarks::tr("Choose name and location of this bookmark."));
    edit->setText(title);
    edit->setCursorPosition(0);
    dialog->setWindowIcon(IconProvider::iconForUrl(url));
    dialog->setWindowTitle(Bookmarks::tr("Add New Bookmark"));

    QSize size = dialog->size();
    size.setWidth(350);
    dialog->resize(size);
    dialog->exec();

    if (dialog->result() == QDialog::Rejected || edit->text().isEmpty()) {
        delete dialog;
        return false;
    }

    auto* bookmark = new BookmarkItem(BookmarkItem::Url);
    bookmark->setTitle(edit->text());
    bookmark->setUrl(url);
    mApp->bookmarks()->addBookmark(folderButton->selectedFolder(), bookmark);

    delete dialog;
    return true;
}

bool BookmarksTools::bookmarkAllTabsDialog(QWidget* parent, TabWidget* tabWidget, BookmarkItem* folder)
{
    Q_ASSERT(tabWidget);

    auto* dialog = new QDialog(parent);
    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom, dialog);
    auto* label = new QLabel(dialog);
    auto* folderButton = new BookmarksFoldersButton(dialog, folder);

    auto* box = new QDialogButtonBox(dialog);
    box->addButton(QDialogButtonBox::Ok);
    box->addButton(QDialogButtonBox::Cancel);
    QObject::connect(box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    QObject::connect(box, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    layout->addWidget(label);
    layout->addWidget(folderButton);
    layout->addWidget(box);

    label->setText(Bookmarks::tr("Choose folder for bookmarks:"));
    dialog->setWindowTitle(Bookmarks::tr("Bookmark All Tabs"));

    QSize size = dialog->size();
    size.setWidth(350);
    dialog->resize(size);
    dialog->exec();

    if (dialog->result() == QDialog::Rejected) {
        return false;
    }

    const auto allTabs = tabWidget->allTabs(false);
    for (WebTab* tab : allTabs) {
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

bool BookmarksTools::editBookmarkDialog(QWidget* parent, BookmarkItem *item)
{
    auto* dialog = new QDialog(parent);
    auto* layout = new QFormLayout(dialog);

    auto* title = new QLineEdit;
    auto* address = new QLineEdit;
    auto* keyword = new QLineEdit;
    auto* description = new QPlainTextEdit;

    auto* box = new QDialogButtonBox(dialog);
    box->addButton(QDialogButtonBox::Ok);
    box->addButton(QDialogButtonBox::Cancel);
    QObject::connect(box, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    QObject::connect(box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    layout->addRow(Bookmarks::tr("Title:"), title);
    title->setText(item->title());
    if (!item->isFolder()) {
        layout->addRow(Bookmarks::tr("Address:"), address);
        address->setText(item->urlString());
        layout->addRow(Bookmarks::tr("Keyword:"), keyword);
        keyword->setText(item->keyword());
    }
    layout->addRow(Bookmarks::tr("Description:"), description);
    description->document()->setPlainText(item->description());
    layout->addWidget(box);

    dialog->setWindowIcon(item->icon());
    dialog->setWindowTitle(Bookmarks::tr("Edit Bookmark"));

    dialog->exec();

    if (dialog->result() == QDialog::Rejected) {
        delete dialog;
        return false;
    }

    item->setTitle(title->text());
    if (!item->isFolder()) {
        item->setUrl(QUrl::fromEncoded(address->text().toUtf8()));
        item->setKeyword(keyword->text());
    }
    item->setDescription(description->toPlainText());

    delete dialog;
    return true;
}

void BookmarksTools::openBookmark(BrowserWindow* window, BookmarkItem* item)
{
    Q_ASSERT(window);

    if (!item || !item->isUrl()) {
        return;
    }

    if (item->isFolder()) {
        openFolderInTabs(window, item);
    }
    else if (item->isUrl()) {
        item->updateVisitCount();
        window->loadAddress(item->url());
    }
}

void BookmarksTools::openBookmarkInNewTab(BrowserWindow* window, BookmarkItem* item)
{
    Q_ASSERT(window);

    if (!item) {
        return;
    }

    if (item->isFolder()) {
        openFolderInTabs(window, item);
    }
    else if (item->isUrl()) {
        item->updateVisitCount();
        window->tabWidget()->addView(item->url(), item->title(), qzSettings->newTabPosition);
    }
}

void BookmarksTools::openBookmarkInNewWindow(BookmarkItem* item)
{
    if (!item->isUrl()) {
        return;
    }

    item->updateVisitCount();
    mApp->createWindow(Qz::BW_NewWindow, item->url());
}

void BookmarksTools::openBookmarkInNewPrivateWindow(BookmarkItem* item)
{
    if (!item->isUrl()) {
        return;
    }

    item->updateVisitCount();
    mApp->startPrivateBrowsing(item->url());
}

void BookmarksTools::openFolderInTabs(BrowserWindow* window, BookmarkItem* folder)
{
    Q_ASSERT(window);
    Q_ASSERT(folder->isFolder());

    const auto children = folder->children();

    bool showWarning = folder->children().size() > 10;
    if (!showWarning) {
        for (BookmarkItem* child : children) {
            if (child->isFolder()) {
                showWarning = true;
                break;
            }
        }
    }

    if (showWarning) {
        const auto button = QMessageBox::warning(window, Bookmarks::tr("Confirmation"),
                                                 Bookmarks::tr("Are you sure you want to open all bookmarks from '%1' folder in tabs?").arg(folder->title()),
                                                 QMessageBox::Yes | QMessageBox::No);
        if (button != QMessageBox::Yes) {
            return;
        }
    }

    for (BookmarkItem* child : children) {
        if (child->isUrl()) {
            openBookmarkInNewTab(window, child);
        }
        else if (child->isFolder()) {
            openFolderInTabs(window, child);
        }
    }
}

void BookmarksTools::addActionToMenu(QObject* receiver, Menu* menu, BookmarkItem* item)
{
    Q_ASSERT(menu);
    Q_ASSERT(item);

    switch (item->type()) {
    case BookmarkItem::Url:
        addUrlToMenu(receiver, menu, item);
        break;
    case BookmarkItem::Folder:
        addFolderToMenu(receiver, menu, item);
        break;
    case BookmarkItem::Separator:
        addSeparatorToMenu(menu, item);
        break;
    default:
        break;
    }
}

void BookmarksTools::addFolderToMenu(QObject* receiver, Menu* menu, BookmarkItem* folder)
{
    Q_ASSERT(menu);
    Q_ASSERT(folder);
    Q_ASSERT(folder->isFolder());

    Menu* m = new Menu(menu);
    QString title = QFontMetrics(m->font()).elidedText(folder->title(), Qt::ElideRight, 250);
    m->setTitle(title);
    m->setIcon(folder->icon());

    QObject::connect(m, &QMenu::aboutToHide, [=]() {
        mApp->getWindow()->statusBar()->clearMessage();
    });

    addFolderContentsToMenu(receiver, m, folder);

    QAction* act = menu->addMenu(m);
    act->setData(QVariant::fromValue<void*>(static_cast<void*>(folder)));
    act->setIconVisibleInMenu(true);

    QObject::connect(act, &QAction::hovered, [=]() {
        mApp->getWindow()->statusBar()->clearMessage();
    });
}

void BookmarksTools::addUrlToMenu(QObject* receiver, Menu* menu, BookmarkItem* bookmark)
{
    Q_ASSERT(menu);
    Q_ASSERT(bookmark);
    Q_ASSERT(bookmark->isUrl());

    auto* act = new Action(menu);
    QString title = QFontMetrics(act->font()).elidedText(bookmark->title(), Qt::ElideRight, 250);
    act->setText(title);
    act->setData(QVariant::fromValue<void*>(static_cast<void*>(bookmark)));
    act->setIconVisibleInMenu(true);

    QObject::connect(act, SIGNAL(triggered()), receiver, SLOT(bookmarkActivated()));
    QObject::connect(act, SIGNAL(ctrlTriggered()), receiver, SLOT(bookmarkCtrlActivated()));
    QObject::connect(act, SIGNAL(shiftTriggered()), receiver, SLOT(bookmarkShiftActivated()));

    QObject::connect(act, &QAction::hovered, [=]() {
        mApp->getWindow()->statusBar()->showMessage(bookmark->url().toString());
    });

    menu->addAction(act);
}

void BookmarksTools::addSeparatorToMenu(Menu* menu, BookmarkItem* separator)
{
    Q_UNUSED(separator)

    Q_ASSERT(menu);
    Q_ASSERT(separator->isSeparator());

    auto* act = menu->addSeparator();
    QObject::connect(act, &QAction::hovered, [=]() {
        mApp->getWindow()->statusBar()->clearMessage();
    });
}

void BookmarksTools::addFolderContentsToMenu(QObject *receiver, Menu *menu, BookmarkItem *folder)
{
    QObject::connect(menu, SIGNAL(aboutToShow()), receiver, SLOT(menuAboutToShow()));
    QObject::connect(menu, SIGNAL(menuMiddleClicked(Menu*)), receiver, SLOT(menuMiddleClicked(Menu*)));

    const auto children = folder->children();
    for (BookmarkItem* child : children) {
        addActionToMenu(receiver, menu, child);
    }

    if (menu->isEmpty()) {
        menu->addAction(Bookmarks::tr("Empty"))->setDisabled(true);
    }
}

bool BookmarksTools::migrateBookmarksIfNecessary(Bookmarks* bookmarks)
{
    QSqlQuery query(SqlDatabase::instance()->database());
    query.exec(QSL("SELECT name FROM sqlite_master WHERE type='table' AND name='folders'"));

    if (!query.next()) {
        return false;
    }

    std::cout << "Bookmarks: Migrating your bookmarks from SQLite to JSON..." << std::endl;

    QHash<QString, BookmarkItem*> folders;
    folders.insert(QSL("bookmarksToolbar"), bookmarks->toolbarFolder());
    folders.insert(QSL("bookmarksMenu"), bookmarks->menuFolder());
    folders.insert(QSL("unsorted"), bookmarks->unsortedFolder());

    query.exec(QSL("SELECT name, subfolder FROM folders"));
    while (query.next()) {
        const QString title = query.value(0).toString();
        bool subfolder = query.value(1).toString() == QLatin1String("yes");

        BookmarkItem* parent = subfolder ? bookmarks->toolbarFolder() : bookmarks->unsortedFolder();
        auto* folder = new BookmarkItem(BookmarkItem::Folder, parent);
        folder->setTitle(title);
        folders.insert(folder->title(), folder);
    }

    query.exec(QSL("SELECT title, folder, url FROM bookmarks ORDER BY position ASC"));
    while (query.next()) {
        const QString title = query.value(0).toString();
        const QString folder = query.value(1).toString();
        const QUrl url = query.value(2).toUrl();

        BookmarkItem* parent = folders.value(folder);
        if (!parent) {
            parent = bookmarks->unsortedFolder();
        }
        Q_ASSERT(parent);

        auto* bookmark = new BookmarkItem(BookmarkItem::Url, parent);
        bookmark->setTitle(title);
        bookmark->setUrl(url);
    }

    query.exec(QSL("DROP TABLE folders"));
    query.exec(QSL("DROP TABLE bookmarks"));
    query.exec(QSL("VACUUM"));

    std::cout << "Bookmarks: Bookmarks successfully migrated!" << std::endl;
    return true;
}
