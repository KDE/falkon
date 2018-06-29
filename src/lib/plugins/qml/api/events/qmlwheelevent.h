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

#include <QWheelEvent>
#include <QObject>

class QmlWheelEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPoint angleDelta READ angleDelta CONSTANT)
    Q_PROPERTY(int buttons READ buttons CONSTANT)
    Q_PROPERTY(QPoint globalPos READ globalPos CONSTANT)
    Q_PROPERTY(QPointF globalPosF READ globalPosF CONSTANT)
    Q_PROPERTY(int globalX READ globalX CONSTANT)
    Q_PROPERTY(int globalY READ globalY CONSTANT)
    Q_PROPERTY(bool inverted READ inverted CONSTANT)
    Q_PROPERTY(int phase READ phase CONSTANT)
    Q_PROPERTY(QPoint pixelDelta READ pixelDelta CONSTANT)
    Q_PROPERTY(QPoint pos READ pos CONSTANT)
    Q_PROPERTY(QPointF posF READ posF CONSTANT)
    Q_PROPERTY(int source READ source CONSTANT)
    Q_PROPERTY(int x READ x CONSTANT)
    Q_PROPERTY(int y READ y CONSTANT)
public:
    explicit QmlWheelEvent(QWheelEvent *wheelEvent = nullptr, QObject *parent = nullptr);
    QPoint angleDelta() const;
    int buttons() const;
    QPoint globalPos() const;
    QPointF globalPosF() const;
    int globalX() const;
    int globalY() const;
    bool inverted() const;
    int phase() const;
    QPoint pixelDelta() const;
    QPoint pos() const;
    QPointF posF() const;
    int source() const;
    int x() const;
    int y() const;
private:
    QWheelEvent *m_wheelEvent;
};
