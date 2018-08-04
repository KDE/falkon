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
#include "qmlhistory.h"
#include "mainapplication.h"
#include "history.h"

Q_GLOBAL_STATIC(QmlHistoryItemData, historyItemData)

QmlHistory::QmlHistory(QObject *parent)
    : QObject(parent)
{
    connect(mApp->history(), &History::historyEntryAdded, this, [this](HistoryEntry entry){
        QmlHistoryItem *historyItem = historyItemData->get(&entry);
        emit visited(historyItem);
    });

    connect(mApp->history(), &History::historyEntryDeleted, this, [this](HistoryEntry entry){
        QmlHistoryItem *historyItem = historyItemData->get(&entry);
        emit visitRemoved(historyItem);
    });
}

/**
 * @brief Searches History Entries against a search query
 * @param String representing the search query
 * @return List of History Entries, each of type [QmlHistoryItem](@ref QmlHistoryItem),
 *         matching the search query
 */
QList<QObject*> QmlHistory::search(const QString &text)
{
    QList<QObject*> list;
    QList<HistoryEntry*> result = mApp->history()->searchHistoryEntry(text);

    for (const auto entry : qAsConst(result)) {
        auto item = historyItemData->get(entry);
        list.append(item);
    }
    return list;
}

/**
 * @brief Get the visit count of a url
 * @param String representing the url
 * @return Integer representing the visit count of the given url
 */
int QmlHistory::getVisits(const QString &url)
{
    HistoryEntry *entry = mApp->history()->getHistoryEntry(url);
    return entry->count;
}

/**
 * @brief Add url to the history
 * @param A JavaScript object containing
 *        - title:
 *          String representing the title of the hisotry entry
 *        - url:
 *          String representing the url of the history entry
 */
void QmlHistory::addUrl(const QVariantMap &map)
{
    if (!map.contains(QSL("title")) || !map.contains(QSL("url"))) {
        qWarning() << "Error:" << "wrong arguments passed to" << __FUNCTION__;
        return;
    }
    QString title = map.value(QSL("title")).toString();
    const QString url = map.value(QSL("url")).toString();

    title = title.isEmpty() ? url : title;

    mApp->history()->addHistoryEntry(QUrl::fromEncoded(url.toUtf8()), title);
}

/**
 * @brief Deletes a url from the history
 * @param String representing the url of the history entry
 */
void QmlHistory::deleteUrl(const QString &url)
{
    mApp->history()->deleteHistoryEntry(url);
}

/**
 * @brief Deletes history entries within the given range
 * @param A JavaScript object containing
 *        - startTime:
 *          A JavaScript Date object representing the start time
 *        - endTime:
 *          A JavaScript Date object representing the end time
 */
void QmlHistory::deleteRange(const QVariantMap &map)
{
    if (!map.contains(QSL("startTime")) || !map.contains(QSL("endTime"))) {
        qWarning() << "Error:" << "wrong arguments passed to" << __FUNCTION__;
        return;
    }
    const double startTime = map.value(QSL("startTime")).toDouble();
    const double endTime = map.value(QSL("endTime")).toDouble();
    mApp->history()->deleteRange(startTime, endTime);
}

/**
 * @brief Clears all the history
 */
void QmlHistory::deleteAll()
{
    mApp->history()->clearHistory();
}
