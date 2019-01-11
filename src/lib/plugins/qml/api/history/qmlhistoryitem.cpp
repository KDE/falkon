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

QmlHistoryItem::QmlHistoryItem(const HistoryEntry &entry, QObject *parent)
    : QObject(parent)
    , m_entry(entry)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

int QmlHistoryItem::id() const
{
    return m_entry.id;
}

QString QmlHistoryItem::url() const
{
    return QString::fromUtf8(m_entry.url.toEncoded());
}

QString QmlHistoryItem::title() const
{
    return m_entry.title;
}

int QmlHistoryItem::visitCount() const
{
    return m_entry.count;
}

QDateTime QmlHistoryItem::lastVisitTime() const
{
    return m_entry.date;
}
