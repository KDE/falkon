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

SiteSettingsManager::SiteSettingsManager ( QObject* parent )
: QObject(parent)
{
    loadSettings();

    supportedAttribute.append(QWebEngineSettings::AutoLoadImages);
    supportedAttribute.append(QWebEngineSettings::JavascriptEnabled);
    supportedAttribute.append(QWebEngineSettings::JavascriptCanOpenWindows);
    supportedAttribute.append(QWebEngineSettings::JavascriptCanAccessClipboard);
    supportedAttribute.append(QWebEngineSettings::JavascriptCanPaste);
    supportedAttribute.append(QWebEngineSettings::AllowWindowActivationFromJavaScript);
    supportedAttribute.append(QWebEngineSettings::LocalStorageEnabled);
    supportedAttribute.append(QWebEngineSettings::ScrollAnimatorEnabled);
    supportedAttribute.append(QWebEngineSettings::FullScreenSupportEnabled);
    supportedAttribute.append(QWebEngineSettings::AllowRunningInsecureContent);
    supportedAttribute.append(QWebEngineSettings::AllowGeolocationOnInsecureOrigins);
    supportedAttribute.append(QWebEngineSettings::PlaybackRequiresUserGesture);
    supportedAttribute.append(QWebEngineSettings::WebRTCPublicInterfacesOnly);


    attributesSql = QSL("SELECT ");

    for (int i = 0; i < supportedAttribute.size(); ++i) {
        if (i > 0) {
            attributesSql.append(QSL(", "));
        }
        attributesSql.append(webAttributeToSqlColumn(supportedAttribute[i]));
    }

    attributesSql.append(QSL(" FROM site_settings WHERE server=?"));
}

SiteSettingsManager::~SiteSettingsManager() noexcept
{
}

void SiteSettingsManager::loadSettings()
{
    Settings settings;
//    settings.beginGroup("Web-Browser-Settings");
//    m_isSaving = settings.value("allowPerDomainZoom", true).toBool();
//    settings.endGroup();

    // TODO Maybe change to Default HTML5 Permissions
    settings.beginGroup("Site-Settings");
/* These are handled by already existing parts of Falkon */
//     m_defaults[poAllowCookies]                  = settings.value("allowCookies", Default).toInt();
//     m_defaults[poZoomLevel]                     = settings.value("defaultZoomLevel", Default).toInt(); // fail
//
//     m_defaults[poAutoloadImages]                         = intToPermission(settings.value("AllowImages",                         Ask).toInt());
//
//     m_defaults[poJavascriptEnabled]                   = intToPermission(settings.value("JavascriptEnabled",                   Ask).toInt());
//     m_defaults[poJavascriptCanOpenWindows]            = intToPermission(settings.value("JavascriptCanOpenWindows",            Ask).toInt());
//     m_defaults[poJavascriptCanAccessClipboard]        = intToPermission(settings.value("JavascriptCanAccessClipboard",        Ask).toInt());
//     m_defaults[poJavascriptCanPaste]                  = intToPermission(settings.value("JavascriptCanPaste",                  Ask).toInt());
//     m_defaults[poAllowWindowActivationFromJavaScript] = intToPermission(settings.value("AllowWindowActivationFromJavaScript", Ask).toInt());
//
//     m_defaults[poLocalStorageEnabled]                 = intToPermission(settings.value("LocalStorageEnabled",                 Ask).toInt());
//     m_defaults[poScrollAnimatorEnabled]               = intToPermission(settings.value("ScrollAnimatorEnabled",               Ask).toInt());
//     m_defaults[poFullScreenSupportEnabled]            = intToPermission(settings.value("FullScreenSupportEnabled",            Ask).toInt());
//     m_defaults[poAllowRunningInsecureContent]         = intToPermission(settings.value("AllowRunningInsecureContent",         Ask).toInt());
//     m_defaults[poAllowGeolocationOnInsecureOrigins]   = intToPermission(settings.value("AllowGeolocationOnInsecureOrigins",   Ask).toInt());
//     m_defaults[poPlaybackRequiresUserGesture]         = intToPermission(settings.value("PlaybackRequiresUserGesture",         Ask).toInt());
//     m_defaults[poWebRTCPublicInterfacesOnly]          = intToPermission(settings.value("WebRTCPublicInterfacesOnly",          Ask).toInt());


    m_defaults[poAllowNotifications]            = intToPermission(settings.value("allowNotifications",              Ask).toInt());
    m_defaults[poAllowGeolocation]              = intToPermission(settings.value("allowGealocation",                Ask).toInt());
    m_defaults[poAllowMediaAudioCapture]        = intToPermission(settings.value("allowMicrophone",                 Ask).toInt());
    m_defaults[poAllowMediaVideoCapture]        = intToPermission(settings.value("allowCamera",                     Ask).toInt());
    m_defaults[poAllowMediaAudioVideoCapture]   = intToPermission(settings.value("allowCameraAndMicrophone",        Ask).toInt());
    m_defaults[poAllowMouseLock]                = intToPermission(settings.value("allowMouseLock",                  Ask).toInt());
    m_defaults[poAllowDesktopVideoCapture]      = intToPermission(settings.value("allowDesktopVideoCapture",        Ask).toInt());
    m_defaults[poAllowDesktopAudioVideoCapture] = intToPermission(settings.value("allowDesktopAudioVideoCapture",   Ask).toInt());
    settings.endGroup();
}

