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

#include <QObject>
#include <QAction>
#include <QVariantMap>

class QmlEngine;

/**
 * @brief The class exposing Action API to QML
 */
class QmlAction : public QObject
{
    Q_OBJECT
public:
    explicit QmlAction(QAction *action, QmlEngine *engine, QObject *parent = nullptr);
    void setProperties(const QVariantMap &map);
    /**
     * @brief Updates the properties of the action
     * @param A JavaScript object containing the updated properties of the action.
     */
    Q_INVOKABLE void update(const QVariantMap &map);

Q_SIGNALS:
    /**
     * @brief This signal is emitted when the action is triggered.
     */
    void triggered();

private:
    QAction *m_action = nullptr;
    QString m_pluginPath;
};
