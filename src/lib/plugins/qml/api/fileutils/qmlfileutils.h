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
 * @brief The QmlFileUtils class, exposed to QML as FileUtils
 */
class QmlFileUtils : public QObject
{
    Q_OBJECT
public:
    explicit QmlFileUtils(const QString &filePath, QObject *parent = nullptr);
    /**
     * @brief Get the path of the file within the plugin directory.
     *        If the filePath provided is resolved to be outside the plugin
     *        directory then empty string is returned
     * @param file path within the plugin directory
     * @return resolved path
     */
    Q_INVOKABLE QString resolve(const QString &filePath);
    /**
     * @brief Read the contents of the file within the plugin directory
     * @param file path within the plugin directory
     * @return contents of the file
     */
    Q_INVOKABLE QByteArray readAllFileContents(const QString &fileName);
    /**
     * @brief Checks if the file exists within the plugin directory
     * @param file path within the plugin directory
     * @return true if file exists, otherwise false
     */
    Q_INVOKABLE bool exists(const QString &filePath);
private:
    QString m_path;
};
