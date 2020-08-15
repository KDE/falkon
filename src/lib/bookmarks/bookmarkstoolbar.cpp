/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "bookmarkstoolbar.h"
#include "bookmarkstoolbarbutton.h"
#include "bookmarkstools.h"
#include "bookmarksmodel.h"
#include "bookmarkitem.h"
#include "bookmarks.h"
#include "mainapplication.h"
#include "iconprovider.h"
#include "qztools.h"

#include <QDragEnterEvent>
#include <QHBoxLayout>
#include <QMimeData>
#include <QTimer>
#include <QFrame>
#include <QInputDialog>

BookmarksToolbar::BookmarksToolbar(BrowserWindow* window, QWidget* parent)
    : QWidget(parent)
    , m_window(window)
    , m_bookmarks(mApp->bookmarks())
    , m_clickedBookmark(0)
    , m_dropRow(-1)
{
    setObjectName("bookmarksbar");
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    m_layout = new QHBoxLayout(this);
    m_layout->setMargin(style()->pixelMetric(QStyle::PM_ToolBarItemMargin, 0, this)
                          + style()->pixelMetric(QStyle::PM_ToolBarFrameWidth, 0, this));
    m_layout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing, 0, this));
    setLayout(m_layout);

    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(300);
    m_updateTimer->setSingleShot(true);
    connect(m_updateTimer, &QTimer::timeout, this, &BookmarksToolbar::refresh);

    connect(m_bookmarks, &Bookmarks::bookmarkAdded, this, &BookmarksToolbar::bookmarksChanged);
    connect(m_bookmarks, &Bookmarks::bookmarkRemoved, this, &BookmarksToolbar::bookmarksChanged);
    connect(m_bookmarks, &Bookmarks::bookmarkChanged, this, &BookmarksToolbar::bookmarksChanged);
    connect(m_bookmarks, &Bookmarks::showOnlyIconsInToolbarChanged, this, &BookmarksToolbar::showOnlyIconsChanged);
    connect(m_bookmarks, &Bookmarks::showOnlyTextInToolbarChanged, this, &BookmarksToolbar::showOnlyTextChanged);
    connect(this, &QWidget::customContextMenuRequested, this, &BookmarksToolbar::contextMenuRequested);

    refresh();
}

void BookmarksToolbar::contextMenuRequested(const QPoint &pos)
{
    BookmarksToolbarButton* button = buttonAt(pos);
    m_clickedBookmark = button ? button->bookmark() : 0;

    QMenu menu;
    QAction* actNewTab = menu.addAction(IconProvider::newTabIcon(), tr("Open in new tab"));
    QAction* actNewWindow = menu.addAction(IconProvider::newWindowIcon(), tr("Open in new window"));
    QAction* actNewPrivateWindow = menu.addAction(IconProvider::privateBrowsingIcon(), tr("Open in new private window"));
    menu.addSeparator();
    QAction* actNewFolder = menu.addAction(QIcon::fromTheme("folder-new"), tr("New Folder"));
    QAction* actEdit = menu.addAction(tr("Edit"));
    QAction* actDelete = menu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"));
    menu.addSeparator();
    m_actShowOnlyIcons = menu.addAction(tr("Show Only Icons"));
    m_actShowOnlyIcons->setCheckable(true);
    m_actShowOnlyIcons->setChecked(m_bookmarks->showOnlyIconsInToolbar());
    connect(m_actShowOnlyIcons, &QAction::toggled, m_bookmarks, &Bookmarks::setShowOnlyIconsInToolbar);
    m_actShowOnlyText = menu.addAction(tr("Show Only Text"));
    m_actShowOnlyText->setCheckable(true);
    m_actShowOnlyText->setChecked(m_bookmarks->showOnlyTextInToolbar());
    connect(m_actShowOnlyText, &QAction::toggled, m_bookmarks, &Bookmarks::setShowOnlyTextInToolbar);

    connect(actNewTab, &QAction::triggered, this, &BookmarksToolbar::openBookmarkInNewTab);
    connect(actNewWindow, &QAction::triggered, this, &BookmarksToolbar::openBookmarkInNewWindow);
    connect(actNewPrivateWindow, &QAction::triggered, this, &BookmarksToolbar::openBookmarkInNewPrivateWindow);
    connect(actNewFolder, &QAction::triggered, this, &BookmarksToolbar::createNewFolder);
    connect(actEdit, &QAction::triggered, this, &BookmarksToolbar::editBookmark);
    connect(actDelete, &QAction::triggered, this, &BookmarksToolbar::deleteBookmark);

    actEdit->setEnabled(m_clickedBookmark && m_bookmarks->canBeModified(m_clickedBookmark));
    actDelete->setEnabled(m_clickedBookmark && m_bookmarks->canBeModified(m_clickedBookmark));
    actNewTab->setEnabled(m_clickedBookmark && m_clickedBookmark->isUrl());
    actNewWindow->setEnabled(m_clickedBookmark && m_clickedBookmark->isUrl());
    actNewPrivateWindow->setEnabled(m_clickedBookmark && m_clickedBookmark->isUrl());

    menu.exec(mapToGlobal(pos));

    if (button) {
        // Clear mouseover state after closing menu
        button->update();
    }

    m_clickedBookmark = nullptr;
    m_actShowOnlyIcons = nullptr;
    m_actShowOnlyText = nullptr;
}

