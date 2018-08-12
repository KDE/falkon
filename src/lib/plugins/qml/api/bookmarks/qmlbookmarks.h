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
#pragma once

#include "qmlbookmarktreenode.h"
#include "mainapplication.h"

#include <QObject>

/**
 * @brief The class exposing the Bookmarks API to QML
 */
class QmlBookmarks : public QObject
{
    Q_OBJECT

public:
    explicit QmlBookmarks(QObject *parent = nullptr);

    /**
     * @brief Checks if the url is bookmarked
     * @param String representing the url to check
     * @return true if bookmarked, else false
     */
    Q_INVOKABLE bool isBookmarked(const QString &url) const;
    /**
     * @brief Get the root bookmark item
     * @return Root boomkark item
     */
    Q_INVOKABLE QmlBookmarkTreeNode *rootItem() const;
    /**
     * @brief Get the bookmarks toolbar
     * @return Bookmarks toolbar
     */
    Q_INVOKABLE QmlBookmarkTreeNode *toolbarFolder() const;
    /**
     * @brief Get the bookmarks menu folder
     * @return Bookmarks menu folder
     */
    Q_INVOKABLE QmlBookmarkTreeNode *menuFolder() const;
    /**
     * @brief Get the unsorted bookmarks folder
     * @return Unsorted bookmarks folder
     */
    Q_INVOKABLE QmlBookmarkTreeNode *unsortedFolder() const;
    /**
     * @brief Get the last used bookmarks folder
     * @return Last used bookmarks folder
     */
    Q_INVOKABLE QmlBookmarkTreeNode *lastUsedFolder() const;
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
    Q_INVOKABLE bool create(const QVariantMap &map) const;
    /**
     * @brief Removes a bookmark item
     * @param treeNode:
     *        Object of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode) to be removed
     * @return true if the bookmark is removed, else false
     */
    Q_INVOKABLE bool remove(QmlBookmarkTreeNode *treeNode) const;
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
    Q_INVOKABLE QList<QObject*> search(const QVariantMap &map) const;
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
    Q_INVOKABLE bool update(QObject *object, const QVariantMap &changes) const;
    /**
     * @brief Get the first matched bookmark item
     * @param String representing the query
     * @return Object of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode) if
     *         the query is matched with a bookmark, else null
     */
    Q_INVOKABLE QmlBookmarkTreeNode *get(const QString &string) const;
    /**
     * @brief Get children of the bookmark item
     * @param Object of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode), representing
     *        the parent whose children are requested.
     * @return List containing the children, of type [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode)
     */
    Q_INVOKABLE QList<QObject*> getChildren(QObject *object) const;

Q_SIGNALS:
    /**
     * @brief This signal is emitted when a new bookmark item is created
     * @param bookmark item, exposed to QML as [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode)
     */
    void created(QmlBookmarkTreeNode *treeNode);

    /**
     * @brief This signal is emitted when a bookmark item is edited
     * @param bookmark item, exposed to QML as [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode)
     */
    void changed(QmlBookmarkTreeNode *treeNode);

    /**
     * @brief This signal is emitted when a bookmark item is removed
     * @param bookmark item, exposed to QML as [QmlBookmarkTreeNode](@ref QmlBookmarkTreeNode)
     */
    void removed(QmlBookmarkTreeNode *treeNode);

private:
    BookmarkItem *getBookmarkItem(QmlBookmarkTreeNode *treeNode) const;
    BookmarkItem *getBookmarkItem(QObject *object) const;
};
