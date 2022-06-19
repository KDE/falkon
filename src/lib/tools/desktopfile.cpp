/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 David Rosca <nowrep@gmail.com>
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
#include "desktopfile.h"

#include <QSettings>
#include <QStandardPaths>

DesktopFile::DesktopFile()
= default;

DesktopFile::DesktopFile(const QString &fileName)
{
    m_settings.reset(new QSettings(fileName, QSettings::IniFormat));
    m_settings->setIniCodec("UTF-8");
    m_settings->beginGroup(QSL("Desktop Entry"));
}

QString DesktopFile::fileName() const
{
    return m_settings ? m_settings->fileName() : QString();
}

QString DesktopFile::name() const
{
    return value(QSL("Name"), true).toString();
}

QString DesktopFile::comment() const
{
    return value(QSL("Comment"), true).toString();
}

QString DesktopFile::type() const
{
    return value(QSL("Type")).toString();
}

QString DesktopFile::icon() const
{
    return value(QSL("Icon")).toString();
}

QVariant DesktopFile::value(const QString &key, bool localized) const
{
    if (!m_settings) {
        return {};
    }
    if (localized) {
        const QLocale locale = QLocale::system();
        QString localeKey = QSL("%1[%2]").arg(key, locale.name());
        if (m_settings->contains(localeKey)) {
            return m_settings->value(localeKey);
        }
        localeKey = QSL("%1[%2]").arg(key, locale.bcp47Name());
        if (m_settings->contains(localeKey)) {
            return m_settings->value(localeKey);
        }
        const int i = locale.name().indexOf(QLatin1Char('_'));
        if (i > 0) {
            localeKey = QSL("%1[%2]").arg(key, locale.name().left(i));
            if (m_settings->contains(localeKey)) {
                return m_settings->value(localeKey);
            }
        }
    }
    return m_settings->value(key);
}

bool DesktopFile::tryExec() const
{
    if (!m_settings) {
        return false;
    }

    const QString exec = m_settings->value(QSL("TryExec")).toString();
    return exec.isEmpty() || !QStandardPaths::findExecutable(exec).isEmpty();
}
