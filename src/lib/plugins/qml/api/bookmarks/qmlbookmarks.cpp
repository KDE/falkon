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
#include "qmlbookmarks.h"
#include "bookmarks.h"
#include "qml/qmlstaticdata.h"
#include <QDebug>
#include <QQmlEngine>

QmlBookmarks::QmlBookmarks(QObject *parent)
    : QObject(parent)
{
    connect(mApp->bookmarks(), &Bookmarks::bookmarkAdded, this, [this](BookmarkItem *item){
        auto treeNode = QmlStaticData::instance().getBookmarkTreeNode(item);
        Q_EMIT created(treeNode);
    });

    connect(mApp->bookmarks(), &Bookmarks::bookmarkChanged, this, [this](BookmarkItem *item){
        auto treeNode = QmlStaticData::instance().getBookmarkTreeNode(item);
        Q_EMIT changed(treeNode);
    });

    connect(mApp->bookmarks(), &Bookmarks::bookmarkRemoved, this, [this](BookmarkItem *item){
        auto treeNode = QmlStaticData::instance().getBookmarkTreeNode(item);
        Q_EMIT removed(treeNode);
    });
}

BookmarkItem *QmlBookmarks::getBookmarkItem(QmlBookmarkTreeNode *treeNode) const
{
    auto bookmarks = mApp->bookmarks();
    QList<BookmarkItem*> items;

    if (treeNode->url().isEmpty()) {
        if (treeNode->item() == bookmarks->rootItem()) {
            return bookmarks->rootItem();

        } else if (treeNode->item() == bookmarks->toolbarFolder()) {
            return bookmarks->toolbarFolder();

        } else if (treeNode->item() == bookmarks->menuFolder()) {
            return bookmarks->menuFolder();

        } else if (treeNode->item() == bookmarks->unsortedFolder()) {
            return bookmarks->unsortedFolder();
        }

        items = bookmarks->searchBookmarks(treeNode->title());
    } else {
        items = bookmarks->searchBookmarks(QUrl::fromEncoded(treeNode->url().toUtf8()));
    }

    for (BookmarkItem *item : qAsConst(items)) {
        if (treeNode->item() == item) {
            return item;
        }
    }

    return nullptr;
}

BookmarkItem *QmlBookmarks::getBookmarkItem(QObject *object) const
{
    auto treeNode = qobject_cast<QmlBookmarkTreeNode*>(object);
    if (!treeNode) {
        return nullptr;
    }

    auto item = getBookmarkItem(treeNode);
    if (!item || item->urlString() != treeNode->url()) {
        return nullptr;
    }

    return item;
}

bool QmlBookmarks::isBookmarked(const QString &url) const
{
    return mApp->bookmarks()->isBookmarked(QUrl::fromEncoded(url.toUtf8()));
}

QmlBookmarkTreeNode *QmlBookmarks::rootItem() const
{
    return QmlStaticData::instance().getBookmarkTreeNode(mApp->bookmarks()->rootItem());
}

QmlBookmarkTreeNode *QmlBookmarks::toolbarFolder() const
{
    return QmlStaticData::instance().getBookmarkTreeNode(mApp->bookmarks()->toolbarFolder());
}

QmlBookmarkTreeNode *QmlBookmarks::menuFolder() const
{
    return QmlStaticData::instance().getBookmarkTreeNode(mApp->bookmarks()->menuFolder());
}

QmlBookmarkTreeNode *QmlBookmarks::unsortedFolder() const
{
    return QmlStaticData::instance().getBookmarkTreeNode(mApp->bookmarks()->unsortedFolder());
}

QmlBookmarkTreeNode *QmlBookmarks::lastUsedFolder() const
{
    return QmlStaticData::instance().getBookmarkTreeNode(mApp->bookmarks()->lastUsedFolder());
}

