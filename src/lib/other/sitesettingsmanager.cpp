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
    settings.beginGroup("Web-Browser-Settings");
//    m_isSaving = settings.value("allowPerDomainZoom", true).toBool();
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

        switch (allow_option) {
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
            Q_UNREACHABLE();
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

        default:
            Q_UNREACHABLE();
            qWarning() << "Unknown option" << option;
            return Deny;
    }
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
            Q_UNREACHABLE();
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
            Q_UNREACHABLE();
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
