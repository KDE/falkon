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

extern "C" {
#include <libintl.h>
}

/**
 * @brief The class exposing GNU Gettext to QML
 */
class QmlI18n : public QObject
{
    Q_OBJECT
public:
    explicit QmlI18n(const QString &pluginName, QObject *parent = nullptr);
    void initTranslations();
    /**
     * @brief wrapper for gettext function
     */
    Q_INVOKABLE QString i18n(const QString &string);
    /**
     * @brief wrapper for ngettext function
     */
    Q_INVOKABLE QString i18np(const QString &string1, const QString &string2, int count);
private:
    QString m_pluginName;
    QString m_domain;
};
