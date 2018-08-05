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

#include "qmlaction.h"
#include <QMenu>

/**
 * @brief The class exposing WebView contextmenu to QML as Menu API
 */
class QmlMenu : public QObject
{
    Q_OBJECT
public:
    explicit QmlMenu(QMenu *menu, QObject *parent = nullptr);
    /**
     * @brief Adds action to menu
     * @param A JavaScript object containing properties for action.
     *        The icon property must be in form of url of the path
     *        and shortcut in form string.
     * @return action of type [QmlAction](@ref QmlAction)
     */
    Q_INVOKABLE QmlAction *addAction(const QVariantMap &map);
    /**
     * @brief Adds sub-menu to menu
     * @param A JavaScript object containing properties of menu.
     *        The icon property must be in form of url of the path.
     * @return menu of type [QmlMenu](@ref QmlMenu)
     */
    Q_INVOKABLE QmlMenu *addMenu(const QVariantMap &map);
    /**
     * @brief Adds a separator to menu
     */
    Q_INVOKABLE void addSeparator();
    void setPluginPath(const QString &path);

Q_SIGNALS:
    /**
     * @brief This signal is emitted when the menu is triggred
     */
    void triggered();

private:
    QMenu *m_menu;
    QString m_pluginPath;
};
