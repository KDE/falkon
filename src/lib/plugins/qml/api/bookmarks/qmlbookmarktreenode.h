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
#include "bookmarkitem.h"

/**
 * @brief The class exposing the bookmark item to QML
 */
class QmlBookmarkTreeNode : public QObject
{
    Q_OBJECT

    /**
     * @brief type of bookmark tree node.
     */
    Q_PROPERTY(Type type READ type CONSTANT)

    /**
     * @brief title of bookmark tree node.
     */
    Q_PROPERTY(QString title READ title CONSTANT)

    /**
     * @brief url of bookmark tree node.
     */
    Q_PROPERTY(QString url READ url CONSTANT)

    /**
     * @brief description of bookmark tree node.
     */
    Q_PROPERTY(QString description READ description CONSTANT)

    /**
     * @brief keyword of bookmark tree node.
     */
    Q_PROPERTY(QString keyword READ keyword CONSTANT)

    /**
     * @brief visit count of bookmark tree node.
     */
    Q_PROPERTY(int visitCount READ visitCount CONSTANT)

    /**
     * @brief parent of bookmark tree node.
     */
    Q_PROPERTY(QmlBookmarkTreeNode* parent READ parent CONSTANT)

    /**
     * @brief checks if bookmark tree node is unmodifiable.
     */
    Q_PROPERTY(bool unmodifiable READ unmodifiable CONSTANT)

    /**
     * @brief gets children of bookmark tree node.
     */
    Q_PROPERTY(QList<QObject*> children READ children CONSTANT)

public:
    /**
     * @brief The Type enum
     *
     * Contains the information of the type of the bookmark item,
     */
    enum Type {
        Root = BookmarkItem::Root,           //!< Represents the root bookmark item
        Url = BookmarkItem::Url,             //!< Represents the single bookmark item of type url
        Folder = BookmarkItem::Folder,       //!< Represents the bookmark folder
        Separator = BookmarkItem::Separator, //!< Represents the bookmark separator
        Invalid = BookmarkItem::Invalid      //!< Represents invalid bookmark item
    };
    Q_ENUM(Type)

    explicit QmlBookmarkTreeNode(BookmarkItem *item = nullptr);

    BookmarkItem *item();
    Type type() const;
    QString title() const;
    QString url() const;
    QString description() const;
    QString keyword() const;

private:
    BookmarkItem *m_item = nullptr;

    int visitCount() const;
    QmlBookmarkTreeNode *parent() const;
    bool unmodifiable() const;
    QList<QObject*> children() const;
};
