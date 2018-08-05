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
#include "qmli18n.h"
#include "qztools.h"
#include <QStandardPaths>

QmlI18n::QmlI18n(const QString &pluginName, QObject *parent)
    : QObject(parent)
{
    m_pluginName = QzTools::filterCharsFromFilename(pluginName);
    // QzTools::filterCharsFromFilename doesn't replaces spaces
    m_pluginName.replace(QLatin1Char(' '), QLatin1Char('_'));
    setlocale(LC_MESSAGES, "");
    initTranslations();
}

void QmlI18n::initTranslations()
{
    QString domain = QString("falkon_%1").arg(m_pluginName);
    QString localeDir = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "locale", QStandardPaths::LocateDirectory);
    bindtextdomain(domain.toUtf8(), localeDir.toUtf8());
    textdomain(domain.toUtf8());
}

QString QmlI18n::i18n(const QString &string)
{
    return QString::fromUtf8(gettext(string.toUtf8()));
}

QString QmlI18n::i18np(const QString &string1, const QString &string2, int count)
{
    return QString::fromUtf8(ngettext(string1.toUtf8(), string2.toUtf8(), count));
}
