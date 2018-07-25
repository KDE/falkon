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
#include "themes.h"
#include "datapaths.h"
#include "desktopfile.h"
#include "qztools.h"
#include "settings.h"
#include "licenseviewer.h"
#include "mainapplication.h"
#include <QDir>

Themes::Themes(QObject *parent)
    : QObject(parent)
{
}

// static
QList<Themes::Theme> Themes::getAvailableThemes()
{
    QList<Theme> availableThemes;
    const QStringList themePaths = DataPaths::allPaths(DataPaths::Themes);
    foreach (const QString &path, themePaths) {
        QDir dir(path);
        QStringList list = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        foreach (const QString &name, list) {
            Theme themeInfo = parseTheme(dir.absoluteFilePath(name) + QLatin1Char('/'), name);
            if (!themeInfo.isValid) {
                continue;
            }
            availableThemes.append(themeInfo);
        }
    }
    return availableThemes;
}

QString Themes::getActiveTheme()
{
    Settings settings;
    settings.beginGroup("Themes");
    const QString activeTheme = settings.value("activeTheme", DEFAULT_THEME_NAME).toString();
    settings.endGroup();
    return activeTheme;
}

void Themes::showLicense(const QString &themeName)
{
    Theme theme = getThemeByName(themeName);
    if (!theme.isValid) {
        return;
    }
    LicenseViewer *licenseViewer = new LicenseViewer;
    licenseViewer->setText(theme.license);
    licenseViewer->show();
    QzTools::centerWidgetOnScreen(licenseViewer);
}

void Themes::makeCurrent(const QString &themeName)
{
    Theme theme = getThemeByName(themeName);
    if (!theme.isValid) {
        return;
    }
    Settings settings;
    settings.beginGroup("Themes");
    settings.setValue("activeTheme", theme.dirName);
    settings.endGroup();

    mApp->reloadSettings();
}

// static
Themes::Theme Themes::parseTheme(const QString &path, const QString &name)
{
    Theme info;
    info.isValid = false;

    if (!QFile(path + "main.css").exists() || !QFile(path + "metadata.desktop").exists()) {
        info.isValid = false;
        return info;
    }

    DesktopFile metadata(path + QSL("metadata.desktop"));
    info.name = metadata.name();
    info.dirName = name;
    info.description = metadata.comment();
    info.author = metadata.value(QSL("X-Falkon-Author")).toString();

    const QString iconName = metadata.icon();
    if (!iconName.isEmpty()) {
        if (QFileInfo::exists(path + iconName)) {
            info.icon = QIcon(path + iconName);
        } else {
            info.icon = QIcon::fromTheme(iconName);
        }
    }

    const QString licensePath = metadata.value(QSL("X-Falkon-License")).toString();
    if (!licensePath.isEmpty() && QFileInfo::exists(path + licensePath)) {
        info.license = QzTools::readAllFileContents(path + licensePath);
    }

    if (info.name.isEmpty()) {
        return info;
    }

    info.isValid = true;
    return info;
}

Themes::Theme Themes::getThemeByName(const QString &themeName) const
{
    const QStringList themePaths = DataPaths::allPaths(DataPaths::Themes);
    foreach (const QString &path, themePaths) {
        QDir dir(path);
        QStringList list = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        foreach (const QString &name, list) {
            Theme themeInfo = parseTheme(dir.absoluteFilePath(name) + QLatin1Char('/'), name);
            if (!themeInfo.isValid) {
                continue;
            }
            if (themeInfo.name == themeName) {
                return themeInfo;
            }
        }
    }
    Theme info;
    info.isValid = false;
    return info;
}
