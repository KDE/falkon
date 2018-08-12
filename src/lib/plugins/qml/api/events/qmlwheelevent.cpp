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
#include "qmlwheelevent.h"
#include <QQmlEngine>

QmlWheelEvent::QmlWheelEvent(QWheelEvent *wheelEvent, QObject *parent)
    : QObject(parent)
    , m_wheelEvent(wheelEvent)
{
}

QPoint QmlWheelEvent::angleDelta() const
{
    if (!m_wheelEvent) {
        return QPoint(-1, -1);
    }
    return m_wheelEvent->angleDelta();
}

int QmlWheelEvent::buttons() const
{
    if (!m_wheelEvent) {
        return -1;
    }
    return static_cast<int>(m_wheelEvent->buttons());
}

QPoint QmlWheelEvent::globalPos() const
{
    if (!m_wheelEvent) {
        return QPoint(-1, -1);
    }
    return m_wheelEvent->globalPos();
}

QPointF QmlWheelEvent::globalPosF() const
{
    if (!m_wheelEvent) {
        return QPointF(-1, -1);
    }
    return m_wheelEvent->globalPosF();
}

int QmlWheelEvent::globalX() const
{
    if (!m_wheelEvent) {
        return -1;
    }
    return m_wheelEvent->globalX();
}

int QmlWheelEvent::globalY() const
{
    if (!m_wheelEvent) {
        return -1;
    }
    return m_wheelEvent->globalY();
}

bool QmlWheelEvent::inverted() const
{
    if (!m_wheelEvent) {
        return false;
    }
    return m_wheelEvent->inverted();
}

int QmlWheelEvent::phase() const
{
    if (!m_wheelEvent) {
        return -1;
    }
    return static_cast<int>(m_wheelEvent->phase());
}

QPoint QmlWheelEvent::pixelDelta() const
{
    if (!m_wheelEvent) {
        return QPoint(-1, -1);
    }
    return m_wheelEvent->pixelDelta();
}

QPoint QmlWheelEvent::pos() const
{
    if (!m_wheelEvent) {
        return QPoint(-1, -1);
    }
    return m_wheelEvent->pos();
}

QPointF QmlWheelEvent::posF() const
{
    if (!m_wheelEvent) {
        return QPointF(-1, -1);
    }
    return m_wheelEvent->posF();
}

int QmlWheelEvent::source() const
{
    if (!m_wheelEvent) {
        return -1;
    }
    return static_cast<int>(m_wheelEvent->source());
}

int QmlWheelEvent::x() const
{
    if (!m_wheelEvent) {
        return -1;
    }
    return m_wheelEvent->x();
}

int QmlWheelEvent::y() const
{
    if (!m_wheelEvent) {
        return -1;
    }
    return m_wheelEvent->y();
}

void QmlWheelEvent::clear()
{
    m_wheelEvent = nullptr;
}