void SiteSettingsManager::saveSettings()
{
    Settings settings;
    settings.beginGroup("Site-Settings");
/* These are handled by already existing parts of Falkon */
//     settings.setValue("allowJavascript", m_defaults[poJavascriptEnabled]);
//     settings.setValue("allowIbutmages", m_defaults[poAutoloadImages]);
//     settings.setValue("allowCookies", m_defaults[poAllowCookies]);
//     settings.setValue("defaultZoomLevel", m_defaults[poZoomLevel]);
    settings.setValue("allowNotifications",             m_defaults[poAllowNotifications]);
    settings.setValue("allowGealocation",               m_defaults[poAllowGeolocation]);
    settings.setValue("allowMicrophone",                m_defaults[poAllowMediaAudioCapture]);
    settings.setValue("allowCamera",                    m_defaults[poAllowMediaVideoCapture]);
    settings.setValue("allowCameraAndMicrophone",       m_defaults[poAllowMediaAudioVideoCapture]);
    settings.setValue("allowMouseLock",                 m_defaults[poAllowMouseLock]);
    settings.setValue("allowDesktopVideoCapture",       m_defaults[poAllowDesktopVideoCapture]);
    settings.setValue("allowDesktopAudioVideoCapture",  m_defaults[poAllowDesktopAudioVideoCapture]);
    settings.endGroup();
}

QHash<QWebEngineSettings::WebAttribute, bool> SiteSettingsManager::getWebAttributes(const QUrl& url)
{
    QHash<QWebEngineSettings::WebAttribute, bool> attributes;

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(attributesSql);
    query.addBindValue(url.host());
    query.exec();

    if (query.next()) {
        for (int i = 0; i < query.record().count(); ++i) {
            Permission perm = intToPermission(query.value(i).toInt());
            QWebEngineSettings::WebAttribute attribute = supportedAttribute[i];

            if (perm == Allow) {
                attributes[attribute] = true;
            }
            else if (perm == Deny) {
                attributes[attribute] = false;
            }
            else {
                attributes[attribute] = mApp->webSettings()->testAttribute(attribute);
            }
        }
    }

    return attributes;
}

void SiteSettingsManager::setJavascript(const QUrl& url, const int value)
{
    setOption(poJavascriptEnabled, url, value);
}

void SiteSettingsManager::setImages(const QUrl& url, const int value)
{
    setOption(poAutoloadImages, url, value);
}

void SiteSettingsManager::setOption(const PageOptions option, const QUrl& url, const int value)
{
    QString column = optionToSqlColumn(option);

    if (column.isEmpty()) {
        return;
    }

    auto job = new SqlQueryJob(QSL("UPDATE site_settings SET %1=? WHERE server=?").arg(column), this);
    job->addBindValue(value);
    job->addBindValue(url.host());
    connect(job, &SqlQueryJob::finished, this, [=]() {
        if (job->numRowsAffected() == 0) {
            auto job = new SqlQueryJob(QSL("INSERT INTO site_settings (server, %1) VALUES (?,?)").arg(column), this);
            job->addBindValue(url.host());
            job->addBindValue(value);
            job->start();
        }
    });
    job->start();
}

