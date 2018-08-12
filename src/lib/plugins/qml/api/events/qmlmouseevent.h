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

/**
 * @brief The class exposing MouseEvent to QML
 */
class QmlMouseEvent : public QObject
{
    Q_OBJECT
    /**
     * @brief button associated with the event
     */
    Q_PROPERTY(int button READ button CONSTANT)
    /**
     * @brief button state associated with the event
     */
    Q_PROPERTY(int buttons READ buttons CONSTANT)
    /**
     * @brief global position of mouse cursor at the time of event
     */
    Q_PROPERTY(QPoint globalPos READ globalPos CONSTANT)
    /**
     * @brief global x position of mouse cursor at the time of event
     */
    Q_PROPERTY(int globalX READ globalX CONSTANT)
    /**
     * @brief global y position of mouse cursor at the time of event
     */
    Q_PROPERTY(int globalY READ globalY CONSTANT)
    /**
     * @brief local position of mouse cursor at the time of event
     */
    Q_PROPERTY(QPointF localPos READ localPos CONSTANT)
    /**
     * @brief position of mouse cursor at the time of event
     */
    Q_PROPERTY(QPoint pos READ pos CONSTANT)
    /**
     * @brief screen position of mouse cursor at the time of event
     */
    Q_PROPERTY(QPointF screenPos READ screenPos CONSTANT)
    /**
     * @brief source of the event
     */
    Q_PROPERTY(int source READ source CONSTANT)
    /**
     * @brief window position of mouse cursor at the time of event
     */
    Q_PROPERTY(QPointF windowPos READ windowPos CONSTANT)
    /**
     * @brief x position of mouse cursor at the time of event
     */
    Q_PROPERTY(int x READ x CONSTANT)
    /**
     * @brief y position of mouse cursor at the time of event
     */
    Q_PROPERTY(int y READ y CONSTANT)
public:
    explicit QmlMouseEvent(QMouseEvent *mouseEvent = nullptr, QObject *parent = nullptr);
    int button() const;
    int buttons() const;
    QPoint globalPos() const;
    int globalX() const;
    int globalY() const;
    QPointF localPos() const;
    QPoint pos() const;
    QPointF screenPos() const;
    int source() const;
    QPointF windowPos() const;
    int x() const;
    int y() const;

    void clear();

private:
    QMouseEvent *m_mouseEvent = nullptr;
};
