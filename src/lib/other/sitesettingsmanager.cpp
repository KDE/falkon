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


    supportedFeatures.append(QWebEnginePage::Notifications);
    supportedFeatures.append(QWebEnginePage::Geolocation);
    supportedFeatures.append(QWebEnginePage::MediaAudioCapture);
    supportedFeatures.append(QWebEnginePage::MediaVideoCapture);
    supportedFeatures.append(QWebEnginePage::MediaAudioVideoCapture);
    supportedFeatures.append(QWebEnginePage::MouseLock);
    supportedFeatures.append(QWebEnginePage::DesktopVideoCapture);
    supportedFeatures.append(QWebEnginePage::DesktopAudioVideoCapture);


    everythingSql = QSL("SELECT ");

    for (int i = 0; i < supportedAttribute.size(); ++i) {
        if (i > 0) {
            everythingSql.append(QSL(", "));
        }
        everythingSql.append(webAttributeToSqlColumn(supportedAttribute[i]));
    }

    for (int i = 0; i < supportedFeatures.size(); ++i) {
        everythingSql.append(QSL(", "));
        everythingSql.append(featureToSqlColumn(supportedFeatures[i]));
    }

    everythingSql.append(QSL(", "));
    everythingSql.append(optionToSqlColumn(poAllowCookies));

    everythingSql.append(QSL(", "));
    everythingSql.append(optionToSqlColumn(poZoomLevel));

    everythingSql.append(QSL(" FROM site_settings WHERE server=?"));
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
    settings.endGroup();


    settings.beginGroup("Site-Settings-Default-Features");
    for (const auto &feature : qAsConst(supportedFeatures)) {
        defaultFeatures[feature] = intToPermission(settings.value(featureToSqlColumn(feature), Ask).toInt());
    }
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
    settings.endGroup();


    settings.beginGroup("Site-Settings-Default-Features");
    for (auto it = defaultFeatures.begin(); it != defaultFeatures.end(); ++it) {
        settings.setValue(featureToSqlColumn(it.key()), it.value());
    }
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
    setOption(QWebEngineSettings::JavascriptEnabled, url, intToPermission(value));
}

void SiteSettingsManager::setImages(const QUrl& url, const int value)
{
    setOption(QWebEngineSettings::AutoLoadImages, url, intToPermission(value));
}

void SiteSettingsManager::setOption(const QString& column, const QUrl& url, const int value)
{
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

void SiteSettingsManager::setOption(const PageOptions option, const QUrl& url, const int value)
{
    setOption(optionToSqlColumn(option), url, value);
}

void SiteSettingsManager::setOption(const QWebEnginePage::Feature& feature, const QUrl& url, const Permission &value)
{
    setOption(featureToSqlColumn(feature), url, value);
}

void SiteSettingsManager::setOption(const QWebEngineSettings::WebAttribute& attribute, const QUrl& url, const SiteSettingsManager::Permission& value)
{
    setOption(webAttributeToSqlColumn(attribute), url, value);
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QString &column, const QUrl& url)
{
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

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const SiteSettingsManager::PageOptions option, const QUrl& url)
{
    return getPermission(optionToSqlColumn(option), url);
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QWebEnginePage::Feature& feature, const QUrl& url)
{
    return getPermission(featureToSqlColumn(feature), url);
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QWebEngineSettings::WebAttribute& attribute, const QUrl& url)
{
    return getPermission(webAttributeToSqlColumn(attribute), url);
}

QString SiteSettingsManager::optionToSqlColumn(const SiteSettingsManager::PageOptions &option)
{
    switch (option) {
        case poAllowCookies:
            return QSL("allow_cookies");
        case poZoomLevel:
            return QSL("zoom_level");
        default:
            qWarning() << "Unknown option:" << option;
            return QLatin1String("");
    }
}

SiteSettingsManager::Permission SiteSettingsManager::getDefaultPermission(const SiteSettingsManager::PageOptions& option)
{
    switch (option) {
        // so far not implemented
        case poZoomLevel:
        case poAllowCookies:
        default:
            qWarning() << "Unknown option:" << option;
            return Deny;
    }
}

SiteSettingsManager::Permission SiteSettingsManager::getDefaultPermission(const QWebEnginePage::Feature& feature)
{
    if (!supportedFeatures.contains(feature)) {
        qWarning() << "Unknown feature:" << feature;
        return Deny;
    }

    return defaultFeatures[feature];
}

SiteSettingsManager::Permission SiteSettingsManager::getDefaultPermission(const QWebEngineSettings::WebAttribute& attribute)
{
    if (!supportedAttribute.contains(attribute)) {
        qWarning() << "Unknown attribute:" << attribute;
        return Deny;
    }

    return testAttribute(attribute);
}

void SiteSettingsManager::setDefaultPermission(const SiteSettingsManager::PageOptions& option, const int& value)
{
    switch (option) {
        case poZoomLevel:
        case poAllowCookies:
        default:
            qWarning() << "Unknown option:" << option;
            break;
    }
}

void SiteSettingsManager::setDefaultPermission(const SiteSettingsManager::PageOptions& option, const SiteSettingsManager::Permission& permission)
{
    switch (option) {
        case poZoomLevel:
        case poAllowCookies:
            qWarning() << "So far not implemented:" << option;
            break;

        default:
            qWarning() << "Unknown option:" << option;
            break;
    }
}

void SiteSettingsManager::setDefaultPermission(const QWebEnginePage::Feature& feature, const SiteSettingsManager::Permission& value)
{
    if (!supportedFeatures.contains(feature)) {
        qWarning() << "Unknown feature:" << feature;
        return;
    }

    defaultFeatures[feature] = value;
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
        case poZoomLevel:
            return QSL("Zoom level");
        case poAllowCookies:
            return QSL("Cookies");
        default:
            qWarning() << "Unknown option:" << option;
            return QSL("Unknown");;
    }
}

