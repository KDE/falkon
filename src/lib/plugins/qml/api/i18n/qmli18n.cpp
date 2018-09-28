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
    m_pluginName = pluginName;
    initTranslations();
}

void QmlI18n::initTranslations()
{
    m_domain = QString(QSL("falkon_%1")).arg(m_pluginName);
    const QString localeDir = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QSL("locale"), QStandardPaths::LocateDirectory);
    const bool isLanguageSet = qEnvironmentVariableIsSet("LANGUAGE");
    const QByteArray language = qgetenv("LANGUAGE");
    qputenv("LANGUAGE", QLocale::system().name().toUtf8());
    bindtextdomain(m_domain.toUtf8(), localeDir.toUtf8());
    if (!isLanguageSet) {
        qunsetenv("LANGUAGE");
    } else {
        qputenv("LANGUAGE", language);
    }
}

QString QmlI18n::i18n(const QString &string)
{
    return QString::fromUtf8(dgettext(m_domain.toUtf8(), string.toUtf8()));
}

QString QmlI18n::i18np(const QString &string1, const QString &string2, int count)
{
    return QString::fromUtf8(dngettext(m_domain.toUtf8(), string1.toUtf8(), string2.toUtf8(), count));
}
