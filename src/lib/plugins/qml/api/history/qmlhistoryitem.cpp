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
#include "qmlhistoryitem.h"
#include <QQmlEngine>

QmlHistoryItem::QmlHistoryItem(HistoryEntry *entry, QObject *parent)
    : QObject(parent)
    , m_entry(entry)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

/**
 * @brief Get the id of the HistoryEntry
 * @return Integer representing the id of the HistoryEntry if it exists, else 0
 */
int QmlHistoryItem::id() const
{
    if (!m_entry) {
        return 0;
    }
    return m_entry->id;
}

/**
 * @brief Get the url of the HistoryEntry
 * @return String representing the url of the HistoryEntry
 */
QString QmlHistoryItem::url() const
{
    if (!m_entry) {
        return QString();
    }
    return QString::fromUtf8(m_entry->url.toEncoded());
}

/**
 * @brief Get the title of the HistoryEntry
 * @return String representing the title of the HistoryEntry
 */
QString QmlHistoryItem::title() const
{
    if (!m_entry) {
        return QString();
    }
    return m_entry->title;
}

/**
 * @brief Get the visit count of the HistoryEntry
 * @return Integer representing the visit count of the HistoryEntry
 */
int QmlHistoryItem::visitCount() const
{
    if (!m_entry) {
        return 0;
    }
    return m_entry->count;
}

/**
 * @brief Get the last visit time of the HistoryEntry
 * @return Last visit time of the HistoryEntry
 */
QDateTime QmlHistoryItem::lastVisitTime() const
{
    if (!m_entry) {
        return QDateTime().currentDateTime().addMonths(1);
    }
    return m_entry->date;
}

QmlHistoryItemData::QmlHistoryItemData()
{
}

QmlHistoryItemData::~QmlHistoryItemData()
{
    qDeleteAll(m_items);
}

QmlHistoryItem *QmlHistoryItemData::get(HistoryEntry *entry)
{
    QmlHistoryItem *item = m_items.value(entry);
    if (!item) {
        item = new QmlHistoryItem(entry);
        m_items.insert(entry, item);
    }
    return item;
}
