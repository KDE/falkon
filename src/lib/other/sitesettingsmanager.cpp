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
//     m_defaults[poAllowJavascript]               = settings.value("allowJavascript", Default).toInt();
//     m_defaults[poAllowImages]                   = settings.value("allowImages", Default).toInt();
//     m_defaults[poAllowCookies]                  = settings.value("allowCookies", Default).toInt();
//     m_defaults[poZoomLevel]                     = settings.value("defaultZoomLevel", Default).toInt(); // fail
    m_defaults[poAllowNotifications]            = intToPermission(settings.value("allowNotifications",              Default).toInt());
    m_defaults[poAllowGeolocation]              = intToPermission(settings.value("allowGealocation",                Default).toInt());
    m_defaults[poAllowMediaAudioCapture]        = intToPermission(settings.value("allowMicrophone",                 Default).toInt());
    m_defaults[poAllowMediaVideoCapture]        = intToPermission(settings.value("allowCamera",                     Default).toInt());
    m_defaults[poAllowMediaAudioVideoCapture]   = intToPermission(settings.value("allowCameraAndMicrophone",        Default).toInt());
    m_defaults[poAllowMouseLock]                = intToPermission(settings.value("allowMouseLock",                  Default).toInt());
    m_defaults[poAllowDesktopVideoCapture]      = intToPermission(settings.value("allowDesktopVideoCapture",        Default).toInt());
    m_defaults[poAllowDesktopAudioVideoCapture] = intToPermission(settings.value("allowDesktopAudioVideoCapture",   Default).toInt());
    settings.endGroup();
}

void SiteSettingsManager::saveSettings()
{
    Settings settings;
    settings.beginGroup("Site-Settings");
/* These are handled by already existing parts of Falkon */
//     settings.setValue("allowJavascript", m_defaults[poAllowJavascript]);
//     settings.setValue("allowIbutmages", m_defaults[poAllowImages]);
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

SiteWebEngineSettings SiteSettingsManager::getWebEngineSettings(const QUrl& url)
{
    SiteWebEngineSettings settings;

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(QSL("SELECT allow_images, allow_javascript FROM site_settings WHERE server=?"));
    query.addBindValue(url.host());
    query.exec();

    if (query.next()) {
        int allow_images = query.value(QSL("allow_images")).toInt();
        int allow_javascript = query.value(QSL("allow_javascript")).toInt();

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

void SiteSettingsManager::setJavascript(const QUrl& url, const int value)
{
    setOption(poAllowJavascript, url, value);
}

void SiteSettingsManager::setImages(const QUrl& url, const int value)
{
    setOption(poAllowImages, url, value);
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
        case poAllowJavascript:
            return QSL("allow_javascript");

        case poAllowImages:
            return QSL("allow_images");

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
        case poAllowJavascript:
            return testAttribute(QWebEngineSettings::JavascriptEnabled);

        case poAllowImages:
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
        case poAllowJavascript:
        case poAllowImages:
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
        case poAllowJavascript:
        case poAllowImages:
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

bool SiteSettingsManager::getDefaultOptionValue(const SiteSettingsManager::PageOptions& option)
{
    switch (option) {
        case poAllowJavascript:
            return mApp->webSettings()->testAttribute(QWebEngineSettings::JavascriptEnabled);

        case poAllowImages:
            return mApp->webSettings()->testAttribute(QWebEngineSettings::AutoLoadImages);

        /* At the moment, no idea */
        case poAllowCookies:
            return false;

        case poZoomLevel:
            return false;

        default:
            qWarning() << "Unknown option" << option;
            return false;
    }
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
        case poAllowJavascript:
            return QSL("JavaScript");
        case poAllowImages:
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
