/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "profilemanager.h"
#include "mainapplication.h"
#include "datapaths.h"
#include "updater.h"
#include "qztools.h"
#include "sqldatabase.h"
#include "sitesettingsmanager.h"
#include "settings.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QWebEnginePage>

#include <iostream>

ProfileManager::ProfileManager()
= default;

void ProfileManager::initConfigDir()
{
    QDir dir(DataPaths::path(DataPaths::Config));

    if (!dir.exists()) {
        migrateFromQupZilla();
    }

    if (QFileInfo::exists(dir.filePath(QStringLiteral("profiles/profiles.ini")))) {
        return;
    }

    std::cout << "Falkon: Creating new profile directory" << std::endl;

    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }

    dir.mkdir(QStringLiteral("profiles"));
    dir.cd(QStringLiteral("profiles"));

    // $Config/profiles
    QFile(dir.filePath(QStringLiteral("profiles.ini"))).remove();
    QFile(QStringLiteral(":data/profiles.ini")).copy(dir.filePath(QStringLiteral("profiles.ini")));
    QFile(dir.filePath(QStringLiteral("profiles.ini"))).setPermissions(QFile::ReadUser | QFile::WriteUser);

    dir.mkdir(QStringLiteral("default"));
    dir.cd(QStringLiteral("default"));

    // $Config/profiles/default
    QFile(QStringLiteral(":data/bookmarks.json")).copy(dir.filePath(QStringLiteral("bookmarks.json")));
    QFile(dir.filePath(QStringLiteral("bookmarks.json"))).setPermissions(QFile::ReadUser | QFile::WriteUser);

    QFile versionFile(dir.filePath(QStringLiteral("version")));
    versionFile.open(QFile::WriteOnly);
    versionFile.write(Qz::VERSION);
    versionFile.close();
}

void ProfileManager::initCurrentProfile(const QString &profileName)
{
    QString profilePath = DataPaths::path(DataPaths::Profiles) + QLatin1Char('/');

    if (profileName.isEmpty()) {
        profilePath.append(startingProfile());
    }
    else {
        profilePath.append(profileName);
    }

    DataPaths::setCurrentProfilePath(profilePath);

    updateCurrentProfile();
    connectDatabase();
    updateDatabase();
}

int ProfileManager::createProfile(const QString &profileName)
{
    QDir dir(DataPaths::path(DataPaths::Profiles));

    if (QDir(dir.absolutePath() + QLatin1Char('/') + profileName).exists()) {
        return -1;
    }
    if (!dir.mkdir(profileName)) {
        return -2;
    }

    dir.cd(profileName);

    QFile versionFile(dir.filePath(QStringLiteral("version")));
    versionFile.open(QFile::WriteOnly);
    versionFile.write(Qz::VERSION);
    versionFile.close();

    return 0;
}

bool ProfileManager::removeProfile(const QString &profileName)
{
    QDir dir(DataPaths::path(DataPaths::Profiles) + QLatin1Char('/') + profileName);

    if (!dir.exists()) {
        return false;
    }

    QzTools::removeRecursively(dir.absolutePath());
    return true;
}

// static
QString ProfileManager::currentProfile()
{
    QString path = DataPaths::currentProfilePath();
    return path.mid(path.lastIndexOf(QLatin1Char('/')) + 1);
}

// static
QString ProfileManager::startingProfile()
{
    QSettings settings(DataPaths::path(DataPaths::Profiles) + QLatin1String("/profiles.ini"), QSettings::IniFormat);
    return settings.value(QStringLiteral("Profiles/startProfile"), QLatin1String("default")).toString();
}

// static
void ProfileManager::setStartingProfile(const QString &profileName)
{
    QSettings settings(DataPaths::path(DataPaths::Profiles) + QLatin1String("/profiles.ini"), QSettings::IniFormat);
    settings.setValue(QStringLiteral("Profiles/startProfile"), profileName);
}