void SiteSettingsManager::setOption(const QWebEnginePage::Feature& feature, const QUrl& url, const Permission &value)
{
    auto option = optionFromWebEngineFeature(feature);
    setOption(option, url, value);
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const SiteSettingsManager::PageOptions option, const QUrl& url)
{
    QString column = optionToSqlColumn(option);

    if (column.isEmpty()) {
        return Deny;
    }

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(QSL("SELECT %1 FROM site_settings WHERE server=?").arg(column));
    query.addBindValue(url.host());
    query.exec();

    if (query.next()) {
        int allow_option = query.value(column).toInt();

        return intToPermission(allow_option);
    }

    return Default;
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QWebEnginePage::Feature& feature, const QUrl& url)
{
    auto option = optionFromWebEngineFeature(feature);
    return getPermission(option, url);
}

QString SiteSettingsManager::optionToSqlColumn(const SiteSettingsManager::PageOptions &option)
{
    switch (option) {
        case poJavascriptEnabled:
            return QSL("wa_js_enabled");
        case poAutoloadImages:
            return QSL("wa_autoload_images");
        case poAllowCookies:
            return QSL("allow_cookies");
        case poZoomLevel:
            return QSL("zoom_level");
        case poAllowNotifications:
            return QSL("allow_notifications");
        case poAllowGeolocation:
            return QSL("allow_geolocation");
        case poAllowMediaAudioCapture:
            return QSL("allow_media_audio_capture");
        case poAllowMediaVideoCapture:
            return QSL("allow_media_video_capture");
        case poAllowMediaAudioVideoCapture:
            return QSL("allow_media_audio_video_capture");
        case poAllowMouseLock:
            return QSL("allow_mouse_lock");
        case poAllowDesktopVideoCapture:
            return QSL("allow_desktop_video_capture");
        case poAllowDesktopAudioVideoCapture:
            return QSL("allow_desktop_audio_video_capture");
        default:
            qWarning() << "Unknown option" << option;
            return QLatin1String("");
    }
}

SiteSettingsManager::Permission SiteSettingsManager::getDefaultPermission(const SiteSettingsManager::PageOptions& option)
{
    switch (option) {
        case poJavascriptEnabled:
            return testAttribute(QWebEngineSettings::JavascriptEnabled);
        case poAutoloadImages:
            return testAttribute(QWebEngineSettings::AutoLoadImages);
        case poAllowNotifications:
            return m_defaults[poAllowNotifications];
        case poAllowGeolocation:
            return m_defaults[poAllowGeolocation];
        case poAllowMediaAudioCapture:
            return m_defaults[poAllowMediaAudioCapture];
        case poAllowMediaVideoCapture:
            return m_defaults[poAllowMediaVideoCapture];
        case poAllowMediaAudioVideoCapture:
            return m_defaults[poAllowMediaAudioVideoCapture];
        case poAllowMouseLock:
            return m_defaults[poAllowMouseLock];
        case poAllowDesktopVideoCapture:
            return m_defaults[poAllowDesktopVideoCapture];
        case poAllowDesktopAudioVideoCapture:
            return m_defaults[poAllowDesktopAudioVideoCapture];

        // so far not implemented
        case poZoomLevel:
        case poAllowCookies:
        default:
            qWarning() << "Unknown option" << option;
            return Deny;
    }
}

SiteSettingsManager::Permission SiteSettingsManager::getDefaultPermission(const QWebEnginePage::Feature& feature)
{
    auto option = optionFromWebEngineFeature(feature);
    return getDefaultPermission(option);
}

