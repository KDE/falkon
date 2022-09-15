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
#include <QWebEnginePage>
#include <QWebEngineSettings>

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
    enum Permission {
        Default = 0,
        Allow   = 1,
        Deny    = 2,
        Ask     = 3,
    };
    Q_ENUM(Permission);

    /* Keep the attributes at the top */
    enum PageOptions {
        poAutoloadImages,

        /* Javascript stuff */
        poJavascriptEnabled,
        poJavascriptCanOpenWindows,
        poJavascriptCanAccessClipboard,
        poJavascriptCanPaste,
        poAllowWindowActivationFromJavaScript,

        poLocalStorageEnabled,
        poScrollAnimatorEnabled,
        poFullScreenSupportEnabled,
        poAllowRunningInsecureContent,
        poAllowGeolocationOnInsecureOrigins,
        poPlaybackRequiresUserGesture,
        poWebRTCPublicInterfacesOnly,

        poAllowCookies,
        poZoomLevel,

        poAllowNotifications,
        poAllowGeolocation,
        poAllowMediaAudioCapture,
        poAllowMediaVideoCapture,
        poAllowMediaAudioVideoCapture,
        poAllowMouseLock,
        poAllowDesktopVideoCapture,
        poAllowDesktopAudioVideoCapture,
    };
    Q_ENUM(PageOptions);

    struct SiteSettings
    {
        Permission AllowCookies;
        int ZoomLevel;
        QHash<QWebEngineSettings::WebAttribute, Permission> attributes; /* Enable disable soem feature eg. Javascript, Images etc */
        QHash<QWebEnginePage::Feature, Permission> features; /* HTML permissions */
        QString server;
    };

    explicit SiteSettingsManager(QObject *parent = 0);
    ~SiteSettingsManager();

    void loadSettings();
    void saveSettings();

    QHash<QWebEngineSettings::WebAttribute, bool> getWebAttributes(const QUrl &url);

    void setJavascript(const QUrl &url, const int value);
    void setImages(const QUrl &url, const int value);

    Permission getPermission(const PageOptions option, const QUrl &url);
    Permission getPermission(const QWebEnginePage::Feature &feature, const QUrl &url);

    void setOption(const PageOptions option, const QUrl &url, const int value);
    void setOption(const QWebEnginePage::Feature &feature, const QUrl &url, const Permission &value);

    Permission getDefaultPermission(const PageOptions &option);
    Permission getDefaultPermission(const QWebEnginePage::Feature &feature);
    void setDefaultPermission(const PageOptions &option, const Permission &permission);
    void setDefaultPermission(const QWebEnginePage::Feature &feature, const Permission &value);
    void setDefaultPermission(const PageOptions &option, const int &value);

    QString sqlColumnFromWebEngineFeature(const QWebEnginePage::Feature &feature);

    QString getOptionName(const SiteSettingsManager::PageOptions &option);
    QString getOptionName(const QWebEnginePage::Feature &feature);

    QString getPermissionName(const Permission permission);

private:
    QString webAttributeToSqlColumn(const QWebEngineSettings::WebAttribute &attribute);
    QString optionToSqlColumn(const PageOptions &option);
    PageOptions optionFromWebEngineFeature(const QWebEnginePage::Feature &feature) const;
    Permission testAttribute(const QWebEngineSettings::WebAttribute attribute) const;
    Permission intToPermission(const int permission) const;

    QMap<PageOptions, Permission> m_defaults;
    QList<QWebEngineSettings::WebAttribute> supportedAttribute;
    QString attributesSql;
};


using SiteSettings = SiteSettingsManager::SiteSettings;

// Hint to QVector to use std::realloc on item moving
Q_DECLARE_TYPEINFO(SiteSettings, Q_MOVABLE_TYPE);


#endif // SITESETTINGS_MANAGER_H
