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

class QmlSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    explicit QmlSettings(QObject *parent = nullptr);
    Q_INVOKABLE bool setValue(const QVariantMap &map);
    Q_INVOKABLE QVariant value(const QVariantMap &map);
    Q_INVOKABLE bool contains(const QString &key);
    Q_INVOKABLE bool remove(const QString &key);
    Q_INVOKABLE bool sync();

private:
    QSettings *m_settings;
    QString m_settingsPath;
    QString m_name;

    QString name() const;
    void setName(const QString &name);
    void createSettings();
};