QString SiteSettingsManager::getOptionName(const QWebEnginePage::Feature& feature)
{
    switch (feature) {
        case QWebEnginePage::Notifications:
            return QSL("Notifications");
        case QWebEnginePage::Geolocation:
            return QSL("Geolocation");
        case QWebEnginePage::MediaAudioCapture:
            return QSL("Microphone");
        case QWebEnginePage::MediaVideoCapture:
            return QSL("Camera");
        case QWebEnginePage::MediaAudioVideoCapture:
            return QSL("Microphone and Camera");
        case QWebEnginePage::MouseLock:
            return QSL("Hide mouse pointer");
        case QWebEnginePage::DesktopVideoCapture:
            return QSL("Screen capture");
        case QWebEnginePage::DesktopAudioVideoCapture:
            return QSL("Screen capture with audio");
        default:
            qWarning() << "Unknown feature:" << feature;
            return QSL("Unknown");
    }
}

QString SiteSettingsManager::getOptionName(const QWebEngineSettings::WebAttribute attribute)
{
    switch (attribute) {
        case QWebEngineSettings::AutoLoadImages:
            return QSL("Autoload images");

        case QWebEngineSettings::JavascriptEnabled:
            return QSL("Enable JavaScript");
        case QWebEngineSettings::JavascriptCanOpenWindows:
            return QSL("JavaScript: Open popup windows");
        case QWebEngineSettings::JavascriptCanAccessClipboard:
            return QSL("JavaScript: Access clipboard");
        case QWebEngineSettings::JavascriptCanPaste:
            return QSL("JavaScript: Paste from clipboard");
        case QWebEngineSettings::AllowWindowActivationFromJavaScript:
            return QSL("JavaScript: Activate windows");

        case QWebEngineSettings::LocalStorageEnabled:
            return QSL("Local storage");
        case QWebEngineSettings::ScrollAnimatorEnabled:
            return QSL("Animated scrolling");
        case QWebEngineSettings::FullScreenSupportEnabled:
            return QSL("FullScreen support");
        case QWebEngineSettings::AllowRunningInsecureContent:
            return QSL("Run insecure content");
        case QWebEngineSettings::AllowGeolocationOnInsecureOrigins:
            return QSL("Geolocation on insecure origin");
        case QWebEngineSettings::PlaybackRequiresUserGesture:
            return QSL("Automatic playing of videos");
        case QWebEngineSettings::WebRTCPublicInterfacesOnly:
            return QSL("Prevent WebRTC from leaking private IP address");

        default:
            qWarning() << "Unknown attribute:" << attribute;
            return QSL("Unknown");
    }
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
            qWarning() << "Uknown permission:" << permission;
            return QSL("Unknown");
    }
}

QString SiteSettingsManager::featureToSqlColumn(const QWebEnginePage::Feature& feature)
{
    switch (feature) {
        case QWebEnginePage::Notifications:
            return QSL("f_notifications");
        case QWebEnginePage::Geolocation:
            return QSL("f_geolocation");
        case QWebEnginePage::MediaAudioCapture:
            return QSL("f_media_audio_capture");
        case QWebEnginePage::MediaVideoCapture:
            return QSL("f_media_video_capture");
        case QWebEnginePage::MediaAudioVideoCapture:
            return QSL("f_media_audio_video_capture");
        case QWebEnginePage::MouseLock:
            return QSL("f_mouse_lock");
        case QWebEnginePage::DesktopVideoCapture:
            return QSL("f_desktop_video_capture");
        case QWebEnginePage::DesktopAudioVideoCapture:
            return QSL("f_desktop_audio_video_capture");
        default:
            qWarning() << "Unknown feature:" << feature;
            return QSL("f_notifications");
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
            qWarning() << "Unknown attribute:" << attribute;
            return QSL("wa_js_enabled");
    }
}

QList<QWebEngineSettings::WebAttribute> SiteSettingsManager::getSupportedAttribute() const
{
    return supportedAttribute;
}

QList<QWebEnginePage::Feature> SiteSettingsManager::getSupportedFeatures() const
{
    return supportedFeatures;
}

SiteSettingsManager::SiteSettings SiteSettingsManager::getSiteSettings(QUrl& url)
{
    SiteSettings siteSettings;
    int i;

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(everythingSql);
    query.addBindValue(url.host());
    query.exec();

    if (query.next()) {
        Permission perm;
        for (i = 0; i < supportedAttribute.size(); ++i) {
            perm = intToPermission(query.value(i).toInt());
            siteSettings.attributes[supportedAttribute[i]] = perm;
        }
        for (i = 0; i < supportedFeatures.size(); ++i) {
            perm = intToPermission(query.value(i + supportedAttribute.size()).toInt());
            siteSettings.features[supportedFeatures[i]] = perm;
        }
        perm = intToPermission(query.value(supportedAttribute.size() + supportedFeatures.size()).toInt());
        siteSettings.AllowCookies = perm;
        siteSettings.ZoomLevel = query.value(supportedAttribute.size() + supportedFeatures.size() + 1).toInt();
    }

    return siteSettings;
}
