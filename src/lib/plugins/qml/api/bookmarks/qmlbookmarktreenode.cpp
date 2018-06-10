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
#include "qmlbookmarktreenode.h"
#include "mainapplication.h"
#include "bookmarks.h"
#include <QQmlEngine>

Q_GLOBAL_STATIC(QmlBookmarkTreeNodeData, bookmarkTreeNodeData)

QmlBookmarkTreeNode::QmlBookmarkTreeNode(BookmarkItem *item)
    : m_item(item)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

/**
 * @brief Get the type of the bookmark item
 * @return Type of the bookmark item
 */
QmlBookmarkTreeNode::Type QmlBookmarkTreeNode::type() const
{
    if (!m_item) {
        return Invalid;
    }

    switch (m_item->type()) {
    case BookmarkItem::Root:
        return Root;

    case BookmarkItem::Url:
        return Url;

    case BookmarkItem::Folder:
        return Folder;

    case BookmarkItem::Separator:
        return Separator;

    case BookmarkItem::Invalid:
    default:
        return Invalid;
    }
}

/**
 * @brief Get the title of the bookmark item
 * @return String representing the title of the bookmark item
 */
QString QmlBookmarkTreeNode::title() const
{
    if (!m_item) {
        return QString();
    }

    return m_item->title();
}

/**
 * @brief Get the url of the bookmark item
 * @return String representing the url of the bookmark item. If type of the
 * bookmark item is not Url, then empty string is returned.
 */
QString QmlBookmarkTreeNode::url() const
{
    if (!m_item) {
        return QString();
    }

    return m_item->urlString();
}

/**
 * @brief Get the description of the bookmark item
 * @return String representing the description of the bookmark item
 */
QString QmlBookmarkTreeNode::description() const
{
    if (!m_item) {
        return QString();
    }

    return m_item->description();
}

/**
 * @brief Get the keyword of the bookmark item
 * @return String representing the keyword of the bookmark item
 */
QString QmlBookmarkTreeNode::keyword() const
{
    if (!m_item) {
        return QString();
    }

    return m_item->keyword();
}

/**
 * @brief Get visit count of the bookmark item.
 * @return Integer representing the visit count of the bookmark item
 */
int QmlBookmarkTreeNode::visitCount() const
{
    if (!m_item) {
        return 0;
    }

    return m_item->visitCount();
}

/**
 * @brief Get the parent of the bookmark item. This will return null
 * if the item is root item.
 * @return Parent of the bookmark item
 */
QmlBookmarkTreeNode *QmlBookmarkTreeNode::parent() const
{
    if (!m_item) {
        return nullptr;
    }

    return bookmarkTreeNodeData->get(m_item->parent());
}

/**
 * @brief Checks if the bookmark item can be modified
 * @return false if it can be modified, else true
 */
bool QmlBookmarkTreeNode::unmodifiable() const
{
    return !mApp->bookmarks()->canBeModified(m_item);
}

/**
 * @brief Get the children of the bookmark item
 * @return List containing children of the bookmark item
 */
QList<QObject*> QmlBookmarkTreeNode::children() const
{
    const auto items = m_item->children();
    QList<QObject*> ret;
    for (const auto &item : items) {
        ret.append(bookmarkTreeNodeData->get(item));
    }
    return ret;
}

QmlBookmarkTreeNodeData::QmlBookmarkTreeNodeData()
{
}

QmlBookmarkTreeNodeData::~QmlBookmarkTreeNodeData()
{
    qDeleteAll(m_nodes);
}

QmlBookmarkTreeNode *QmlBookmarkTreeNodeData::get(BookmarkItem *item)
{
    QmlBookmarkTreeNode *node = m_nodes.value(item);
    if (!node) {
        node = new QmlBookmarkTreeNode(item);
        m_nodes.insert(item, node);
    }
    return node;
}
