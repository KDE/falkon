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

/**
 * @brief Checks if the url is bookmarked
 * @param String representing the url to check
 * @return true if bookmarked, else false
 */
bool QmlBookmarks::isBookmarked(const QString &url) const
{
    return mApp->bookmarks()->isBookmarked(QUrl::fromEncoded(url.toUtf8()));
}

/**
 * @brief Get the root bookmark item
 * @return Root boomkark item
 */
QmlBookmarkTreeNode *QmlBookmarks::rootItem() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->rootItem());
}

/**
 * @brief Get the bookmarks toolbar
 * @return Bookmarks toolbar
 */
QmlBookmarkTreeNode *QmlBookmarks::toolbarFolder() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->toolbarFolder());
}

/**
 * @brief Get the bookmarks menu folder
 * @return Bookmarks menu folder
 */
QmlBookmarkTreeNode *QmlBookmarks::menuFolder() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->menuFolder());
}

/**
 * @brief Get the unsorted bookmarks folder
 * @return Unsorted bookmarks folder
 */
QmlBookmarkTreeNode *QmlBookmarks::unsortedFolder() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->unsortedFolder());
}

/**
 * @brief Get the last used bookmarks folder
 * @return Last used bookmarks folder
 */
QmlBookmarkTreeNode *QmlBookmarks::lastUsedFolder() const
{
    return bookmarkTreeNodeData->get(mApp->bookmarks()->lastUsedFolder());
}

/**
 * @brief Creates a bookmark item
 * @param A JavaScript object containing
 *        - parent:
 *          Object of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode), representing
 *          the parent of the new bookmark item. This is required field.
 *        - title:
 *          String representing the title of the new bookmark item. Defaults to empty string
 *        - url:
 *          String representing the url of the new bookmark item. Defaults to empty string
 *        - description
 *          String representing the description of the new bookmark item. Defaults to empty string
 *        - type:
 *          [Type](@ref QmlBookmarkTreeNode::Type) representing the type of the new bookmark item.
 *          Defaults to [Url](@ref QmlBookmarkTreeNode::Url) if url is provided, else
 *          [Folder](@ref QmlBookmarkTreeNode::Folder) if title is provided, else
 *          [Invalid](@ref QmlBookmarkTreeNode::Invalid)
 * @return true if the bookmark it created, else false
 */
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

/**
 * @brief Removes a bookmark item
 * @param treeNode:
 *        Object of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode) to be removed
 * @return true if the bookmark is removed, else false
 */
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

/**
 * @brief QmlBookmarks::search
 * @param A JavaScript object containing
 *        - query:
 *          String containing search query
 *        - url:
 *          String representing url to be search
 * @return List containing [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode). If both
 *         query and url are not supplied then empty list is returned.
 */
QList<QObject*> QmlBookmarks::search(const QVariantMap &map) const
{
    if (!map.contains(QSL("query")) && !map.contains(QSL("url"))) {
        qWarning() << "Unable to search bookmarks";
        return QList<QObject*>();
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
    for (auto item : items) {
        ret.append(bookmarkTreeNodeData->get(item));
    }
    return ret;
}

/**
 * @brief Updates a bookmark item
 * @param Object of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode), representing the bookmark
 *        to update
 * @param JavaScript object containing the values to be updated
 *        - title:
 *          String representing the new title of the bookmark item
 *        - description:
 *          String representing the new description of the bookmark item
 *        - keyword:
 *          String representing the new keyword of the bookmark item
 * @return true if the bookmark is updated, else false
 */
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

/**
 * @brief Get the first matched bookmark item
 * @param String representing the query
 * @return Object of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode) if
 *         the query is matched with a bookmark, else null
 */
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

/**
 * @brief Get children of the bookmark item
 * @param Object of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode), representing
 *        the parent whose children are requested.
 * @return List containing the children, of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode)
 */
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
