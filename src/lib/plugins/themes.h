/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#include <QIcon>
#include "qzcommon.h"

class FALKON_EXPORT Themes : public QObject
{
    Q_OBJECT
public:
    struct Theme {
        bool isValid;
        QIcon icon;
        QString name;
        QString dirName;
        QString author;
        QString description;
        QString license;
    };
    explicit Themes(QObject *parent = nullptr);

    static QList<Theme> getAvailableThemes();
    static QString getActiveTheme();
public Q_SLOTS:
    void showLicense(const QString &themeName);
    void makeCurrent(const QString &themeName);
private:
    static Theme parseTheme(const QString &path, const QString &name);
    Theme getThemeByName(const QString &themeName) const;
};
