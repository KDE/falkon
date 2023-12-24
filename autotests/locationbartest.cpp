/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 David Rosca <nowrep@gmail.com>
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
#include "locationbartest.h"
#include "autotests.h"
#include "locationbar.h"
#include "searchenginesmanager.h"
#include "bookmarks.h"
#include "bookmarkitem.h"
#include "qzsettings.h"

static void removeBookmarks(BookmarkItem *parent)
{
    for (BookmarkItem *child : parent->children()) {
        mApp->bookmarks()->removeBookmark(child);
        removeBookmarks(child);
    }
}

void LocationBarTest::initTestCase()
{
}

void LocationBarTest::cleanupTestCase()
{
}

void LocationBarTest::init()
{
    mApp->searchEnginesManager()->setAllEngines({});
    removeBookmarks(mApp->bookmarks()->rootItem());
}

void LocationBarTest::loadActionBasicTest()
{
    LocationBar::LoadAction action;

    action = LocationBar::loadAction(QSL("http://kde.org"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://kde.org")));

    action = LocationBar::loadAction(QSL("kde.org"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://kde.org")));

    action = LocationBar::loadAction(QSL("localhost"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://localhost")));

    action = LocationBar::loadAction(QSL("localhost/test/path?x=2"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://localhost/test/path?x=2")));

    action = LocationBar::loadAction(QSL("host.com/test/path?x=2"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://host.com/test/path?x=2")));

    action = LocationBar::loadAction(QSL("not-url"));
    QCOMPARE(action.type, LocationBar::LoadAction::Search);

    action = LocationBar::loadAction(QSL("not url with spaces"));
    QCOMPARE(action.type, LocationBar::LoadAction::Search);

    action = LocationBar::loadAction(QSL("falkon:about"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("falkon:about")));
}

void LocationBarTest::loadActionBookmarksTest()
{
    auto* bookmark = new BookmarkItem(BookmarkItem::Url);
    bookmark->setTitle(QSL("KDE Bookmark title"));
    bookmark->setUrl(QUrl(QSL("http://kde.org")));
    bookmark->setKeyword(QSL("kde-bookmark"));
    mApp->bookmarks()->addBookmark(mApp->bookmarks()->unsortedFolder(), bookmark);

    LocationBar::LoadAction action;

    action = LocationBar::loadAction(QSL("http://kde.org"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://kde.org")));

    action = LocationBar::loadAction(QSL("kde-bookmark-notkeyword"));
    QCOMPARE(action.type, LocationBar::LoadAction::Search);

    action = LocationBar::loadAction(QSL("kde-bookmark"));
    QCOMPARE(action.type, LocationBar::LoadAction::Bookmark);
    QCOMPARE(action.bookmark, bookmark);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://kde.org")));
}

void LocationBarTest::loadActionSearchTest()
{
    SearchEngine engine;
    engine.name = QSL("Test Engine");
    engine.url = QSL("http://test/%s");
    engine.shortcut = QSL("t");
    mApp->searchEnginesManager()->addEngine(engine);
    mApp->searchEnginesManager()->setDefaultEngine(engine);

    LocationBar::LoadAction action;

    action = LocationBar::loadAction(QSL("search term"));
    QCOMPARE(action.type, LocationBar::LoadAction::Search);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://test/search%20term")));

    action = LocationBar::loadAction(QSL("t search term"));
    QCOMPARE(action.type, LocationBar::LoadAction::Search);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://test/search%20term")));

    action = LocationBar::loadAction(QSL(" ttt-notsearch"));
    QCOMPARE(action.type, LocationBar::LoadAction::Search);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://test/ttt-notsearch")));
}

void LocationBarTest::loadAction_kdebug389491()
{
    // "site:website.com searchterm" and "link:website.com" are loaded instead of searched

    SearchEngine engine;
    engine.name = QSL("Test Engine");
    engine.url = QSL("http://test/%s");
    engine.shortcut = QSL("t");
    mApp->searchEnginesManager()->addEngine(engine);
    mApp->searchEnginesManager()->setActiveEngine(engine);

    LocationBar::LoadAction action;

    action = LocationBar::loadAction(QSL("site:website.com searchterm"));
    QCOMPARE(action.type, LocationBar::LoadAction::Search);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://test/site%3Awebsite.com%20searchterm")));

    action = LocationBar::loadAction(QSL("link:website.com"));
    QCOMPARE(action.type, LocationBar::LoadAction::Search);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://test/link%3Awebsite.com")));

    action = LocationBar::loadAction(QSL("http://website.com?search=searchterm and another"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://website.com?search=searchterm and another")));
}

void LocationBarTest::loadActionSpecialSchemesTest()
{
    LocationBar::LoadAction action;

    action = LocationBar::loadAction(QSL("data:image/png;base64,xxxxx"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("data:image/png;base64,xxxxx")));

    action = LocationBar::loadAction(QSL("falkon:about"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("falkon:about")));

    action = LocationBar::loadAction(QSL("file:test.html"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("file:test.html")));

    action = LocationBar::loadAction(QSL("about:blank"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("about:blank")));

    action = LocationBar::loadAction(QSL("javascript:test"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("javascript:test")));

    action = LocationBar::loadAction(QSL("javascript:alert(' test ');"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("javascript:alert('%20test%20');")));
}

void LocationBarTest::loadAction_issue2578()
{
    // typed text is not correctly transformed to QUrl when searchFromAddressBar is disabled

    qzSettings->searchFromAddressBar = false;

    LocationBar::LoadAction action;

    action = LocationBar::loadAction(QSL("github.com"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://github.com")));

    action = LocationBar::loadAction(QSL("github"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://github")));

    action = LocationBar::loadAction(QSL("github/test/path"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://github/test/path")));

    action = LocationBar::loadAction(QSL("localhost"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://localhost")));

    action = LocationBar::loadAction(QSL("localhost/test/path"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://localhost/test/path")));

    action = LocationBar::loadAction(QSL("github.com foo bar"));
    QCOMPARE(action.type, LocationBar::LoadAction::Invalid);
}

void LocationBarTest::loadAction_kdebug392445()
{
    // %20 in url will make it incorrectly treat as web search

    qzSettings->searchFromAddressBar = true;

    LocationBar::LoadAction action;

    action = LocationBar::loadAction(QSL("http://www.example.com/my%20beautiful%20page"));
    QCOMPARE(action.type, LocationBar::LoadAction::Url);
    QCOMPARE(action.loadRequest.url(), QUrl(QSL("http://www.example.com/my%20beautiful%20page")));
}

FALKONTEST_MAIN(LocationBarTest)