void SiteSettingsManager::setDefaultPermission(const SiteSettingsManager::PageOptions& option, const int& value)
{
    switch (option) {
        case poAllowNotifications:
        case poAllowGeolocation:
        case poAllowMediaAudioCapture:
        case poAllowMediaVideoCapture:
        case poAllowMediaAudioVideoCapture:
        case poAllowMouseLock:
        case poAllowDesktopVideoCapture:
        case poAllowDesktopAudioVideoCapture:
            setDefaultPermission(option, intToPermission(value));
            break;

        case poZoomLevel:
        case poAllowCookies:
        case poJavascriptEnabled:
        case poAutoloadImages:
        default:
            qWarning() << "Unknown option" << option;
            break;
    }
}

void SiteSettingsManager::setDefaultPermission(const SiteSettingsManager::PageOptions& option, const SiteSettingsManager::Permission& permission)
{
    switch (option) {
        case poZoomLevel:
        case poAllowCookies:
        case poJavascriptEnabled:
        case poAutoloadImages:
            qWarning() << "So far not implemented" << option;
            break;

        case poAllowNotifications:
        case poAllowGeolocation:
        case poAllowMediaAudioCapture:
        case poAllowMediaVideoCapture:
        case poAllowMediaAudioVideoCapture:
        case poAllowMouseLock:
        case poAllowDesktopVideoCapture:
        case poAllowDesktopAudioVideoCapture:
            m_defaults[option] = permission;
            break;
        default:
            qWarning() << "Unknown option" << option;
            break;
    }
}

void SiteSettingsManager::setDefaultPermission(const QWebEnginePage::Feature& feature, const SiteSettingsManager::Permission& value)
{
    auto option = optionFromWebEngineFeature(feature);
    setDefaultPermission(option, value);
}

SiteSettingsManager::PageOptions SiteSettingsManager::optionFromWebEngineFeature(const QWebEnginePage::Feature& feature) const
{
    switch (feature) {
        case QWebEnginePage::Notifications:
            return poAllowNotifications;
        case QWebEnginePage::Geolocation:
            return poAllowGeolocation;
        case QWebEnginePage::MediaAudioCapture:
            return poAllowMediaAudioCapture;
        case QWebEnginePage::MediaVideoCapture:
            return poAllowMediaVideoCapture;
        case QWebEnginePage::MediaAudioVideoCapture:
            return poAllowMediaAudioVideoCapture;
        case QWebEnginePage::MouseLock:
            return poAllowMouseLock;
        case QWebEnginePage::DesktopVideoCapture:
            return poAllowDesktopVideoCapture;
        case QWebEnginePage::DesktopAudioVideoCapture:
            return poAllowDesktopAudioVideoCapture;
        default:
            qWarning() << "Unknown feature" << feature;
            return poAllowNotifications;
    }
}

QString SiteSettingsManager::sqlColumnFromWebEngineFeature(const QWebEnginePage::Feature& feature)
{
    return optionToSqlColumn(optionFromWebEngineFeature(feature));
}

SiteSettingsManager::Permission SiteSettingsManager::testAttribute(const QWebEngineSettings::WebAttribute attribute) const
{
    if (mApp->webSettings()->testAttribute(attribute)) {
        return Allow;
    }
    else {
        return Deny;
    }
}

SiteSettingsManager::Permission SiteSettingsManager::intToPermission(const int permission) const
{
    switch (permission) {
        case Allow:
            return Allow;
        case Deny:
            return Deny;
        case Ask:
            return Ask;
        default:
            return Default;
    }
}

QString SiteSettingsManager::getOptionName(const SiteSettingsManager::PageOptions& option)
{
    switch (option) {
        case poJavascriptEnabled:
            return QSL("JavaScript");
        case poAutoloadImages:
            return QSL("Autoload Images");
        case poZoomLevel:
            return QSL("Zoom level");
        case poAllowCookies:
            return QSL("Cookies");
        case poAllowNotifications:
            return QSL("Notifications");
        case poAllowGeolocation:
            return QSL("Location");
        case poAllowMediaAudioCapture:
            return QSL("Microphone");
        case poAllowMediaVideoCapture:
            return QSL("Camera");
        case poAllowMediaAudioVideoCapture:
            return QSL("Microphone and Camera");
        case poAllowMouseLock:
            return QSL("Hide mouse pointer");
        case poAllowDesktopVideoCapture:
            return QSL("Screen capture");
        case poAllowDesktopAudioVideoCapture:
            return QSL("Screen capture with audio");
        default:
            qWarning() << "Unknown option" << option;
            return QSL("Unknown");;
    }
}

