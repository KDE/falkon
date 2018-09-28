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
#include <QSettings>

/**
 * @brief The class exposing Settings API to QML
 */
class QmlSettings : public QObject
{
    Q_OBJECT

    /**
     * @brief name of the folder in which settings.ini file is located
     *        on the standard extension path.
     */
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    explicit QmlSettings(QObject *parent = nullptr);
    /**
     * @brief Sets the value for a given key.
     * @param A JavaScript object containing
     *        - key: QString representing the key
     *        - value: QVariant representing the value for the key
     * @return true if value is set, else false
     */
    Q_INVOKABLE bool setValue(const QVariantMap &map);
    /**
     * @brief Gets the value for a given key.
     * @param A JavaScript object containing
     *        - key: QString representing the key
     *        - defaultValue: QVariant representing the default value for the key
     * @return QVariant representing value
     */
    Q_INVOKABLE QVariant value(const QVariantMap &map);
    /**
     * @brief Checks if a given key exists.
     * @param QString representing the key
     * @return true if key exists, else false
     */
    Q_INVOKABLE bool contains(const QString &key);
    /**
     * @brief Removes the given key-value from the settings.
     * @param QString representing the key
     * @return true if key-value pair is removed, else false
     */
    Q_INVOKABLE bool remove(const QString &key);
    /**
     * @brief syncs the settings
     * @return true if success, else false
     */
    Q_INVOKABLE bool sync();

private:
    QSettings *m_settings = nullptr;
    QString m_settingsPath;
    QString m_name;

    QString name() const;
    void setName(const QString &name);
    void createSettings();
};
