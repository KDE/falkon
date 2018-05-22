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

QmlHistoryItem::QmlHistoryItem(HistoryEntry *entry, QObject *parent) :
    QObject(parent)
  , m_entry(entry)
{
}

int QmlHistoryItem::id() const
{
    if (!m_entry) {
        return 0;
    }
    return m_entry->id;
}

QString QmlHistoryItem::url() const
{
    if (!m_entry) {
        return QString();
    }
    return m_entry->url.toString();
}

QString QmlHistoryItem::title() const
{
    if (!m_entry) {
        return QString();
    }
    return m_entry->title;
}

int QmlHistoryItem::visitCount() const
{
    if (!m_entry) {
        return 0;
    }
    return m_entry->count;
}

QDateTime QmlHistoryItem::lastVisitTime() const
{
    if (!m_entry) {
        return QDateTime().currentDateTime().addMonths(1);
    }
    return m_entry->date;
}
