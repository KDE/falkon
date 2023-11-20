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

#include <QtWebEngineWidgetsVersion>


HTML5PermissionsManager::HTML5PermissionsManager(QObject* parent)
    : QObject(parent)
{
    loadSettings();
}

void HTML5PermissionsManager::requestPermissions(WebPage* page, const QUrl &origin, const QWebEnginePage::Feature &feature)
{
    if (!page) {
        return;
    }

    if (!m_granted.contains(feature) || !m_denied.contains(feature)) {
        qWarning() << "HTML5PermissionsManager: Unknown feature" << feature;
        return;
    }

    // Permission granted
    if (m_granted.value(feature).contains(origin.toString())) {
        page->setFeaturePermission(origin, feature, QWebEnginePage::PermissionGrantedByUser);
        return;
    }

    // Permission denied
    if (m_denied.value(feature).contains(origin.toString())) {
        page->setFeaturePermission(origin, feature, QWebEnginePage::PermissionDeniedByUser);
        return;
    }

    // Ask user for permission
    auto* notif = new HTML5PermissionsNotification(origin, page, feature);
    page->view()->addNotification(notif);
}

void HTML5PermissionsManager::rememberPermissions(const QUrl &origin, const QWebEnginePage::Feature &feature,
        const QWebEnginePage::PermissionPolicy &policy)
{
    if (origin.isEmpty()) {
        return;
    }

    if (policy == QWebEnginePage::PermissionGrantedByUser) {
        m_granted[feature].append(origin.toString());
    }
    else {
        m_denied[feature].append(origin.toString());
    }

    saveSettings();
}

void HTML5PermissionsManager::loadSettings()
{
    Settings settings;
    settings.beginGroup(QSL("HTML5Notifications"));

    m_granted[QWebEnginePage::Notifications] = settings.value(QSL("NotificationsGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::Notifications] = settings.value(QSL("NotificationsDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::Geolocation] = settings.value(QSL("GeolocationGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::Geolocation] = settings.value(QSL("GeolocationDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::MediaAudioCapture] = settings.value(QSL("MediaAudioCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::MediaAudioCapture] = settings.value(QSL("MediaAudioCaptureDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::MediaVideoCapture] = settings.value(QSL("MediaVideoCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::MediaVideoCapture] = settings.value(QSL("MediaVideoCaptureDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::MediaAudioVideoCapture] = settings.value(QSL("MediaAudioVideoCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::MediaAudioVideoCapture] = settings.value(QSL("MediaAudioVideoCaptureDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::MouseLock] = settings.value(QSL("MouseLockGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::MouseLock] = settings.value(QSL("MouseLockDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::DesktopVideoCapture] = settings.value(QSL("DesktopVideoCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::DesktopVideoCapture] = settings.value(QSL("DesktopVideoCaptureDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::DesktopAudioVideoCapture] = settings.value(QSL("DesktopAudioVideoCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::DesktopAudioVideoCapture] = settings.value(QSL("DesktopAudioVideoCaptureDenied"), QStringList()).toStringList();

    settings.endGroup();
}

void HTML5PermissionsManager::saveSettings()
{
    Settings settings;
    settings.beginGroup(QSL("HTML5Notifications"));

    settings.setValue(QSL("NotificationsGranted"), m_granted[QWebEnginePage::Notifications]);
    settings.setValue(QSL("NotificationsDenied"), m_denied[QWebEnginePage::Notifications]);

    settings.setValue(QSL("GeolocationGranted"), m_granted[QWebEnginePage::Geolocation]);
    settings.setValue(QSL("GeolocationDenied"), m_denied[QWebEnginePage::Geolocation]);

    settings.setValue(QSL("MediaAudioCaptureGranted"), m_granted[QWebEnginePage::MediaAudioCapture]);
    settings.setValue(QSL("MediaAudioCaptureDenied"), m_denied[QWebEnginePage::MediaAudioCapture]);

    settings.setValue(QSL("MediaVideoCaptureGranted"), m_granted[QWebEnginePage::MediaVideoCapture]);
    settings.setValue(QSL("MediaVideoCaptureDenied"), m_denied[QWebEnginePage::MediaVideoCapture]);

    settings.setValue(QSL("MediaAudioVideoCaptureGranted"), m_granted[QWebEnginePage::MediaAudioVideoCapture]);
    settings.setValue(QSL("MediaAudioVideoCaptureDenied"), m_denied[QWebEnginePage::MediaAudioVideoCapture]);

    settings.setValue(QSL("MouseLockGranted"), m_granted[QWebEnginePage::MouseLock]);
    settings.setValue(QSL("MouseLockDenied"), m_denied[QWebEnginePage::MouseLock]);

    settings.setValue(QSL("DesktopVideoCaptureGranted"), m_granted[QWebEnginePage::DesktopVideoCapture]);
    settings.setValue(QSL("DesktopVideoCaptureDenied"), m_denied[QWebEnginePage::DesktopVideoCapture]);

    settings.setValue(QSL("DesktopAudioVideoCaptureGranted"), m_granted[QWebEnginePage::DesktopAudioVideoCapture]);
    settings.setValue(QSL("DesktopAudioVideoCaptureDenied"), m_denied[QWebEnginePage::DesktopAudioVideoCapture]);

    settings.endGroup();
}
