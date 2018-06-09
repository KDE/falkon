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

class QmlHistory : public QObject
{
    Q_OBJECT
public:
    explicit QmlHistory(QObject *parent = nullptr);
    Q_INVOKABLE QList<QObject*> search(const QVariantMap &map);
    Q_INVOKABLE int getVisits(const QVariantMap &map);
    Q_INVOKABLE void addUrl(const QVariantMap &map);
    Q_INVOKABLE void deleteUrl(const QVariantMap &map);
    Q_INVOKABLE void deleteRange(const QVariantMap &map);
    Q_INVOKABLE void deleteAll();
Q_SIGNALS:
    void visited(QmlHistoryItem *historyItem);
    void visitRemoved(QmlHistoryItem *historyItem);
};
