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
#include "sitesettingsmanager.h"

#include "mainapplication.h"
#include "settings.h"
#include "sqldatabase.h"
#include <QUrl>
#include <qwebenginesettings.h>

SiteSettingsManager::SiteSettingsManager ( QObject* parent )
: QObject(parent)
{
    loadSettings();
}

SiteSettingsManager::~SiteSettingsManager() noexcept
{
}

void SiteSettingsManager::loadSettings()
{
    Settings settings;
    settings.beginGroup("Web-Browser-Settings");
//    m_isSaving = settings.value("allowPerDomainZoom", true).toBool();
    settings.endGroup();
}

SiteWebEngineSettings SiteSettingsManager::getWebEngineSettings(const QUrl& url)
{
    SiteWebEngineSettings settings;

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare("SELECT allow_images, allow_javascript FROM history WHERE server=?");
    query.bindValue(0, url.host());
    query.exec();

    if (query.next()) {
        int allow_images = query.value("allow_images").toInt();
        int allow_javascript = query.value("allow_javascript").toInt();

        if (allow_images == 0) {
            settings.allowImages = mApp->webSettings()->testAttribute(QWebEngineSettings::AutoLoadImages);
        }
        else {
            settings.allowImages = (allow_images == 1);
        }

        if (allow_javascript == 0) {
            settings.allowJavaScript = mApp->webSettings()->testAttribute(QWebEngineSettings::JavascriptEnabled);
        }
        else {
            settings.allowJavaScript = (allow_javascript == 1);
        }
    }
    else {
        settings.allowImages = mApp->webSettings()->testAttribute(QWebEngineSettings::AutoLoadImages);
        settings.allowJavaScript = mApp->webSettings()->testAttribute(QWebEngineSettings::JavascriptEnabled);
    }

    return settings;
}
