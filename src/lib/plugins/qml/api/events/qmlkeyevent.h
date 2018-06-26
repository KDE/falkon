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

#include <QKeyEvent>
#include <QObject>

class QmlKeyEvent : public QObject
{
    Q_OBJECT
public:
    explicit QmlKeyEvent(QKeyEvent *keyEvent = nullptr, QObject *parent = nullptr);
    Q_INVOKABLE int count() const;
    Q_INVOKABLE bool isAutoRepeat() const;
    Q_INVOKABLE int key() const;
    Q_INVOKABLE int modifiers() const;
    Q_INVOKABLE quint32 nativeModifiers() const;
    Q_INVOKABLE quint32 nativeScanCode() const;
    Q_INVOKABLE quint32 nativeVirtualKey() const;
    Q_INVOKABLE QString text() const;

private:
    QKeyEvent *m_keyEvent;
};
