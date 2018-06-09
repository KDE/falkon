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

#include <QDebug>

Q_GLOBAL_STATIC(QmlBookmarkTreeNodeData, bookmarkTreeNodeData)

QmlBookmarks::QmlBookmarks(QObject *parent)
    : QObject(parent)
{
    connect(mApp->bookmarks(), &Bookmarks::bookmarkAdded, this, [this](BookmarkItem *item){
        auto treeNode = bookmarkTreeNodeData->get(item);
        emit created(treeNode);
    });

    connect(mApp->bookmarks(), &Bookmarks::bookmarkChanged, this, [this](BookmarkItem *item){
        auto treeNode = bookmarkTreeNodeData->get(item);
        emit changed(treeNode);
    });

    connect(mApp->bookmarks(), &Bookmarks::bookmarkRemoved, this, [this](BookmarkItem *item){
        auto treeNode = bookmarkTreeNodeData->get(item);
        emit removed(treeNode);
    });
}

BookmarkItem *QmlBookmarks::getBookmarkItem(QmlBookmarkTreeNode *treeNode) const
{
    auto bookmarks = mApp->bookmarks();
    QList<BookmarkItem*> items;

    if (treeNode->url().isEmpty()) {
        if (isTreeNodeEqualsItem(treeNode, bookmarks->rootItem())) {
            return bookmarks->rootItem();

        } else if (isTreeNodeEqualsItem(treeNode, bookmarks->toolbarFolder())) {
            return bookmarks->toolbarFolder();

        } else if (isTreeNodeEqualsItem(treeNode, bookmarks->menuFolder())) {
            return bookmarks->menuFolder();

        } else if (isTreeNodeEqualsItem(treeNode, bookmarks->unsortedFolder())) {
            return bookmarks->unsortedFolder();
        }

        items = bookmarks->searchBookmarks(treeNode->title());
    } else {
        items = bookmarks->searchBookmarks(QUrl::fromEncoded(treeNode->url().toUtf8()));
    }

    for (auto item : items) {
        if (isTreeNodeEqualsItem(treeNode, item)) {
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

bool QmlBookmarks::isBookmarked(const QUrl &url) const
{
    return mApp->bookmarks()->isBookmarked(url);
}

QmlBookmarkTreeNode *QmlBookmarks::rootItem() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->rootItem());
}

QmlBookmarkTreeNode *QmlBookmarks::toolbarFolder() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->toolbarFolder());
}

QmlBookmarkTreeNode *QmlBookmarks::menuFolder() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->menuFolder());
}

QmlBookmarkTreeNode *QmlBookmarks::unsortedFolder() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->unsortedFolder());
}


QmlBookmarkTreeNode *QmlBookmarks::lastUsedFolder() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->lastUsedFolder());
}

bool QmlBookmarks::create(const QVariantMap &map) const
{
    if (!map.contains(QSL("parent"))) {
        qWarning() << "Unable to create new bookmark:" << "parent not found";
        return false;
    }
    QString title = map.value(QSL("title")).toString();
    QString urlString = map.value(QSL("url")).toString();
    QString description = map.value(QSL("description")).toString();

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
    mApp->bookmarks()->removeBookmark(item);
    return true;
}

QList<QObject*> QmlBookmarks::search(const QVariantMap &map) const
{
    if (!map.contains(QSL("query")) && !map.contains(QSL("url"))) {
        qWarning() << "Unable to search bookmarks";
        return QList<QObject*>();
    }

    QString query = map.value(QSL("query")).toString();
    QString urlString = map.value(QSL("url")).toString();
    QList<BookmarkItem*> items;
    if (urlString.isEmpty()) {
        items = mApp->bookmarks()->searchBookmarks(query);
    } else {
        items = mApp->bookmarks()->searchBookmarks(QUrl::fromEncoded(urlString.toUtf8()));
    }
    QList<QObject*> ret;
    for (auto item : items) {
        ret.append(bookmarkTreeNodeData->get(item));
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
    auto items = mApp->bookmarks()->searchBookmarks(QUrl(string));
    for (auto item : items) {
        if (item->urlString() == string) {
            return bookmarkTreeNodeData->get(item);
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

    auto items = bookmarkItem->children();
    for (auto item : items) {
        ret.append(bookmarkTreeNodeData->get(item));
    }

    return ret;
}

bool QmlBookmarks::isTreeNodeEqualsItem(QmlBookmarkTreeNode *treeNode, BookmarkItem *item) const
{
    return treeNode->title() == item->title()
            && treeNode->url() == item->urlString()
            && treeNode->description() == item->description()
            && (int)(treeNode->type()) == (int)(item->type());
}