QString SiteSettingsManager::getOptionName(const QWebEnginePage::Feature& feature)
{
    return getOptionName(optionFromWebEngineFeature(feature));
}

QString SiteSettingsManager::getPermissionName(const SiteSettingsManager::Permission permission)
{
    switch (permission) {
        case Allow:
            return QSL("Allow");
        case Ask:
            return QSL("Ask");
        case Deny:
            return QSL("Deny");
        case Default:
            return QSL("Default");
        default:
            qWarning() << "Uknown permission" << permission;
            return QSL("Unknown");
    }
}

QWebEngineSettings::WebAttribute SiteSettingsManager::optionToAttribute(const SiteSettingsManager::PageOptions& option) const
{
    switch (option) {
        case poAutoloadImages:
            return QWebEngineSettings::AutoLoadImages;

        case poJavascriptEnabled:
            return QWebEngineSettings::JavascriptEnabled;
        case poJavascriptCanOpenWindows:
            return QWebEngineSettings::JavascriptCanOpenWindows;
        case poJavascriptCanAccessClipboard:
            return QWebEngineSettings::JavascriptCanAccessClipboard;
        case poJavascriptCanPaste:
            return QWebEngineSettings::JavascriptCanPaste;
        case poAllowWindowActivationFromJavaScript:
            return QWebEngineSettings::AllowWindowActivationFromJavaScript;

        case poLocalStorageEnabled:
            return QWebEngineSettings::LocalStorageEnabled;
        case poScrollAnimatorEnabled:
            return QWebEngineSettings::ScrollAnimatorEnabled;
        case poFullScreenSupportEnabled:
            return QWebEngineSettings::FullScreenSupportEnabled;
        case poAllowRunningInsecureContent:
            return QWebEngineSettings::AllowRunningInsecureContent;
        case poAllowGeolocationOnInsecureOrigins:
            return QWebEngineSettings::AllowGeolocationOnInsecureOrigins;
        case poPlaybackRequiresUserGesture:
            return QWebEngineSettings::PlaybackRequiresUserGesture;
        case poWebRTCPublicInterfacesOnly:
            return QWebEngineSettings::WebRTCPublicInterfacesOnly;
        default:
            return QWebEngineSettings::JavascriptEnabled;
    }
}

QString SiteSettingsManager::webAttributeToSqlColumn(const QWebEngineSettings::WebAttribute& attribute)
{
    switch (attribute) {
        case QWebEngineSettings::AutoLoadImages:
            return QSL("wa_autoload_images");

        case QWebEngineSettings::JavascriptEnabled:
            return QSL("wa_js_enabled");
        case QWebEngineSettings::JavascriptCanOpenWindows:
            return QSL("wa_js_open_windows");
        case QWebEngineSettings::JavascriptCanAccessClipboard:
            return QSL("wa_js_access_clipboard");
        case QWebEngineSettings::JavascriptCanPaste:
            return QSL("wa_js_can_paste");
        case QWebEngineSettings::AllowWindowActivationFromJavaScript:
            return QSL("wa_js_window_activation");

        case QWebEngineSettings::LocalStorageEnabled:
            return QSL("wa_local_storage");
        case QWebEngineSettings::ScrollAnimatorEnabled:
            return QSL("wa_acroll_animator");
        case QWebEngineSettings::FullScreenSupportEnabled:
            return QSL("wa_fullscreen_support");
        case QWebEngineSettings::AllowRunningInsecureContent:
            return QSL("wa_run_insecure_content");
        case QWebEngineSettings::AllowGeolocationOnInsecureOrigins:
            return QSL("wa_insecure_geolocation");
        case QWebEngineSettings::PlaybackRequiresUserGesture:
            return QSL("wa_playback_needs_gesture");
        case QWebEngineSettings::WebRTCPublicInterfacesOnly:
            return QSL("wa_webrtc_public_interface_only");

        default:
            qWarning() << "Unknown attribute: " << attribute;
            return QSL("wa_js_enabled");
    }
}
