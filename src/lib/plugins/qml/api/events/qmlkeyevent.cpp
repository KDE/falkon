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
#include "qmlkeyevent.h"
#include <QQmlEngine>

QmlKeyEvent::QmlKeyEvent(QKeyEvent *keyEvent, QObject *parent)
    : QObject(parent)
    , m_keyEvent(keyEvent)
{
}

int QmlKeyEvent::count() const
{
    if (!m_keyEvent) {
        return -1;
    }
    return m_keyEvent->count();
}

bool QmlKeyEvent::isAutoRepeat() const
{
    if (!m_keyEvent) {
        return false;
    }
    return m_keyEvent->isAutoRepeat();
}

int QmlKeyEvent::key() const
{
    if (!m_keyEvent) {
        return -1;
    }
    return m_keyEvent->key();
}

int QmlKeyEvent::modifiers() const
{
    if (!m_keyEvent) {
        return -1;
    }
    return static_cast<int>(m_keyEvent->modifiers());
}

quint32 QmlKeyEvent::nativeModifiers() const
{
    if (!m_keyEvent) {
        return 0;
    }
    return static_cast<quint32>(m_keyEvent->nativeModifiers());
}

quint32 QmlKeyEvent::nativeScanCode() const
{
    if (!m_keyEvent) {
        return 0;
    }
    return static_cast<quint32>(m_keyEvent->nativeScanCode());
}

quint32 QmlKeyEvent::nativeVirtualKey() const
{
    if (!m_keyEvent) {
        return 0;
    }
    return static_cast<quint32>(m_keyEvent->nativeVirtualKey());
}

QString QmlKeyEvent::text() const
{
    if (!m_keyEvent) {
        return {};
    }
    return m_keyEvent->text();
}

void QmlKeyEvent::clear()
{
    m_keyEvent = nullptr;
}
