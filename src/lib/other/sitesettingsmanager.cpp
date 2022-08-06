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
#include <QtWebEngineCoreVersion>


const QList<QWebEngineSettings::WebAttribute> supportedAttribute = {
    QWebEngineSettings::AutoLoadImages
    ,QWebEngineSettings::JavascriptEnabled
    ,QWebEngineSettings::JavascriptCanOpenWindows
    ,QWebEngineSettings::JavascriptCanAccessClipboard
    ,QWebEngineSettings::JavascriptCanPaste
    ,QWebEngineSettings::AllowWindowActivationFromJavaScript
    ,QWebEngineSettings::LocalStorageEnabled
    ,QWebEngineSettings::FullScreenSupportEnabled
    ,QWebEngineSettings::AllowRunningInsecureContent
    ,QWebEngineSettings::PlaybackRequiresUserGesture
#if QTWEBENGINECORE_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    ,QWebEngineSettings::ReadingFromCanvasEnabled
#endif
#if QTWEBENGINECORE_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    ,QWebEngineSettings::ForceDarkMode
#endif
};
const QList<QWebEnginePage::Feature> supportedFeatures = {
    QWebEnginePage::Notifications
    ,QWebEnginePage::Geolocation
    ,QWebEnginePage::MediaAudioCapture
    ,QWebEnginePage::MediaVideoCapture
    ,QWebEnginePage::MediaAudioVideoCapture
    ,QWebEnginePage::MouseLock
    ,QWebEnginePage::DesktopVideoCapture
    ,QWebEnginePage::DesktopAudioVideoCapture
};


SiteSettingsManager::SiteSettingsManager ( QObject* parent )
: QObject(parent)
{
    prepareSqls();
    loadSettings();
}

SiteSettingsManager::~SiteSettingsManager() = default;

void SiteSettingsManager::loadSettings()
{
    Settings settings;

    settings.beginGroup(QSL("Site-Settings"));
    /* HTML5 Feature */
    for (const auto &feature : std::as_const(supportedFeatures)) {
        defaultFeatures[feature] = intToPermission(settings.value(featureToSqlColumn(feature), Ask).toInt());
    }
    settings.endGroup();
}

void SiteSettingsManager::saveSettings()
{
    Settings settings;
    settings.beginGroup(QSL("Site-Settings"));
    for (auto it = defaultFeatures.begin(); it != defaultFeatures.end(); ++it) {
        settings.setValue(featureToSqlColumn(it.key()), it.value());
    }
    settings.endGroup();
}

