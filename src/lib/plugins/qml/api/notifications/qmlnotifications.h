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

/**
 * @brief The class to display notifications
 */
class QmlNotifications : public QObject
{
    Q_OBJECT
public:
    explicit QmlNotifications(QObject *parent = nullptr);
    /**
     * @brief Create and display a notification
     * @param JavaScript object containing
     *        - icon:
     *          String representing the icon file url. The icon path will be
     *          search in the following order
     *          - Falkon resource: for the icons starting with ":", they are searched in
     *               falkon resource file
     *          - Files in plugin directory: All other paths will be resolved relative to
     *               the plugin directory. If the icon path is outside the
     *               plugin directory, then it will be resolved as empty path.
     *        - heading:
     *          String representing the heading of the notification
     *        - message:
     *          String representing the message of the notification
     */
    Q_INVOKABLE void create(const QVariantMap &map);
    void setPluginPath(const QString &path);
private:
    QString m_pluginPath;
};
