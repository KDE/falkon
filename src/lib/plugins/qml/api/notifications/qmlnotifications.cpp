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
#include "qmlnotifications.h"
#include "mainapplication.h"
#include "desktopnotificationsfactory.h"
#include "qztools.h"
#include "qml/api/fileutils/qmlfileutils.h"

QmlNotifications::QmlNotifications(QObject *parent)
    : QObject(parent)
{
}

void QmlNotifications::create(const QVariantMap &map)
{
    const QString iconUrl = map.value(QSL("icon")).toString();
    QmlFileUtils fileUtils(m_pluginPath);
    const QString iconPath = fileUtils.resolve(iconUrl);
    const QPixmap icon = QPixmap(iconPath);
    const QString heading = map.value(QSL("heading")).toString();
    const QString message = map.value(QSL("message")).toString();
    mApp->desktopNotifications()->showNotification(icon, heading, message);
}

void QmlNotifications::setPluginPath(const QString &path)
{
    m_pluginPath = path;
}
