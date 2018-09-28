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
#include "qmlhistoryitem.h"

/**
 * @brief The class exposing the History API to QML
 */
class QmlHistory : public QObject
{
    Q_OBJECT
public:
    explicit QmlHistory(QObject *parent = nullptr);
    /**
     * @brief Searches History Entries against a search query
     * @param String representing the search query
     * @return List of History Entries, each of type [QmlHistoryItem](@ref QmlHistoryItem),
     *         matching the search query
     */
    Q_INVOKABLE QList<QObject*> search(const QString &text);
    /**
     * @brief Get the visit count of a url
     * @param String representing the url
     * @return Integer representing the visit count of the given url
     */
    Q_INVOKABLE int getVisits(const QString &url);
    /**
     * @brief Add url to the history
     * @param A JavaScript object containing
     *        - title:
     *          String representing the title of the hisotry entry
     *        - url:
     *          String representing the url of the history entry
     */
    Q_INVOKABLE void addUrl(const QVariantMap &map);
    /**
     * @brief Deletes a url from the history
     * @param String representing the url of the history entry
     */
    Q_INVOKABLE void deleteUrl(const QString &url);
    /**
     * @brief Deletes history entries within the given range
     * @param A JavaScript object containing
     *        - startTime:
     *          A JavaScript Date object representing the start time
     *        - endTime:
     *          A JavaScript Date object representing the end time
     */
    Q_INVOKABLE void deleteRange(const QVariantMap &map);
    /**
     * @brief Clears all the history
     */
    Q_INVOKABLE void deleteAll();
Q_SIGNALS:
    /**
     * @brief The signal emitted when a HistoryEntry is added
     * @param Object of type [QmlHistoryItem](@ref QmlHistoryItem), representing
     *        the added History Entry
     */
    void visited(QmlHistoryItem *historyItem);

    /**
     * @brief The signal emitted when a HistoryEntry is removed
     * @param Object of type [QmlHistoryItem](@ref QmlHistoryItem), representing
     *        the removed History Entry
     */
    void visitRemoved(QmlHistoryItem *historyItem);
};
