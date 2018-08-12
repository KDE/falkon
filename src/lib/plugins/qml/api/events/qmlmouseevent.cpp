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
#include "qmlmouseevent.h"
#include <QQmlEngine>

QmlMouseEvent::QmlMouseEvent(QMouseEvent *mouseEvent, QObject *parent)
    : QObject(parent)
    , m_mouseEvent(mouseEvent)
{
}

int QmlMouseEvent::button() const
{
    if (!m_mouseEvent) {
        return -1;
    }
    return static_cast<int>(m_mouseEvent->button());
}

int QmlMouseEvent::buttons() const
{
    if (!m_mouseEvent) {
        return -1;
    }
    return static_cast<int>(m_mouseEvent->buttons());
}

QPoint QmlMouseEvent::globalPos() const
{
    if (!m_mouseEvent) {
        return QPoint(-1, -1);
    }
    return m_mouseEvent->globalPos();
}

int QmlMouseEvent::globalX() const
{
    if (!m_mouseEvent) {
        return -1;
    }
    return m_mouseEvent->globalX();
}

int QmlMouseEvent::globalY() const
{
    if (!m_mouseEvent) {
        return -1;
    }
    return m_mouseEvent->globalY();
}

QPointF QmlMouseEvent::localPos() const
{
    if (!m_mouseEvent) {
        return QPointF(-1, -1);
    }
    return m_mouseEvent->localPos();
}

QPoint QmlMouseEvent::pos() const
{
    if (!m_mouseEvent) {
        return QPoint(-1, -1);
    }
    return m_mouseEvent->pos();
}

QPointF QmlMouseEvent::screenPos() const
{
    if (!m_mouseEvent) {
        return QPointF(-1, -1);
    }
    return m_mouseEvent->screenPos();
}

int QmlMouseEvent::source() const
{
    if (!m_mouseEvent) {
        return -1;
    }
    return static_cast<int>(m_mouseEvent->source());
}

QPointF QmlMouseEvent::windowPos() const
{
    if (!m_mouseEvent) {
        return QPointF(-1, -1);
    }
    return m_mouseEvent->windowPos();
}

int QmlMouseEvent::x() const
{
    if (!m_mouseEvent) {
        return -1;
    }
    return m_mouseEvent->x();
}

int QmlMouseEvent::y() const
{
    if (!m_mouseEvent) {
        return -1;
    }
    return m_mouseEvent->y();
}

void QmlMouseEvent::clear()
{
    m_mouseEvent = nullptr;
}