bool QmlBookmarks::create(const QVariantMap &map) const
{
    if (!map.contains(QSL("parent"))) {
        qWarning() << "Unable to create new bookmark:" << "parent not found";
        return false;
    }
    const QString title = map.value(QSL("title")).toString();
    const QString urlString = map.value(QSL("url")).toString();
    const QString description = map.value(QSL("description")).toString();

    BookmarkItem::Type type;
    if (map.contains(QSL("type"))) {
        type = BookmarkItem::Type(map.value(QSL("type")).toInt());
    } else if (urlString.isEmpty()){
        if (!title.isEmpty()) {
            type = BookmarkItem::Folder;
        } else {
            type = BookmarkItem::Invalid;
        }
    } else {
        type = BookmarkItem::Url;
    }

    auto object = map.value(QSL("parent")).value<QObject*>();
    auto parent = getBookmarkItem(object);
    if (!parent) {
        qWarning() << "Unable to create new bookmark:" << "parent not found";
        return false;
    }
    auto item = new BookmarkItem(type);
    item->setTitle(title);
    item->setUrl(QUrl::fromEncoded(urlString.toUtf8()));
    item->setDescription(description);
    mApp->bookmarks()->addBookmark(parent, item);
    return true;
}

bool QmlBookmarks::remove(QmlBookmarkTreeNode *treeNode) const
{
    auto item = getBookmarkItem(treeNode);
    if (!item) {
        qWarning() << "Unable to remove bookmark:" <<"BookmarkItem not found";
        return false;
    }
    return mApp->bookmarks()->removeBookmark(item);
}

QList<QObject*> QmlBookmarks::search(const QVariantMap &map) const
{
    if (!map.contains(QSL("query")) && !map.contains(QSL("url"))) {
        qWarning() << "Unable to search bookmarks";
        return {};
    }

    const QString query = map.value(QSL("query")).toString();
    const QString urlString = map.value(QSL("url")).toString();
    QList<BookmarkItem*> items;
    if (urlString.isEmpty()) {
        items = mApp->bookmarks()->searchBookmarks(query);
    } else {
        items = mApp->bookmarks()->searchBookmarks(QUrl::fromEncoded(urlString.toUtf8()));
    }
    QList<QObject*> ret;
    ret.reserve(items.size());
    for (auto item : qAsConst(items)) {
        ret.append(QmlStaticData::instance().getBookmarkTreeNode(item));
    }
    return ret;
}

bool QmlBookmarks::update(QObject *object, const QVariantMap &changes) const
{
    auto treeNode = qobject_cast<QmlBookmarkTreeNode*>(object);
    if (!treeNode) {
        qWarning() << "Unable to update bookmark:" << "unable to cast QVariant to QmlBookmarkTreeNode";
        return false;
    }

    auto item = getBookmarkItem(treeNode);
    if (!item) {
        qWarning() << "Unable to update bookmark:" << "bookmark not found";
        return false;
    }

    if (!mApp->bookmarks()->canBeModified(item)) {
        qWarning() << "Unable to update bookmark:" << "bookmark can not be modified";
    }

    QString newTitle = treeNode->title();
    if (changes.contains(QSL("title"))) {
        newTitle = changes.value(QSL("title")).toString();
    }
    QString newDescription = treeNode->description();
    if (changes.contains(QSL("description"))) {
        newDescription = changes.value(QSL("description")).toString();
    }
    QString newKeyword = treeNode->keyword();
    if (changes.contains(QSL("keyword"))) {
        newKeyword = changes.value(QSL("keyword")).toString();
    }

    item->setTitle(newTitle);
    item->setDescription(newDescription);
    item->setKeyword(newKeyword);
    mApp->bookmarks()->changeBookmark(item);
    return true;
}

QmlBookmarkTreeNode *QmlBookmarks::get(const QString &string) const
{
    const QList<BookmarkItem*> items = mApp->bookmarks()->searchBookmarks(QUrl(string));
    for (BookmarkItem *item : items) {
        if (item->urlString() == string) {
            return QmlStaticData::instance().getBookmarkTreeNode(item);
        }
    }

    return nullptr;
}

QList<QObject*> QmlBookmarks::getChildren(QObject *object) const
{
    QList<QObject*> ret;

    auto bookmarkItem = getBookmarkItem(object);
    if (!bookmarkItem) {
        qWarning() << "Unable to get children:" << "parent not found";
        return ret;
    }

    const QList<BookmarkItem*> items = bookmarkItem->children();
    for (BookmarkItem *item : items) {
        ret.append(QmlStaticData::instance().getBookmarkTreeNode(item));
    }

    return ret;
}
