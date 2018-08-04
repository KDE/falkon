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
#include "qmlmenu.h"
#include "qztools.h"
#include "qml/api/fileutils/qmlfileutils.h"
#include <QQmlEngine>

QmlMenu::QmlMenu(QMenu *menu, QObject *parent)
    : QObject(parent)
    , m_menu(menu)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);

    connect(m_menu, &QMenu::triggered, this, &QmlMenu::triggered);
}

/**
 * @brief Adds action to menu
 * @param A JavaScript object containing properties for action.
 *        The icon property must be in form of url of the path
 *        and shortcut in form string.
 * @return action of type [QmlAction](@ref QmlAction)
 */
QmlAction *QmlMenu::addAction(const QVariantMap &map)
{
    if (!m_menu) {
        return nullptr;
    }

    QAction *action = new QAction();
    QmlAction *qmlAction = new QmlAction(action, this);
    qmlAction->setPluginPath(m_pluginPath);
    qmlAction->setProperties(map);
    m_menu->addAction(action);

    return qmlAction;
}

/**
 * @brief Adds sub-menu to menu
 * @param A JavaScript object containing properties of menu.
 *        The icon property must be in form of url of the path.
 * @return menu of type [QmlMenu](@ref QmlMenu)
 */
QmlMenu *QmlMenu::addMenu(const QVariantMap &map)
{
    if (!m_menu) {
        return nullptr;
    }

    QMenu *newMenu = new QMenu();
    QMapIterator<QString, QVariant> it(map);
    while (it.hasNext()) {
        const QString key = it.key();
        if (key == QSL("icon")) {
            QString iconPath = map.value(key).toString();
            QIcon icon;
            if (QIcon::hasThemeIcon(iconPath)) {
                icon = QIcon::fromTheme(iconPath);
            } else if (iconPath.startsWith(QSL(":"))) {
                // Icon is loaded from falkon resource
                icon = QIcon(iconPath);
            } else {
                QmlFileUtils fileUtils(m_pluginPath);
                icon = QIcon(fileUtils.resolve(iconPath));
            }
            newMenu->setIcon(icon);
            continue;
        }
        newMenu->setProperty(key.toUtf8(), map.value(key));
    }
    m_menu->addMenu(newMenu);
    QmlMenu *newQmlMenu = new QmlMenu(newMenu, this);
    newQmlMenu->setPluginPath(m_pluginPath);
    return newQmlMenu;
}

/**
 * @brief Adds a separator to menu
 */
void QmlMenu::addSeparator()
{
    if (!m_menu) {
        return;
    }

    m_menu->addSeparator();
}

void QmlMenu::setPluginPath(const QString &path)
{
    m_pluginPath = path;
}
