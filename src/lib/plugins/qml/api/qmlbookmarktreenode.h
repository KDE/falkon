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

    Q_PROPERTY(Type type READ type CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString keyword READ keyword CONSTANT)
    Q_PROPERTY(int visitCount READ visitCount CONSTANT)
    Q_PROPERTY(QmlBookmarkTreeNode* parent READ parent CONSTANT)
    Q_PROPERTY(bool unmodifiable READ unmodifiable CONSTANT)
    Q_PROPERTY(QList<QObject*> children READ children CONSTANT)

    Type type() const;
    QString title() const;
    QString url() const;
    QString description() const;
    QString keyword() const;
    int visitCount() const;
    QmlBookmarkTreeNode *parent() const;
    bool unmodifiable() const;
    QList<QObject*> children() const;

    static QmlBookmarkTreeNode *fromBookmarkItem(BookmarkItem *item);

private:
    BookmarkItem *m_item;
};

#endif // QMLBOOKMARKTREENODE_H