// static
QStringList ProfileManager::availableProfiles()
{
    QDir dir(DataPaths::path(DataPaths::Profiles));
    return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

void ProfileManager::updateCurrentProfile()
{
    QDir profileDir(DataPaths::currentProfilePath());

    if (!profileDir.exists()) {
        QDir newDir(profileDir.path().remove(profileDir.dirName()));
        newDir.mkdir(profileDir.dirName());
    }

    QFile versionFile(profileDir.filePath(QStringLiteral("version")));

    // If file exists, just update the profile to current version
    if (versionFile.exists()) {
        versionFile.open(QFile::ReadOnly);
        profileVersion = QString::fromUtf8(versionFile.readAll()).trimmed();
        versionFile.close();

        updateProfile(QString::fromLatin1(Qz::VERSION), profileVersion);
    }
    else {
        copyDataToProfile();
    }

    versionFile.open(QFile::WriteOnly);
    versionFile.write(Qz::VERSION);
    versionFile.close();
}

void ProfileManager::updateProfile(const QString &current, const QString &profile)
{
    if (current == profile) {
        return;
    }

    Updater::Version prof(profile);

    // Profile is from newer version than running application
    if (prof > Updater::Version(QString::fromLatin1(Qz::VERSION))) {
        // Only copy data when profile is not from development version
        if (prof.revisionNumber != 99) {
            copyDataToProfile();
        }
        return;
    }

    if (prof < Updater::Version(QStringLiteral("1.9.0"))) {
        std::cout << "Falkon: Using profile from QupZilla " << qPrintable(profile) << " is not supported!" << std::endl;
        return;
    }

    // No change in 2.0
    if (prof < Updater::Version(QStringLiteral("2.9.99"))) {
        return;
    }

    // No change in 3.0
    if (prof < Updater::Version(QStringLiteral("3.0.99"))) {
        return;
    }

    // No change in 3.1
    if (prof < Updater::Version(QStringLiteral("3.1.99"))) {
        return;
    }

    if (prof < Updater::Version(QStringLiteral("25.07.71"))) {
        QSettings settings(DataPaths::currentProfilePath() + QLatin1String("/settings.ini"), QSettings::IniFormat);

        settings.beginGroup(QSL("User-Agent-Settings"));
        QStringList domainList = settings.value(QSL("DomainList"), QStringList()).toStringList();
        QStringList userAgentsList = settings.value(QSL("UserAgentsList"), QStringList()).toStringList();
        settings.endGroup();

        if (domainList.count() == userAgentsList.count()) {
            QStringList modifiedDomainList;
            QStringList modifiedUserAgentsList;

            for (int i = 0; i < domainList.count(); ++i) {
                QString host = QUrl(domainList.at(i)).host();
                if (!host.isEmpty()) {
                    modifiedDomainList.append(host);
                    modifiedUserAgentsList.append(userAgentsList.at(i));
                }
            }

            settings.beginGroup(QSL("User-Agent-Settings"));
            settings.setValue(QSL("DomainList"), modifiedDomainList);
            settings.setValue(QSL("UserAgentsList"), modifiedUserAgentsList);
            settings.endGroup();
            settings.sync();

            qInfo() << "ProfileManager: Updated UserAgent per domain settings";
        }
        else {
            qInfo() << "ProfileManager: Unable to update UserAgent per domain settings." << Qt::endl
                    << "Number of domains and UserAgents does not match." << Qt::endl
                    << "UserAgents were not changed.";
        }
    }
}

void ProfileManager::copyDataToProfile()
{
    QDir profileDir(DataPaths::currentProfilePath());

    QFile browseData(profileDir.filePath(QStringLiteral("browsedata.db")));

    if (browseData.exists()) {
        const QString browseDataBackup = QzTools::ensureUniqueFilename(profileDir.filePath(QStringLiteral("browsedata-backup.db")));
        browseData.copy(browseDataBackup);
        browseData.remove();

        QFile settings(profileDir.filePath(QSL("settings.ini")));
        if (settings.exists()) {
            const QString settingsBackup = QzTools::ensureUniqueFilename(profileDir.filePath(QSL("settings-backup.ini")));
            settings.copy(settingsBackup);
            settings.remove();
        }

        QFile sessionFile(profileDir.filePath(QSL("session.dat")));
        if (sessionFile.exists()) {
            QString oldVersion = QzTools::readAllFileContents(profileDir.filePath(QSL("version"))).trimmed();
            if (oldVersion.isEmpty()) {
                oldVersion = QSL("unknown-version");
            }
            const QString sessionBackup = QzTools::ensureUniqueFilename(profileDir.filePath(QSL("sessions/backup-%1.dat").arg(oldVersion)));
            sessionFile.copy(sessionBackup);
            sessionFile.remove();
        }

        const QString text = QSL("Incompatible profile version has been detected. To avoid losing your profile data, they were "
                             "backed up in following file:<br/><br/><b>") + browseDataBackup + QSL("<br/></b>");
        QMessageBox::warning(nullptr, QStringLiteral("Falkon: Incompatible profile version"), text);
    }
}

void ProfileManager::migrateFromQupZilla()
{
    if (mApp->isPortable()) {
        return;
    }

#if defined(Q_OS_WIN)
    const QString qzConfig = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QL1S("/qupzilla");
#elif defined(Q_OS_MACOS)
    const QString qzConfig = QDir::homePath() + QLatin1String("/Library/Application Support/QupZilla");
#else // Unix
    const QString qzConfig = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QL1S("/qupzilla");
#endif

    if (!QFileInfo::exists(qzConfig)) {
        return;
    }

    std::cout << "Falkon: Migrating config from QupZilla..." << std::endl;

    QzTools::copyRecursively(qzConfig, DataPaths::path(DataPaths::Config));
}

void ProfileManager::connectDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    if (!db.isValid()) {
        qCritical() << "Qt sqlite database driver is missing! Continuing without database....";
        return;
    }

    if (mApp->isPrivate()) {
        db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY"));
    }

    db.setDatabaseName(DataPaths::currentProfilePath() + QLatin1String("/browsedata.db"));

    if (!db.open()) {
        qCritical() << "Cannot open SQLite database! Continuing without database....";
        return;
    }

    if (db.tables().isEmpty()) {
        const QStringList statements = QzTools::readAllFileContents(QSL(":/data/browsedata.sql")).split(QL1C(';'));
        for (const QString &statement : statements) {
            const QString stmt = statement.trimmed();
            if (stmt.isEmpty()) {
                continue;
            }
            if (stmt.startsWith(QSL("--")) && !stmt.contains(QL1C('\n'))) {
                continue;
            }
            QSqlQuery query;
            if (!query.exec(stmt)) {
                qCritical() << "Error creating database schema" << query.lastError().text();
            }
        }
    }

    SqlDatabase::instance()->setDatabase(db);
}

