/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QWidget>
#include <QIcon>
#include <QHash>

#include "qzcommon.h"


namespace Ui
{
class ThemeManager;
}

class Preferences;

class FALKON_EXPORT ThemeManager : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeManager(QWidget* parent, Preferences* preferences);
    ~ThemeManager();

    void save();

private Q_SLOTS:
    void currentChanged();
    void showLicense();
    void removeTheme();

private:
    struct Theme {
        bool isValid;
        QIcon icon;
        QString name;
        QString author;
        QString description;
        QString license;
        QString themePath;
    };

    Theme parseTheme(const QString &path, const QString &name);

    Ui::ThemeManager* ui;
    Preferences* m_preferences;

    QString m_activeTheme;
    QHash<QString, Theme> m_themeHash;
};

#endif // THEMEMANAGER_H
