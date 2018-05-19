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
#ifndef QMLBOOKMARKS_H
#define QMLBOOKMARKS_H

#include "qmlbookmarktreenode.h"
#include "mainapplication.h"

#include <QObject>

class QmlBookmarks : public QObject
{
    Q_OBJECT

public:
    QmlBookmarks(QObject *parent = 0);

    Q_INVOKABLE bool isBookmarked(const QUrl &url);
    Q_INVOKABLE QmlBookmarkTreeNode *rootItem() const;
    Q_INVOKABLE QmlBookmarkTreeNode *toolbarFolder() const;
    Q_INVOKABLE QmlBookmarkTreeNode *menuFolder() const;
    Q_INVOKABLE QmlBookmarkTreeNode *unsortedFolder() const;
    Q_INVOKABLE QmlBookmarkTreeNode *lastUsedFolder() const;
    Q_INVOKABLE bool create(const QVariantMap &map);
    Q_INVOKABLE bool remove(QObject *object);
    Q_INVOKABLE QList<QObject*> search(const QVariantMap &map);
    Q_INVOKABLE bool update(QObject *object, const QVariantMap &changes);
    Q_INVOKABLE QmlBookmarkTreeNode *get(const QString &string);
    Q_INVOKABLE QList<QObject*> getChildren(QObject *object);
signals:
    void created(QmlBookmarkTreeNode *treeNode);
    void changed(QmlBookmarkTreeNode *treeNode);
    void removed(QmlBookmarkTreeNode *treeNode);

private:
    bool isTreeNodeEqualsItem(QmlBookmarkTreeNode *treeNode, BookmarkItem *item);
    BookmarkItem *getBookmarkItem(QmlBookmarkTreeNode *treeNode);
    BookmarkItem *getBookmarkItem(QObject *object);
};

#endif // QMLBOOKMARKS_H
