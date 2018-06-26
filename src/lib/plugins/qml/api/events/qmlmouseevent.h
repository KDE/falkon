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

#include <QMouseEvent>

class QmlMouseEvent : public QObject
{
    Q_OBJECT
public:
    explicit QmlMouseEvent(QMouseEvent *mouseEvent = nullptr, QObject *parent = nullptr);
    Q_INVOKABLE int button() const;
    Q_INVOKABLE int buttons() const;
    Q_INVOKABLE QPoint globalPos() const;
    Q_INVOKABLE int globalX() const;
    Q_INVOKABLE int globalY() const;
    Q_INVOKABLE QPointF localPos() const;
    Q_INVOKABLE QPoint pos() const;
    Q_INVOKABLE QPointF screenPos() const;
    Q_INVOKABLE int source() const;
    Q_INVOKABLE QPointF windowPos() const;
    Q_INVOKABLE int x() const;
    Q_INVOKABLE int y() const;

private:
    QMouseEvent *m_mouseEvent;
};
