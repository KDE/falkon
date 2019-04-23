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
#include "qml/qmlplugincontext.h"
#include "qml/qmlstaticdata.h"

#include <QQmlEngine>

QmlMenu::QmlMenu(QMenu *menu, QObject *parent)
    : QObject(parent)
    , m_menu(menu)
{
    Q_ASSERT(m_menu);
    connect(m_menu, &QMenu::triggered, this, &QmlMenu::triggered);
}

QJSValue QmlMenu::addAction(const QVariantMap &map)
{
    QAction *action = new QAction();
    QmlAction *qmlAction = new QmlAction(action, this);
    QQmlEngine::setContextForObject(qmlAction, QmlPluginContext::contextForObject(this));
    action->setParent(qmlAction);
    qmlAction->setProperties(map);
    m_menu->addAction(action);
    return qmlEngine(this)->newQObject(qmlAction);
}

QJSValue QmlMenu::addMenu(const QVariantMap &map)
{
    QMenu *newMenu = new QMenu();
    for (auto it = map.cbegin(); it != map.cend(); it++) {
        const QString key = it.key();
        if (key == QSL("icon")) {
            const QString iconPath = map.value(key).toString();
            const QIcon icon = QmlStaticData::instance().getIcon(iconPath, QmlPluginContext::contextForObject(this)->pluginPath());
            newMenu->setIcon(icon);
            continue;
        }
        newMenu->setProperty(key.toUtf8(), map.value(key));
    }
    m_menu->addMenu(newMenu);
    QmlMenu *newQmlMenu = new QmlMenu(newMenu, this);
    QQmlEngine::setContextForObject(newQmlMenu, QmlPluginContext::contextForObject(this));
    connect(newQmlMenu, &QObject::destroyed, newMenu, &QObject::deleteLater);
    return qmlEngine(this)->newQObject(newQmlMenu);
}

void QmlMenu::addSeparator()
{
    m_menu->addSeparator();
}
