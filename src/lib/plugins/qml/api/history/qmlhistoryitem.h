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

#include <QObject>
#include "history.h"

/**
 * @brief The class exposing HistoryEntry to QML
 */
class QmlHistoryItem : public QObject
{
    Q_OBJECT

    /**
     * @brief id of the history item
     */
    Q_PROPERTY(int id READ id CONSTANT)

    /**
     * @brief url of the history item
     */
    Q_PROPERTY(QString url READ url CONSTANT)

    /**
     * @brief title of the history item
     */
    Q_PROPERTY(QString title READ title CONSTANT)

    /**
     * @brief visit count of the history item
     */
    Q_PROPERTY(int visitCount READ visitCount CONSTANT)

    /**
     * @brief last visit time of the history item
     */
    Q_PROPERTY(QDateTime lastVisitTime READ lastVisitTime CONSTANT)
public:
    explicit QmlHistoryItem(const HistoryEntry &entry, QObject *parent = nullptr);

private:
    HistoryEntry m_entry;

    int id() const;
    QString url() const;
    QString title() const;
    int visitCount() const;
    QDateTime lastVisitTime() const;
};
