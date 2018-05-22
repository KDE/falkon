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
#ifndef QMLHISTORYITEM_H
#define QMLHISTORYITEM_H

#include <QObject>
#include "history.h"

class QmlHistoryItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(int visitCount READ visitCount CONSTANT)
    Q_PROPERTY(QDateTime lastVisitTime READ lastVisitTime CONSTANT)
public:
    explicit QmlHistoryItem(HistoryEntry *entry = 0, QObject *parent = 0);
    int id() const;
    QString url() const;
    QString title() const;
    int visitCount() const;
    QDateTime lastVisitTime() const;
private:
    HistoryEntry *m_entry;
};

#endif // QMLHISTORYITEM_H
