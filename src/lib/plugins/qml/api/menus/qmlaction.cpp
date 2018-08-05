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
#include "qmlaction.h"
#include "qztools.h"
#include "qml/api/fileutils/qmlfileutils.h"

QmlAction::QmlAction(QAction *action, QObject *parent)
    : QObject(parent)
    , m_action(action)
{
    connect(m_action, &QAction::triggered, this, &QmlAction::triggered);
}

void QmlAction::setProperties(const QVariantMap &map)
{
    if (!m_action) {
        return;
    }

    QMapIterator<QString, QVariant> it(map);
    while (it.hasNext()) {
        it.next();
        const QString key = it.key();
        if (key == QSL("icon")) {
            QString iconPath = map.value(key).toString();
            QIcon icon;
            if (QIcon::hasThemeIcon(iconPath)) {
                icon = QIcon::fromTheme(iconPath);
            } else {
                QmlFileUtils fileUtils(m_pluginPath);
                icon = QIcon(fileUtils.resolve(iconPath));
            }
            m_action->setIcon(icon);
        } else if (key == QSL("shortcut")) {
            m_action->setShortcut(QKeySequence(map.value(key).toString()));
        } else {
            m_action->setProperty(key.toUtf8(), map.value(key));
        }
    }
}

void  QmlAction::update(const QVariantMap &map)
{
    setProperties(map);
}

void QmlAction::setPluginPath(const QString &path)
{
    m_pluginPath = path;
}