void BookmarksToolbar::refresh()
{
    clear();

    BookmarkItem* folder = mApp->bookmarks()->toolbarFolder();

    const auto children = folder->children();
    for (BookmarkItem* child : children) {
        addItem(child);
    }

    m_layout->addStretch();
}

void BookmarksToolbar::bookmarksChanged()
{
    m_updateTimer->start();
}

void BookmarksToolbar::showOnlyIconsChanged(bool state)
{
    if (state && m_actShowOnlyText) {
        m_actShowOnlyText->setChecked(false);
    }

    for (int i = 0; i < m_layout->count(); ++i) {
        BookmarksToolbarButton* b = qobject_cast<BookmarksToolbarButton*>(m_layout->itemAt(i)->widget());
        if (b) {
            b->setShowOnlyIcon(state);
        }
    }
}

void BookmarksToolbar::showOnlyTextChanged(bool state)
{
    if (state && m_actShowOnlyIcons) {
        m_actShowOnlyIcons->setChecked(false);
    }

    for (int i = 0; i < m_layout->count(); ++i) {
        BookmarksToolbarButton* b = qobject_cast<BookmarksToolbarButton*>(m_layout->itemAt(i)->widget());
        if (b) {
            b->setShowOnlyText(state);
        }
    }
}

void BookmarksToolbar::openBookmarkInNewTab()
{
    if (m_clickedBookmark) {
        BookmarksTools::openBookmarkInNewTab(m_window, m_clickedBookmark);
    }
}

void BookmarksToolbar::openBookmarkInNewWindow()
{
    if (m_clickedBookmark) {
        BookmarksTools::openBookmarkInNewWindow(m_clickedBookmark);
    }
}

void BookmarksToolbar::openBookmarkInNewPrivateWindow()
{
    if (m_clickedBookmark) {
        BookmarksTools::openBookmarkInNewPrivateWindow(m_clickedBookmark);
    }
}

void BookmarksToolbar::createNewFolder() 
{
    QString name = QInputDialog::getText(nullptr, tr("New Folder"), tr("Enter Folder Name:"));
    
    if (!name.isEmpty()) {
        BookmarkItem* parent;
        
        if (m_clickedBookmark) {
            parent = m_clickedBookmark;
        } else {
            parent = m_bookmarks->toolbarFolder();
        }
        
        auto folder = new BookmarkItem(BookmarkItem::Folder);
        folder->setTitle(name);
        m_bookmarks->addBookmark(parent, folder);
    }
}

void BookmarksToolbar::editBookmark()
{
    if (m_clickedBookmark) {
        BookmarksTools::editBookmarkDialog(this, m_clickedBookmark);
        m_bookmarks->changeBookmark(m_clickedBookmark);
    }
}

void BookmarksToolbar::deleteBookmark()
{
    if (m_clickedBookmark) {
        m_bookmarks->removeBookmark(m_clickedBookmark);
    }
}

void BookmarksToolbar::clear()
{
    int count = m_layout->count();

    for (int i = 0; i < count; ++i) {
        QLayoutItem* item = m_layout->takeAt(0);
        delete item->widget();
        delete item;
    }

    Q_ASSERT(m_layout->isEmpty());
}

