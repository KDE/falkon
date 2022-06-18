/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2014  David Rosca <nowrep@gmail.com>
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
#ifndef BOOKMARKSMODEL_H
#define BOOKMARKSMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QMimeData>

#include "qzcommon.h"

class QTimer;

class Bookmarks;
class BookmarkItem;

class FALKON_EXPORT BookmarksModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles {
        TypeRole = Qt::UserRole + 1,
        UrlRole = Qt::UserRole + 2,
        UrlStringRole = Qt::UserRole + 3,
        TitleRole = Qt::UserRole + 4,
        IconRole = Qt::UserRole + 5,
        DescriptionRole = Qt::UserRole + 6,
        KeywordRole = Qt::UserRole + 7,
        VisitCountRole = Qt::UserRole + 8,
        ExpandedRole = Qt::UserRole + 9,
        SidebarExpandedRole = Qt::UserRole + 10,
        MaxRole = SidebarExpandedRole
    };

    explicit BookmarksModel(BookmarkItem* root, Bookmarks* bookmarks, QObject* parent = nullptr);

    void addBookmark(BookmarkItem* parent, int row, BookmarkItem* item);
    void removeBookmark(BookmarkItem* item);

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    bool hasChildren(const QModelIndex &parent) const override;

    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(BookmarkItem* item, int column = 0) const;

    BookmarkItem* item(const QModelIndex &index) const;

private Q_SLOTS:
    void bookmarkChanged(BookmarkItem* item);

private:
    BookmarkItem* m_root;
    Bookmarks* m_bookmarks;
};

class FALKON_EXPORT BookmarksFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit BookmarksFilterModel(QAbstractItemModel* parent);

public Q_SLOTS:
    void setFilterFixedString(const QString &pattern);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private Q_SLOTS:
    void startFiltering();

private:
    QString m_pattern;
    QTimer* m_filterTimer;
};

class FALKON_EXPORT BookmarksButtonMimeData : public QMimeData
{
    Q_OBJECT

public:
    explicit BookmarksButtonMimeData();

    BookmarkItem *item() const;
    void setBookmarkItem(BookmarkItem *item);

    bool hasFormat(const QString &format) const override;
    QStringList formats() const override;

    static QString mimeType();

private:
    BookmarkItem* m_item;
};

#endif // BOOKMARKSMODEL_H
