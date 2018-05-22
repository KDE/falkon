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

QmlHistory::QmlHistory(QObject *parent) :
    QObject(parent)
{
    connect(mApp->history(), &History::historyEntryAdded, this, [=](HistoryEntry entry){
        QmlHistoryItem *historyItem = new QmlHistoryItem(&entry);
        emit visited(historyItem);
    });

    connect(mApp->history(), &History::historyEntryDeleted, this, [=](HistoryEntry entry){
        QmlHistoryItem *historyItem = new QmlHistoryItem(&entry);
        emit visitRemoved(historyItem);
    });
}

QList<QObject*> QmlHistory::search(const QVariantMap &map)
{
    QList<QObject*> list;

    if (!map["text"].isValid()) {
        qWarning() << "Error:" << "wrong arguments passed to" << __FUNCTION__;
        return list;
    }
    QString text = map["text"].toString();
    QList<HistoryEntry*> result = mApp->history()->searchHistoryEntry(text);

    foreach(auto entry, result) {
        auto item = new QmlHistoryItem(entry);
        list.append(item);
    }
    return list;
}

int QmlHistory::getVisits(const QVariantMap &map)
{
    if (!map["url"].isValid()) {
        qWarning() << "Error:" << "wrong arguments passed to" << __FUNCTION__;
        return 0;
    }
    QString url = map["url"].toString();
    HistoryEntry *entry = mApp->history()->getHistoryEntry(url);
    return entry->count;
}

void QmlHistory::addUrl(const QVariantMap &map)
{
    if (!map["title"].isValid() || !map["url"].isValid()) {
        qWarning() << "Error:" << "wrong arguments passed to" << __FUNCTION__;
        return;
    }
    QString title = map["title"].toString();
    QString url = map["url"].toString();

    title = title.isEmpty() ? url : title;

    mApp->history()->addHistoryEntry(QUrl(url), title);
}

void QmlHistory::deleteUrl(const QString &url)
{
    mApp->history()->deleteHistoryEntry(url);
}

void QmlHistory::deleteRange(const QVariantMap &map)
{
    if (!map["startTime"].isValid() || !map["endTime"].isValid()) {
        qWarning() << "Error:" << "wrong arguments passed to" << __FUNCTION__;
        return;
    }
    double startTime = map["startTime"].toDouble();
    double endTime = map["endTime"].toDouble();
    mApp->history()->deleteRange(startTime, endTime);
}

void QmlHistory::deleteAll()
{
    mApp->history()->clearHistory();
}
