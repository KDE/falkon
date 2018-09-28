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

/**
 * @brief The class exposing KeyEvent to QML
 */
class QmlKeyEvent : public QObject
{
    Q_OBJECT
    /**
     * @brief number of keys involved in this event
     */
    Q_PROPERTY(int count READ count CONSTANT)
    /**
     * @brief checks if the event comes from an auto-repeating key
     */
    Q_PROPERTY(bool autoRepeat READ isAutoRepeat CONSTANT)
    /**
     * @brief key code which is pressed/released
     */
    Q_PROPERTY(int key READ key CONSTANT)
    /**
     * @brief modifiers associated with the event
     */
    Q_PROPERTY(int modifiers READ modifiers CONSTANT)
    /**
     * @brief native modifiers of the event
     */
    Q_PROPERTY(quint32 nativeModifiers READ nativeModifiers CONSTANT)
    /**
     * @brief native scan code of the event
     */
    Q_PROPERTY(quint32 nativeScanCode READ nativeScanCode CONSTANT)
    /**
     * @brief native virtual key, or key sum of the event
     */
    Q_PROPERTY(quint32 nativeVirtualKey READ nativeVirtualKey CONSTANT)
    /**
     * @brief Returns the Unicode text that this key generated
     */
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

    void clear();

private:
    QKeyEvent *m_keyEvent = nullptr;
};
