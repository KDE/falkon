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
    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(bool autoRepeat READ isAutoRepeat CONSTANT)
    Q_PROPERTY(int key READ key CONSTANT)
    Q_PROPERTY(int modifiers READ modifiers CONSTANT)
    Q_PROPERTY(quint32 nativeModifiers READ nativeModifiers CONSTANT)
    Q_PROPERTY(quint32 nativeScanCode READ nativeScanCode CONSTANT)
    Q_PROPERTY(quint32 nativeVirtualKey READ nativeVirtualKey CONSTANT)
    Q_PROPERTY(QString text READ text CONSTANT)
public:
    explicit QmlKeyEvent(QKeyEvent *keyEvent = nullptr, QObject *parent = nullptr);
    int count() const;
    bool isAutoRepeat() const;
    int key() const;
    int modifiers() const;
    quint32 nativeModifiers() const;
    quint32 nativeScanCode() const;
    quint32 nativeVirtualKey() const;
    QString text() const;

private:
    QKeyEvent *m_keyEvent;
};