void ProfileManager::updateDatabase()
{
    if (QString::fromLatin1(Qz::VERSION) == profileVersion) {
        return;
    }

    Updater::Version prof(profileVersion);

    /* Profile is from newer version than running application */
    if (prof > Updater::Version(QString::fromLatin1(Qz::VERSION))) {
        // Ignore
        return;
    }

    /* Do not try to update database of too old profile */
    if (prof < Updater::Version(QStringLiteral("1.9.0"))) {
        std::cout << "Falkon: Using profile from QupZilla " << qPrintable(profileVersion) << " is not supported!" << std::endl;
        return;
    }

    /* Update in 24.08.00 */
    if (prof < Updater::Version(QStringLiteral("24.07.70"))) {
        std::cout << "Falkon: Updating database to version " << qPrintable(QString::fromLatin1(Qz::VERSION)) << std::endl;

        SqlDatabase::instance()->database().transaction();

        QSqlQuery query(SqlDatabase::instance()->database());
        query.prepare(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS site_settings ("
                "id INTEGER PRIMARY KEY,"
                "server TEXT NOT NULL,"

                "zoom_level INTEGER DEFAULT -1,"
                "allow_cookies INTEGER DEFAULT 0,"

                "wa_autoload_images INTEGER DEFAULT 0,"
                "wa_js_enabled INTEGER DEFAULT 0,"
                "wa_js_open_windows INTEGER DEFAULT 0,"
                "wa_js_access_clipboard INTEGER DEFAULT 0,"
                "wa_js_can_paste INTEGER DEFAULT 0,"
                "wa_js_window_activation INTEGER DEFAULT 0,"
                "wa_local_storage INTEGER DEFAULT 0,"
                "wa_fullscreen_support INTEGER DEFAULT 0,"
                "wa_run_insecure_content INTEGER DEFAULT 0,"
                "wa_playback_needs_gesture INTEGER DEFAULT 0,"
                "wa_reading_from_canvas INTEGER DEFAULT 0,"
                "wa_force_dark_mode INTEGER DEFAULT 0,"

                "f_notifications INTEGER DEFAULT 0,"
                "f_geolocation INTEGER DEFAULT 0,"
                "f_media_audio_capture INTEGER DEFAULT 0,"
                "f_media_video_capture INTEGER DEFAULT 0,"
                "f_media_audio_video_capture INTEGER DEFAULT 0,"
                "f_mouse_lock INTEGER DEFAULT 0,"
                "f_desktop_video_capture INTEGER DEFAULT 0,"
                "f_desktop_audio_video_capture INTEGER DEFAULT 0"
            ");"
        ));

        if (!query.exec()) {
            qCritical() << "Error while creating table 'site_settings' in database: " << query.lastError().text();
            qFatal("ProfileManager::updateDatabase Unable to create table 'site_settings' in the database!");
        }

        query.prepare(QStringLiteral(
            "CREATE UNIQUE INDEX IF NOT EXISTS site_settings_server_uniqueindex ON site_settings (server);"
        ));

        if (!query.exec()) {
            qFatal() << "Error while creating unique index for table 'site_settings': " << query.lastError().text();
        }

        const QHash<QWebEnginePage::Feature, QString> html5SettingPairs = {
            {QWebEnginePage::Notifications, QSL("Notifications")},
            {QWebEnginePage::Geolocation, QSL("Geolocation")},
            {QWebEnginePage::MediaAudioCapture, QSL("MediaAudioCapture")},
            {QWebEnginePage::MediaVideoCapture, QSL("MediaVideoCapture")},
            {QWebEnginePage::MediaAudioVideoCapture, QSL("MediaAudioVideoCapture")},
            {QWebEnginePage::MouseLock, QSL("MouseLock")},
            {QWebEnginePage::DesktopVideoCapture, QSL("DesktopVideoCapture")},
            {QWebEnginePage::DesktopAudioVideoCapture,QSL("DesktopAudioVideoCapture")}
        };
        QHash<QString, SiteSettingsManager::SiteSettings> siteSettings;

        QSettings settings(DataPaths::currentProfilePath() + QLatin1String("/settings.ini"), QSettings::IniFormat);

        /* HTML5 permissions */
        settings.beginGroup(QSL("HTML5Notifications"));

        auto loadHtml5Settings = [&](const QString &suflix, const SiteSettingsManager::Permission permission) {
            for (auto [feature, settingName] : html5SettingPairs.asKeyValueRange()) {
                auto const serverList = settings.value(settingName + suflix, QStringList()).toStringList();

                for (const auto &serverUrl : serverList) {
                    const auto server = SiteSettingsManager::adjustUrl(QUrl(serverUrl));

                    if (!siteSettings.contains(server)) {
                        siteSettings[server] = SiteSettingsManager::SiteSettings();
                        for (auto [f, nameUnused] : html5SettingPairs.asKeyValueRange()) {
                            siteSettings[server].features[f] = SiteSettingsManager::Default;
                        }
                    }

                    siteSettings[server].server = server;
                    siteSettings[server].features[feature] = permission;
                }
            }
        };

        loadHtml5Settings(QSL("Granted"), SiteSettingsManager::Allow);
        loadHtml5Settings(QSL("Denied"), SiteSettingsManager::Deny);

        settings.endGroup();

        /* Cookies white/black lists */
        settings.beginGroup(QSL("Cookie-Settings"));

        auto loadCookiesSettings = [&](const QString &listName, const SiteSettingsManager::Permission permission) {
            auto const serverList = settings.value(listName, QStringList()).toStringList();

            for (const auto &serverUrl : serverList) {
                const auto server = SiteSettingsManager::adjustUrl(QUrl(serverUrl));
                if (!siteSettings.contains(server)) {
                    siteSettings[server] = SiteSettingsManager::SiteSettings();
                }

                siteSettings[server].server = server;
                siteSettings[server].AllowCookies = permission;
            }
        };

        loadCookiesSettings(QSL("whitelist"), SiteSettingsManager::Allow);
        loadCookiesSettings(QSL("blacklist"), SiteSettingsManager::Deny);

        settings.endGroup();

        /* Insert SQL for SiteSettings */
        query.prepare(QSL(
            "INSERT INTO site_settings ("
                "server,"
                "allow_cookies,"
                "f_notifications,"
                "f_geolocation,"
                "f_media_audio_capture,"
                "f_media_video_capture,"
                "f_media_audio_video_capture,"
                "f_mouse_lock,"
                "f_desktop_video_capture,"
                "f_desktop_audio_video_capture"
            ")"
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
        ));

        for (const auto &siteSetting : std::as_const(siteSettings)) {
            query.bindValue(0, siteSetting.server);
            query.bindValue(1, siteSetting.AllowCookies);
            query.bindValue(2, siteSetting.features[QWebEnginePage::Notifications]);
            query.bindValue(3, siteSetting.features[QWebEnginePage::Geolocation]);
            query.bindValue(4, siteSetting.features[QWebEnginePage::MediaAudioCapture]);
            query.bindValue(5, siteSetting.features[QWebEnginePage::MediaVideoCapture]);
            query.bindValue(6, siteSetting.features[QWebEnginePage::MediaAudioVideoCapture]);
            query.bindValue(7, siteSetting.features[QWebEnginePage::MouseLock]);
            query.bindValue(8, siteSetting.features[QWebEnginePage::DesktopVideoCapture]);
            query.bindValue(9, siteSetting.features[QWebEnginePage::DesktopAudioVideoCapture]);

            query.exec();
        }

        if (!SqlDatabase::instance()->database().commit()) {
            SqlDatabase::instance()->database().rollback();

            qFatal() << "Unable to update database.";
        }
    }
}
