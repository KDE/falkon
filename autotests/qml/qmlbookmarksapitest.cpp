/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmlbookmarksapitest.h"
#include "autotests.h"
#include "qmltesthelper.h"
#include "mainapplication.h"
#include "bookmarks.h"
#include "bookmarkitem.h"
#include "qml/api/qmlbookmarktreenode.h"

void QmlBookmarksApiTest::initTestCase()
{
}

void QmlBookmarksApiTest::cleanupTestCase()
{
}

void QmlBookmarksApiTest::testBookmarkTreeNodeType()
{
    QmlTestHelper qmlTest;

    auto type = BookmarkItem::Type(qmlTest.evaluate("Falkon.Bookmarks.rootItem().type").toInt());
    QCOMPARE(mApp->bookmarks()->rootItem()->type(), type);

    type = BookmarkItem::Type(qmlTest.evaluate("Falkon.Bookmarks.toolbarFolder().type").toInt());
    QCOMPARE(mApp->bookmarks()->toolbarFolder()->type(), type);
}

void QmlBookmarksApiTest::testBookmarkTreeNode()
{
    QmlTestHelper qmlTest;

    QObject *bookmark = qmlTest.evaluateQObject("Falkon.Bookmarks.toolbarFolder()");
    QVERIFY(bookmark);
    auto toolbarFolder = mApp->bookmarks()->toolbarFolder();

    QCOMPARE(toolbarFolder->title(), bookmark->property("title").toString());
    QCOMPARE(toolbarFolder->urlString(), bookmark->property("url").toString());
    QCOMPARE(toolbarFolder->description(), bookmark->property("description").toString());
    QCOMPARE(!mApp->bookmarks()->canBeModified(toolbarFolder), bookmark->property("unmodifiable").toBool());
    QObject* parent = qvariant_cast<QObject*>(bookmark->property("parent"));
    QVERIFY(parent);
    QCOMPARE(mApp->bookmarks()->rootItem()->title(), parent->property("title").toString());
}

void QmlBookmarksApiTest::testBookmarksCreation()
{
    QmlTestHelper qmlTest;

    auto item = new BookmarkItem(BookmarkItem::Url);
    item->setTitle("Example Domain");
    item->setUrl(QUrl("https://example.com/"));
    item->setDescription("Testing bookmark description");

    QObject *qmlBookmarks = qmlTest.evaluateQObject("Falkon.Bookmarks");
    QVERIFY(qmlBookmarks);

    QSignalSpy qmlBookmarksSpy(qmlBookmarks, SIGNAL(created(QmlBookmarkTreeNode*)));
    mApp->bookmarks()->addBookmark(mApp->bookmarks()->rootItem(), item);

    QCOMPARE(qmlBookmarksSpy.count(), 1);

    QObject *created = qvariant_cast<QObject*>(qmlBookmarksSpy.at(0).at(0));
    QVERIFY(created);
    QCOMPARE(item->title(), created->property("title").toString());

    qRegisterMetaType<BookmarkItem*>();
    QSignalSpy bookmarksSpy(mApp->bookmarks(), &Bookmarks::bookmarkAdded);

    auto out = qmlTest.evaluate("Falkon.Bookmarks.create({"
                                "    parent: Falkon.Bookmarks.toolbarFolder(),"
                                "    title: 'Example Plugin',"
                                "    url: 'https://another-example.com'"
                                "});");
    QVERIFY(out.toBool());

    QCOMPARE(bookmarksSpy.count(), 1);
    BookmarkItem* createdItem = qvariant_cast<BookmarkItem*>(bookmarksSpy.at(0).at(0));
    QVERIFY(createdItem);
    QCOMPARE(createdItem->title(), QString("Example Plugin"));
}

void QmlBookmarksApiTest::testBookmarksExistence()
{
    // in continuation from testBookmarksCreation

    QmlTestHelper qmlTest;
    auto result = qmlTest.evaluate("Falkon.Bookmarks.isBookmarked('https://example.com/')").toBool();
    QVERIFY(result);
    QCOMPARE(mApp->bookmarks()->isBookmarked(QUrl("https://example.com/")), result);
}

void QmlBookmarksApiTest::testBookmarksModification()
{
    // in continuation from testBookmarksExistence

    QmlTestHelper qmlTest;

    QObject *qmlBookmarks = qmlTest.evaluateQObject("Falkon.Bookmarks");
    QVERIFY(qmlBookmarks);

    QSignalSpy qmlBookmarksSpy(qmlBookmarks, SIGNAL(changed(QmlBookmarkTreeNode*)));
    BookmarkItem* item = mApp->bookmarks()->searchBookmarks("https://example.com/").at(0);
    item->setTitle("Modified Example Domain");
    mApp->bookmarks()->changeBookmark(item);

    QCOMPARE(qmlBookmarksSpy.count(), 1);

    QObject *modified = qvariant_cast<QObject*>(qmlBookmarksSpy.at(0).at(0));
    QVERIFY(modified);
    QCOMPARE(modified->property("title").toString(), QString("Modified Example Domain"));

    qRegisterMetaType<BookmarkItem*>();
    QSignalSpy bookmarksSpy(mApp->bookmarks(), &Bookmarks::bookmarkChanged);

    auto out = qmlTest.evaluate("Falkon.Bookmarks.update(Falkon.Bookmarks.get('https://another-example.com'),{"
                                "    title: 'Modified Example Plugin'"
                                "})");
    QVERIFY(out.toBool());

    QCOMPARE(bookmarksSpy.count(), 1);
    BookmarkItem* modifiedItem = qvariant_cast<BookmarkItem*>(bookmarksSpy.at(0).at(0));
    QVERIFY(modifiedItem);
    QCOMPARE(modifiedItem->title(), QString("Modified Example Plugin"));
}

void QmlBookmarksApiTest::testBookmarksRemoval()
{
    // in continuation from testBookmarksModification

    QmlTestHelper qmlTest;

    QObject *qmlBookmarks = qmlTest.evaluateQObject("Falkon.Bookmarks");
    QVERIFY(qmlBookmarks);

    QSignalSpy qmlBookmarksSpy(qmlBookmarks, SIGNAL(removed(QmlBookmarkTreeNode*)));
    BookmarkItem* item = mApp->bookmarks()->searchBookmarks("https://example.com/").at(0);
    mApp->bookmarks()->removeBookmark(item);

    QCOMPARE(qmlBookmarksSpy.count(), 1);

    QObject *removed = qvariant_cast<QObject*>(qmlBookmarksSpy.at(0).at(0));
    QVERIFY(removed);
    QCOMPARE(removed->property("title").toString(), QString("Modified Example Domain"));

    qRegisterMetaType<BookmarkItem*>();
    QSignalSpy bookmarksSpy(mApp->bookmarks(), &Bookmarks::bookmarkRemoved);

    auto out = qmlTest.evaluate("Falkon.Bookmarks.remove(Falkon.Bookmarks.get('https://another-example.com'))");
    QVERIFY(out.toBool());

    QCOMPARE(bookmarksSpy.count(), 1);
    BookmarkItem* removedItem = qvariant_cast<BookmarkItem*>(bookmarksSpy.at(0).at(0));
    QVERIFY(removedItem);
    QCOMPARE(removedItem->title(), QString("Modified Example Plugin"));
}

FALKONTEST_MAIN(QmlBookmarksApiTest)
