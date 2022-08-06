/* ============================================================
 * Falkon - Qt web browser
 * Copyright (C) 2022 Juraj Oravec <jurajoravec@mailo.com>
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
#ifndef SITESETTINGS_MANAGER_H
#define SITESETTINGS_MANAGER_H

#include "qzcommon.h"

class QUrl;

class SiteWebEngineSettings
{
public:
    bool allowJavaScript;
    bool allowImages;
};

class FALKON_EXPORT SiteSettingsManager : QObject
{
    Q_OBJECT

public:
    explicit SiteSettingsManager(QObject *parent = 0);
    ~SiteSettingsManager();

    void loadSettings();

    /**
     * @brief Get settings which should be applied to webpage before loading.
     *        Since this is using blocking database querry group all settings
     *        in one SQL call for faster laoding.
     * @param url Address for which to fetch the settings
     */
    SiteWebEngineSettings getWebEngineSettings(const QUrl &url);

private:
};

#endif // SITESETTINGS_MANAGER_H
