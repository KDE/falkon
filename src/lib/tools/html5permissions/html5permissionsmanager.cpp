/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2015  David Rosca <nowrep@gmail.com>
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
#include "html5permissionsmanager.h"
#include "html5permissionsnotification.h"
#include "settings.h"
#include "webview.h"
#include "mainapplication.h"
#include "sitesettingsmanager.h"

#include <QtWebEngineWidgetsVersion>


HTML5PermissionsManager::HTML5PermissionsManager(QObject* parent)
    : QObject(parent)
{
}

void HTML5PermissionsManager::requestPermissions(WebPage* page, const QUrl &origin, const QWebEnginePage::Feature &feature)
{
    if (!page) {
        return;
    }

    if (!checkFeature(feature)) {
        qWarning() << "HTML5PermissionsManager: Unknown feature" << feature;
        return;
    }

    const auto permission = mApp->siteSettingsManager()->getPermission(feature, origin);

    if (permission == SiteSettingsManager::Allow) {
        page->setFeaturePermission(origin, feature, QWebEnginePage::PermissionGrantedByUser);
    }
    else if (permission == SiteSettingsManager::Deny) {
        page->setFeaturePermission(origin, feature, QWebEnginePage::PermissionDeniedByUser);
    }
    else {
        auto* notif = new HTML5PermissionsNotification(origin, page, feature);
        page->view()->addNotification(notif);
    }
}

void HTML5PermissionsManager::rememberPermissions(const QUrl &origin, const QWebEnginePage::Feature &feature,
        const QWebEnginePage::PermissionPolicy &policy)
{
    if (origin.isEmpty()) {
        return;
    }

    if (policy == QWebEnginePage::PermissionGrantedByUser) {
        mApp->siteSettingsManager()->setOption(feature, origin, SiteSettingsManager::Allow);
    }
    else {
        mApp->siteSettingsManager()->setOption(feature, origin, SiteSettingsManager::Deny);
    }
}

bool HTML5PermissionsManager::checkFeature(const QWebEnginePage::Feature& feature)
{
    switch (feature) {
        case QWebEnginePage::Notifications:
        case QWebEnginePage::Geolocation:
        case QWebEnginePage::MediaAudioCapture:
        case QWebEnginePage::MediaVideoCapture:
        case QWebEnginePage::MediaAudioVideoCapture:
        case QWebEnginePage::MouseLock:
        case QWebEnginePage::DesktopVideoCapture:
        case QWebEnginePage::DesktopAudioVideoCapture:
            return true;

        default:
            return false;
    }
}
