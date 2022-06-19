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
#include "qml/qmlengine.h"
#include "qml/qmlstaticdata.h"

QmlMenu::QmlMenu(QMenu *menu, QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_menu(menu)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);

    m_engine = qobject_cast<QmlEngine*>(engine);
    m_pluginPath = m_engine->extensionPath();
    connect(m_menu, &QMenu::triggered, this, &QmlMenu::triggered);
}

QmlAction *QmlMenu::addAction(const QVariantMap &map)
{
    if (!m_menu) {
        return nullptr;
    }

    auto *action = new QAction();
    auto *qmlAction = new QmlAction(action, m_engine, this);
    qmlAction->setProperties(map);
    m_menu->addAction(action);

    return qmlAction;
}

QmlMenu *QmlMenu::addMenu(const QVariantMap &map)
{
    if (!m_menu) {
        return nullptr;
    }

    auto *newMenu = new QMenu();
    for (auto it = map.cbegin(); it != map.cend(); it++) {
        const QString key = it.key();
        if (key == QSL("icon")) {
            const QString iconPath = map.value(key).toString();
            const QIcon icon = QmlStaticData::instance().getIcon(iconPath, m_pluginPath);
            newMenu->setIcon(icon);
            continue;
        }
        newMenu->setProperty(key.toUtf8(), map.value(key));
    }
    m_menu->addMenu(newMenu);
    auto *newQmlMenu = new QmlMenu(newMenu, m_engine, this);
    return newQmlMenu;
}

void QmlMenu::addSeparator()
{
    if (!m_menu) {
        return;
    }

    m_menu->addSeparator();
}
