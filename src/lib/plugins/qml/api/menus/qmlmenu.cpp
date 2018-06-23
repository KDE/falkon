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

QmlMenu::QmlMenu(QMenu *menu, QObject *parent)
    : QObject(parent)
    , m_menu(menu)
{
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
    for (const QString &key : map.keys()) {
        if (key == QSL("icon")) {
            QUrl url = map.value(key).toUrl();
            QIcon icon(QzTools::getPathFromUrl(url));
            newMenu->setIcon(icon);
            continue;
        }
        newMenu->setProperty(key.toUtf8(), map.value(key));
    }
    m_menu->addMenu(newMenu);
    QmlMenu *newQmlMenu = new QmlMenu(newMenu, this);
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
