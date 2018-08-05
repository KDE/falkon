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
    QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);
}

int QmlMouseEvent::button() const
{
    return (int)m_mouseEvent->button();
}

int QmlMouseEvent::buttons() const
{
    return (int)m_mouseEvent->buttons();
}

QPoint QmlMouseEvent::globalPos() const
{
    return m_mouseEvent->globalPos();
}

int QmlMouseEvent::globalX() const
{
    return m_mouseEvent->globalX();
}

int QmlMouseEvent::globalY() const
{
    return m_mouseEvent->globalY();
}

QPointF QmlMouseEvent::localPos() const
{
    return m_mouseEvent->localPos();
}

QPoint QmlMouseEvent::pos() const
{
    return m_mouseEvent->pos();
}

QPointF QmlMouseEvent::screenPos() const
{
    return m_mouseEvent->screenPos();
}

int QmlMouseEvent::source() const
{
    return (int)m_mouseEvent->source();
}

QPointF QmlMouseEvent::windowPos() const
{
    return m_mouseEvent->windowPos();
}

int QmlMouseEvent::x() const
{
    return m_mouseEvent->x();
}

int QmlMouseEvent::y() const
{
    return m_mouseEvent->y();
}
