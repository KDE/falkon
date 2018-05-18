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
#ifndef QMLBOOKMARKTREENODE_H
#define QMLBOOKMARKTREENODE_H

#include "bookmarkitem.h"

class QmlBookmarkTreeNode : public QObject
{
    Q_OBJECT

public:
    QmlBookmarkTreeNode(BookmarkItem *item = nullptr);
    enum Type {
        Root = BookmarkItem::Root,
        Url = BookmarkItem::Url,
        Folder = BookmarkItem::Folder,
        Separator = BookmarkItem::Separator,
        Invalid = BookmarkItem::Invalid
    };

    Q_ENUMS(Type)

    Q_INVOKABLE Type type() const;
    Q_INVOKABLE QString title() const;
    Q_INVOKABLE QString url() const;
    Q_INVOKABLE QString description() const;
    Q_INVOKABLE QString keyword() const;
    Q_INVOKABLE int visitCount() const;
    Q_INVOKABLE QmlBookmarkTreeNode *parent() const;
    Q_INVOKABLE bool unmodifiable() const;
    Q_INVOKABLE QList<QObject*> children() const;

    static QmlBookmarkTreeNode *fromBookmarkItem(BookmarkItem *item);

private:
    BookmarkItem *m_item;
};

#endif // QMLBOOKMARKTREENODE_H
