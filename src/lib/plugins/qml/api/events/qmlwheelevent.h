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

/**
 * @brief The class exposing WheelEvent to QML
 */
class QmlWheelEvent : public QObject
{
    Q_OBJECT
    /**
     * @brief the distance that the wheel is rotated, in eighths of a degree
     */
    Q_PROPERTY(QPoint angleDelta READ angleDelta CONSTANT)
    /**
     * @brief mouse state at the time of event
     */
    Q_PROPERTY(int buttons READ buttons CONSTANT)
    /**
     * @brief global position of mouse cursor at the time of event
     */
    Q_PROPERTY(QPoint globalPos READ globalPos CONSTANT)
    /**
     * @brief global position of mouse cursor at the time of event
     */
    Q_PROPERTY(QPointF globalPosF READ globalPosF CONSTANT)
    /**
     * @brief global x position of mouse cursor at the time of event
     */
    Q_PROPERTY(int globalX READ globalX CONSTANT)
    /**
     * @brief global y position of mouse cursor at the time of event
     */
    Q_PROPERTY(int globalY READ globalY CONSTANT)
    /**
     * @brief checks if the delta values delivered with the event are inverted
     */
    Q_PROPERTY(bool inverted READ inverted CONSTANT)
    /**
     * @brief scrolling phase of this wheel event
     */
    Q_PROPERTY(int phase READ phase CONSTANT)
    /**
     * @brief scrolling distance in pixels on screen
     */
    Q_PROPERTY(QPoint pixelDelta READ pixelDelta CONSTANT)
    /**
     * @brief position of mouse cursor at the time of event
     */
    Q_PROPERTY(QPoint pos READ pos CONSTANT)
    /**
     * @brief position of mouse cursor at the time of event
     */
    Q_PROPERTY(QPointF posF READ posF CONSTANT)
    /**
     * @brief source of the event
     */
    Q_PROPERTY(int source READ source CONSTANT)
    /**
     * @brief x position of mouse cursor at the time of event
     */
    Q_PROPERTY(int x READ x CONSTANT)
    /**
     * @brief y position of mouse cursor at the time of event
     */
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

    void clear();
private:
    QWheelEvent *m_wheelEvent = nullptr;
};