QHash<QWebEngineSettings::WebAttribute, bool> SiteSettingsManager::getWebAttributes(const QUrl& url)
{
    QHash<QWebEngineSettings::WebAttribute, bool> attributes;
    QString host = url.host();

    if (host.isEmpty()) {
        for (int i = 0; i < supportedAttribute.size(); ++i) {
            QWebEngineSettings::WebAttribute attribute = supportedAttribute[i];
            attributes[attribute] = mApp->webSettings()->testAttribute(attribute);
        }

        return attributes;
    }

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(attributesSql);
    query.addBindValue(host);
    query.exec();

    if (query.next()) {
        for (int i = 0; i < query.record().count(); ++i) {
            SiteSettingsManager::Permission perm = intToPermission(query.value(i).toInt());
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

void SiteSettingsManager::setOption(const QString& column, const QUrl& url, const int value)
{
    QString host = url.host();

    if (column.isEmpty() || host.isEmpty()) {
        return;
    }

    auto job = new SqlQueryJob(QSL("UPDATE %2 SET %1=? WHERE server=?").arg(column, sqlTable()), this);
    job->addBindValue(value);
    job->addBindValue(host);
    connect(job, &SqlQueryJob::finished, this, [=]() {
        if (job->numRowsAffected() == 0) {
            auto job = new SqlQueryJob(QSL("INSERT INTO %2 (server, %1) VALUES (?,?)").arg(column, sqlTable()), this);
            job->addBindValue(host);
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

void SiteSettingsManager::setOption(const QWebEnginePage::Feature& feature, const QUrl& url, const SiteSettingsManager::Permission value)
{
    setOption(featureToSqlColumn(feature), url, value);
}

void SiteSettingsManager::setOption(const QWebEngineSettings::WebAttribute& attribute, const QUrl& url, const SiteSettingsManager::Permission value)
{
    setOption(webAttributeToSqlColumn(attribute), url, value);
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QString& column, const QString& host)
{
    if (column.isEmpty()) {
        return Deny;
    }
    if (host.isEmpty()) {
        return Default;
    }

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(QSL("SELECT %1 FROM %2 WHERE server=?").arg(column, sqlTable()));
    query.addBindValue(host);
    query.exec();

    if (query.next()) {
        int allow_option = query.value(column).toInt();

        return intToPermission(allow_option);
    }

    return Default;
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const SiteSettingsManager::PageOptions option, const QString& host)
{
    return getPermission(optionToSqlColumn(option), host);
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QWebEnginePage::Feature feature, const QString& host)
{
    return getPermission(featureToSqlColumn(feature), host);
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QWebEngineSettings::WebAttribute attribute, const QString& host)
{
    return getPermission(webAttributeToSqlColumn(attribute), host);
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QString &column, const QUrl& url)
{
    return getPermission(column, url.host());
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const SiteSettingsManager::PageOptions option, const QUrl& url)
{
    return getPermission(optionToSqlColumn(option), url.host());
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QWebEnginePage::Feature feature, const QUrl& url)
{
    return getPermission(featureToSqlColumn(feature), url.host());
}

SiteSettingsManager::Permission SiteSettingsManager::getPermission(const QWebEngineSettings::WebAttribute attribute, const QUrl& url)
{
    return getPermission(webAttributeToSqlColumn(attribute), url.host());
}

SiteSettingsManager::Permission SiteSettingsManager::getDefaultPermission(const SiteSettingsManager::PageOptions option)
{
    switch (option) {
        case poAllowCookies: {
            Settings settings;
            settings.beginGroup(QSL("Cookie-Settings"));
            auto defaultCookies = settings.value(QSL("allowCookies"), true).toBool() ? Allow : Deny;
            settings.endGroup();

            return defaultCookies;
        }
        // so far not implemented
        case poZoomLevel:
        default:
            qWarning() << "Unknown option:" << option;
            return Deny;
    }
}

SiteSettingsManager::Permission SiteSettingsManager::getDefaultPermission(const QWebEnginePage::Feature feature) const
{
    if (!supportedFeatures.contains(feature)) {
        qWarning() << "Unknown feature:" << feature;
        return Deny;
    }

    return defaultFeatures[feature];
}

SiteSettingsManager::Permission SiteSettingsManager::getDefaultPermission(const QWebEngineSettings::WebAttribute attribute) const
{
    if (!supportedAttribute.contains(attribute)) {
        qWarning() << "Unknown attribute:" << attribute;
        return Deny;
    }

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

QString SiteSettingsManager::getOptionName(const SiteSettingsManager::PageOptions option) const
{
    switch (option) {
        case poZoomLevel:
            return tr("Zoom level");
        case poAllowCookies:
            return tr("Cookies");
        default:
            qWarning() << "Unknown option:" << option;
            return tr("Unknown");;
    }
}

QString SiteSettingsManager::getOptionName(const QWebEnginePage::Feature feature) const
{
    switch (feature) {
        case QWebEnginePage::Notifications:
            return tr("Notifications");
        case QWebEnginePage::Geolocation:
            return tr("Geolocation");
        case QWebEnginePage::MediaAudioCapture:
            return tr("Microphone");
        case QWebEnginePage::MediaVideoCapture:
            return tr("Camera");
        case QWebEnginePage::MediaAudioVideoCapture:
            return tr("Microphone and Camera");
        case QWebEnginePage::MouseLock:
            return tr("Hide mouse pointer");
        case QWebEnginePage::DesktopVideoCapture:
            return tr("Screen capture");
        case QWebEnginePage::DesktopAudioVideoCapture:
            return tr("Screen capture with audio");
        default:
            qWarning() << "Unknown feature:" << feature;
            return tr("Unknown");
    }
}

QString SiteSettingsManager::getOptionName(const QWebEngineSettings::WebAttribute attribute) const
{
    switch (attribute) {
        case QWebEngineSettings::AutoLoadImages:
            return tr("Autoload images");

        case QWebEngineSettings::JavascriptEnabled:
            return tr("Enable JavaScript");
        case QWebEngineSettings::JavascriptCanOpenWindows:
            return tr("JavaScript: Open popup windows");
        case QWebEngineSettings::JavascriptCanAccessClipboard:
            return tr("JavaScript: Access clipboard");
        case QWebEngineSettings::JavascriptCanPaste:
            return tr("JavaScript: Paste from clipboard");
        case QWebEngineSettings::AllowWindowActivationFromJavaScript:
            return tr("JavaScript: Activate windows");

        case QWebEngineSettings::LocalStorageEnabled:
            return tr("Local storage");
        case QWebEngineSettings::FullScreenSupportEnabled:
            return tr("FullScreen support");
        case QWebEngineSettings::AllowRunningInsecureContent:
            return tr("Run insecure content");
        case QWebEngineSettings::PlaybackRequiresUserGesture:
            return tr("Automatic playing of videos");
#if QTWEBENGINECORE_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        case QWebEngineSettings::ReadingFromCanvasEnabled:
            return tr("Allow reading from canvas");
#endif
#if QTWEBENGINECORE_VERSION >= QT_VERSION_CHECK(6, 7, 0)
        case QWebEngineSettings::ForceDarkMode:
            return tr("Force dark mode");
#endif

        default:
            qWarning() << "Unknown attribute:" << attribute;
            return tr("Unknown");
    }
}

QString SiteSettingsManager::getPermissionName(const SiteSettingsManager::Permission permission) const
{
    switch (permission) {
        case Allow:
            return tr("Allow");
        case Ask:
            return tr("Ask");
        case Deny:
            return tr("Deny");
        case Default:
            return tr("Default");
        default:
            qWarning() << "Uknown permission:" << permission;
            return tr("Unknown");
    }
}

QString SiteSettingsManager::optionToSqlColumn(const SiteSettingsManager::PageOptions option) const
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

QString SiteSettingsManager::featureToSqlColumn(const QWebEnginePage::Feature feature) const
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

QString SiteSettingsManager::webAttributeToSqlColumn(const QWebEngineSettings::WebAttribute attribute) const
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
        case QWebEngineSettings::FullScreenSupportEnabled:
            return QSL("wa_fullscreen_support");
        case QWebEngineSettings::AllowRunningInsecureContent:
            return QSL("wa_run_insecure_content");
        case QWebEngineSettings::PlaybackRequiresUserGesture:
            return QSL("wa_playback_needs_gesture");
#if QTWEBENGINECORE_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        case QWebEngineSettings::ReadingFromCanvasEnabled:
            return QSL("wa_reading_from_canvas");
#endif
#if QTWEBENGINECORE_VERSION >= QT_VERSION_CHECK(6, 7, 0)
        case QWebEngineSettings::ForceDarkMode:
            return QSL("wa_force_dark_mode");
#endif

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
    siteSettings.server = url.host();

    if (url.isEmpty()) {
        return siteSettings;
    }

    int index = 0;

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(everythingSql.arg(sqlTable()));
    query.addBindValue(url.host());
    query.exec();

    if (query.next()) {
        Permission perm;
        for (int i = 0; i < supportedAttribute.size(); ++i, ++index) {
            perm = intToPermission(query.value(index).toInt());
            siteSettings.attributes[supportedAttribute[i]] = perm;
        }
        for (int i = 0; i < supportedFeatures.size(); ++i, ++index) {
            perm = intToPermission(query.value(index).toInt());
            siteSettings.features[supportedFeatures[i]] = perm;
        }
        siteSettings.AllowCookies = intToPermission(query.value(index++).toInt());
        siteSettings.ZoomLevel = query.value(index++).toInt();
    }

    return siteSettings;
}

void SiteSettingsManager::setSiteSettings(SiteSettingsManager::SiteSettings& siteSettings)
{
    if (siteSettings.server.isEmpty()) {
        return;
    }

    auto job = new SqlQueryJob(everythingUpdateSql.arg(sqlTable()), this);

    for (int i = 0; i < supportedAttribute.size(); ++i) {
        job->addBindValue(siteSettings.attributes[supportedAttribute[i]]);
    }
    for (int i = 0; i < supportedFeatures.size(); ++i) {
        job->addBindValue(siteSettings.features[supportedFeatures[i]]);
    }
    job->addBindValue(siteSettings.AllowCookies);
    job->addBindValue(siteSettings.ZoomLevel);
    job->addBindValue(siteSettings.server);

    connect(job, &SqlQueryJob::finished, this, [=]() {
        if (job->numRowsAffected() == 0) {
            auto job = new SqlQueryJob(everythingInsertSql.arg(sqlTable()), this);

            for (int i = 0; i < supportedAttribute.size(); ++i) {
                job->addBindValue(siteSettings.attributes[supportedAttribute[i]]);
            }
            for (int i = 0; i < supportedFeatures.size(); ++i) {
                job->addBindValue(siteSettings.features[supportedFeatures[i]]);
            }
            job->addBindValue(siteSettings.AllowCookies);
            job->addBindValue(siteSettings.ZoomLevel);
            job->addBindValue(siteSettings.server);

            job->start();
        }
    });
    job->start();
}

QString SiteSettingsManager::sqlTable()
{
    return QSL("site_settings");
}

void SiteSettingsManager::prepareSqls() {
    /* Select SQL for QtWE Attributes */
    attributesSql = QSL("SELECT ");

    for (int i = 0; i < supportedAttribute.size(); ++i) {
        if (i > 0) {
            attributesSql.append(QSL(", "));
        }
        attributesSql.append(webAttributeToSqlColumn(supportedAttribute[i]));
    }

    attributesSql.append(QSL(" FROM %1 WHERE server=?").arg(sqlTable()));


    /* Select SQL for SiteSettings */
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

    everythingSql.append(QSL(" FROM %1 WHERE server=?"));


    /* Insert SQL for SiteSettings */
    everythingInsertSql = QSL("INSERT INTO %1 (");
    for (int i = 0; i < supportedAttribute.size(); ++i) {
        everythingInsertSql.append(webAttributeToSqlColumn(supportedAttribute[i]));
        everythingInsertSql.append(QSL(", "));
    }
    for (int i = 0; i < supportedFeatures.size(); ++i) {
        everythingInsertSql.append(featureToSqlColumn(supportedFeatures[i]));
        everythingInsertSql.append(QSL(", "));
    }

    everythingInsertSql.append(optionToSqlColumn(poAllowCookies));
    everythingInsertSql.append(QSL(", "));

    everythingInsertSql.append(optionToSqlColumn(poZoomLevel));

    everythingInsertSql.append(QSL(", server"));

    everythingInsertSql.append(QSL(") Values ("));
    /* Index = sum(server, numberOfAttributes, numberOfFeatures, cookies, zoom) */
    int index = 1 + supportedAttribute.size() + supportedFeatures.size() + 2;
    for (int i = 0; i < index; ++i) {
        if (i > 0) {
            everythingInsertSql.append(QSL(", "));
        }
        everythingInsertSql.append(QSL("?"));
    }
    everythingInsertSql.append(QSL(")"));


    /* Update SQL for SiteSettings */
    everythingUpdateSql = QSL("UPDATE %1 SET ");
    for (int i = 0; i < supportedAttribute.size(); ++i) {
        everythingUpdateSql.append(webAttributeToSqlColumn(supportedAttribute[i]));
        everythingUpdateSql.append(QSL("=?, "));
    }
    for (int i = 0; i < supportedFeatures.size(); ++i) {
        everythingUpdateSql.append(featureToSqlColumn(supportedFeatures[i]));
        everythingUpdateSql.append(QSL("=?, "));
    }

    everythingUpdateSql.append(optionToSqlColumn(poAllowCookies));
    everythingUpdateSql.append(QSL("=?, "));

    everythingUpdateSql.append(optionToSqlColumn(poZoomLevel));
    everythingUpdateSql.append(QSL("=? "));

    everythingUpdateSql.append(QSL(" WHERE server=?"));
}