void BookmarksToolbar::addItem(BookmarkItem* item)
{
    Q_ASSERT(item);

    BookmarksToolbarButton* button = new BookmarksToolbarButton(item, this);
    button->setMainWindow(m_window);
    button->setShowOnlyIcon(m_bookmarks->showOnlyIconsInToolbar());
    button->setShowOnlyText(m_bookmarks->showOnlyTextInToolbar());
    m_layout->addWidget(button);
}

BookmarksToolbarButton* BookmarksToolbar::buttonAt(const QPoint &pos)
{
    return qobject_cast<BookmarksToolbarButton*>(QApplication::widgetAt(mapToGlobal(pos)));
}

QSize BookmarksToolbar::minimumSizeHint() const
{
    QSize size = QWidget::minimumSizeHint();
    size.setHeight(qMax(20, size.height()));
    return size;
}

void BookmarksToolbar::dropEvent(QDropEvent* e)
{
    int row = m_dropRow;
    clearDropIndicator();

    const QMimeData* mime = e->mimeData();

    if (!mime->hasUrls() && !mime->hasFormat(BookmarksButtonMimeData::mimeType())) {
        QWidget::dropEvent(e);
        return;
    }

    BookmarkItem* parent = m_bookmarks->toolbarFolder();
    BookmarkItem* bookmark = nullptr;

    if (mime->hasFormat(BookmarksButtonMimeData::mimeType())) {
        const BookmarksButtonMimeData* bookmarkMime = static_cast<const BookmarksButtonMimeData*>(mime);
        bookmark = bookmarkMime->item();
        const int initialIndex = bookmark->parent()->children().indexOf(bookmark);
        BookmarksToolbarButton* current = buttonAt(m_dropPos);
        if (initialIndex < m_layout->indexOf(current)) {
            row -= 1;
        }
    } else {
        const QUrl url = mime->urls().at(0);
        const QString title = mime->hasText() ? mime->text() : url.toEncoded(QUrl::RemoveScheme);

        bookmark = new BookmarkItem(BookmarkItem::Url);
        bookmark->setTitle(title);
        bookmark->setUrl(url);
    }

    if (row >= 0) {
        m_bookmarks->insertBookmark(parent, row, bookmark);
    } else {
        m_bookmarks->addBookmark(parent, bookmark);
    }
}

void BookmarksToolbar::dragEnterEvent(QDragEnterEvent* e)
{
    const QMimeData* mime = e->mimeData();

    if ((mime->hasUrls() && mime->hasText()) || mime->hasFormat(BookmarksButtonMimeData::mimeType())) {
        e->acceptProposedAction();
        return;
    }

    QWidget::dragEnterEvent(e);
}

void BookmarksToolbar::dragMoveEvent(QDragMoveEvent *e)
{
    int eventX = e->pos().x();
    BookmarksToolbarButton* button = buttonAt(e->pos());
    m_dropPos = e->pos();
    m_dropRow = m_layout->indexOf(button);
    if (button) {
        bool res = eventX - button->x() < button->x() + button->width() -eventX;
        m_dropRow = res ? m_dropRow : m_dropRow + 1;
    } else {
        m_dropRow = -1;
    }

    update();
}

void BookmarksToolbar::dragLeaveEvent(QDragLeaveEvent *e)
{
    Q_UNUSED(e);
    clearDropIndicator();
}

void BookmarksToolbar::clearDropIndicator()
{
    m_dropRow = -1;
    update();
}

void BookmarksToolbar::paintEvent(QPaintEvent *p)
{
    QWidget::paintEvent(p);

    // Draw drop indicator
    if (m_dropRow != -1) {
        BookmarksToolbarButton* button = buttonAt(m_dropPos);
        if (button) {
            if (button->bookmark()->isFolder()) {
                return;
            }
            const QRect tr = QRect(button->x(), 0, button->width(), height());
            QRect r;

            if (m_dropRow == m_layout->indexOf(button)) {
                r = QRect(qMax(0, tr.left() - 2), tr.top(), 3, tr.height());
            } else {
                r = QRect(tr.right() + 0, tr.top(), 3, tr.height());
            }

            QzTools::paintDropIndicator(this, r);
        }
    }
}
