/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2014  David Rosca <nowrep@gmail.com>
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
#include "bookmarksmenu.h"
#include "bookmarkstools.h"
#include "bookmarkitem.h"
#include "bookmarks.h"
#include "mainapplication.h"
#include "browsinglibrary.h"
#include "browserwindow.h"
#include "qzsettings.h"
#include "tabwidget.h"
#include "iconprovider.h"
#include "statusbar.h"

BookmarksMenu::BookmarksMenu(QWidget* parent)
    : Menu(parent)
    , m_window(nullptr)
    , m_changed(true)
{
    init();

    connect(mApp->bookmarks(), &Bookmarks::bookmarkAdded, this, &BookmarksMenu::bookmarksChanged);
    connect(mApp->bookmarks(), &Bookmarks::bookmarkRemoved, this, &BookmarksMenu::bookmarksChanged);
    connect(mApp->bookmarks(), &Bookmarks::bookmarkChanged, this, &BookmarksMenu::bookmarksChanged);
}

void BookmarksMenu::setMainWindow(BrowserWindow* window)
{
    Q_ASSERT(window);

    m_window = window;
}

void BookmarksMenu::bookmarkPage()
{
    if (m_window) {
        m_window->bookmarkPage();
    }
}

void BookmarksMenu::bookmarkAllTabs()
{
    if (m_window) {
        BookmarksTools::bookmarkAllTabsDialog(m_window, m_window->tabWidget());
    }
}

void BookmarksMenu::showBookmarksManager()
{
    if (m_window) {
        mApp->browsingLibrary()->showBookmarks(m_window);
    }
}

void BookmarksMenu::bookmarksChanged()
{
    m_changed = true;
}

void BookmarksMenu::aboutToShow()
{
    if (m_changed) {
        refresh();
        m_changed = false;
    }
}

void BookmarksMenu::menuAboutToShow()
{
    Q_ASSERT(qobject_cast<Menu*>(sender()));
    Menu *menu = static_cast<Menu*>(sender());

    const auto menuActions = menu->actions();
    for (QAction *action : menuActions) {
        BookmarkItem *item = static_cast<BookmarkItem*>(action->data().value<void*>());
        if (item && item->type() == BookmarkItem::Url && action->icon().isNull()) {
            action->setIcon(item->icon());
        }
    }
}

void BookmarksMenu::menuAboutToHide()
{
    mApp->getWindow()->statusBar()->clearMessage();
}

void BookmarksMenu::menuMiddleClicked(Menu* menu)
{
    BookmarkItem* item = static_cast<BookmarkItem*>(menu->menuAction()->data().value<void*>());
    Q_ASSERT(item);
    openFolder(item);
}

void BookmarksMenu::bookmarkActivated()
{
    if (auto* action = qobject_cast<QAction*>(sender())) {
        BookmarkItem* item = static_cast<BookmarkItem*>(action->data().value<void*>());
        Q_ASSERT(item);
        openBookmark(item);
    }
}

void BookmarksMenu::bookmarkCtrlActivated()
{
    if (auto* action = qobject_cast<QAction*>(sender())) {
        BookmarkItem* item = static_cast<BookmarkItem*>(action->data().value<void*>());
        Q_ASSERT(item);
        openBookmarkInNewTab(item);
    }
}

void BookmarksMenu::bookmarkShiftActivated()
{
    if (auto* action = qobject_cast<QAction*>(sender())) {
        BookmarkItem* item = static_cast<BookmarkItem*>(action->data().value<void*>());
        Q_ASSERT(item);
        openBookmarkInNewWindow(item);
    }
}

void BookmarksMenu::openFolder(BookmarkItem* item)
{
    Q_ASSERT(item->isFolder());

    if (m_window) {
        BookmarksTools::openFolderInTabs(m_window, item);
    }
}

void BookmarksMenu::openBookmark(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    if (m_window) {
        BookmarksTools::openBookmark(m_window, item);
    }
}

void BookmarksMenu::openBookmarkInNewTab(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    if (m_window) {
        BookmarksTools::openBookmarkInNewTab(m_window, item);
    }
}

void BookmarksMenu::openBookmarkInNewWindow(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    BookmarksTools::openBookmarkInNewWindow(item);
}

void BookmarksMenu::init()
{
    setTitle(tr("&Bookmarks"));

    addAction(tr("Bookmark &This Page"), this, &BookmarksMenu::bookmarkPage)->setShortcut(QKeySequence(QSL("Ctrl+D")));
    addAction(tr("Bookmark &All Tabs"), this, &BookmarksMenu::bookmarkAllTabs);
    addAction(QIcon::fromTheme(QSL("bookmarks-organize")), tr("Organize &Bookmarks"), this, &BookmarksMenu::showBookmarksManager)->setShortcut(QKeySequence(QSL("Ctrl+Shift+O")));
    addSeparator();

    connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
    connect(this, SIGNAL(aboutToShow()), this, SLOT(menuAboutToShow()));
    connect(this, SIGNAL(menuMiddleClicked(Menu*)), this, SLOT(menuMiddleClicked(Menu*)));
    connect(this, &QMenu::aboutToHide, this, &BookmarksMenu::menuAboutToHide);
}

void BookmarksMenu::refresh()
{
    while (actions().count() != 4) {
        QAction* act = actions().at(4);
        if (act->menu()) {
            act->menu()->clear();
        }
        removeAction(act);
        delete act;
    }

    BookmarksTools::addActionToMenu(this, this, mApp->bookmarks()->toolbarFolder());
    addSeparator();

    const auto children = mApp->bookmarks()->menuFolder()->children();
    for (BookmarkItem* child : children) {
        BookmarksTools::addActionToMenu(this, this, child);
    }

    addSeparator();
    BookmarksTools::addActionToMenu(this, this, mApp->bookmarks()->unsortedFolder());
}
