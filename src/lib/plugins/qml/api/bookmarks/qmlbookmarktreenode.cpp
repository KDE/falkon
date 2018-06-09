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

QString QmlBookmarkTreeNode::title() const
{
    if (!m_item) {
        return QString();
    }

    return m_item->title();
}

QString QmlBookmarkTreeNode::url() const
{
    if (!m_item) {
        return QString();
    }

    return m_item->urlString();
}

QString QmlBookmarkTreeNode::description() const
{
    if (!m_item) {
        return QString();
    }

    return m_item->description();
}

QString QmlBookmarkTreeNode::keyword() const
{
    if (!m_item) {
        return QString();
    }

    return m_item->keyword();
}

int QmlBookmarkTreeNode::visitCount() const
{
    if (!m_item) {
        return 0;
    }

    return m_item->visitCount();
}

QmlBookmarkTreeNode *QmlBookmarkTreeNode::parent() const
{
    if (!m_item) {
        return nullptr;
    }

    return bookmarkTreeNodeData->get(m_item->parent());
}

bool QmlBookmarkTreeNode::unmodifiable() const
{
    return !mApp->bookmarks()->canBeModified(m_item);
}

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
