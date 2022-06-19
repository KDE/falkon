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
#include "qmlsettings.h"
#include "datapaths.h"
#include <QDebug>

QmlSettings::QmlSettings(QObject *parent)
    : QObject(parent)
{
    m_settingsPath = DataPaths::currentProfilePath() + QL1S("/extensions");
}

bool QmlSettings::setValue(const QVariantMap &map)
{
    if (!m_settings) {
        return false;
    }

    if (!map.contains(QSL("key")) || !map.contains(QSL("value"))) {
        qWarning() << "Unable to set value:" << "cannot determine Key-Value from the argument";
        return false;
    }
    const QString key = map.value(QSL("key")).toString();
    const QVariant value = map.value(QSL("value"));
    m_settings->setValue(key, value);
    return true;
}

QVariant QmlSettings::value(const QVariantMap &map)
{
    if (!m_settings) {
        return {};
    }

    if (!map.contains(QSL("key"))) {
        qWarning() << "Unable to get value:" << "key not defined";
        return {};
    }

    const QString key = map.value(QSL("key")).toString();
    const QVariant defaultValue = map.value(QSL("defaultValue"));
    return m_settings->value(key, defaultValue);
}

bool QmlSettings::contains(const QString &key)
{
    if (!m_settings) {
        return false;
    }

    return m_settings->contains(key);
}

bool QmlSettings::remove(const QString &key)
{
    if (!m_settings) {
        return false;
    }

    m_settings->remove(key);
    return true;
}

bool QmlSettings::sync()
{
    if (!m_settings) {
        return false;
    }

    m_settings->sync();
    return true;
}

QString QmlSettings::name() const
{
    return m_name;
}

void QmlSettings::setName(const QString &name)
{
    m_name = name;
    createSettings();
}

void QmlSettings::createSettings()
{
    m_settingsPath += QL1C('/') + m_name + QL1S("/settings.ini");
    m_settings = new QSettings(m_settingsPath, QSettings::IniFormat, this);
}
