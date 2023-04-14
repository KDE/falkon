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

    /* Broser options */
    enum PageOptions {
        poAllowCookies,
        poZoomLevel,
    };
    Q_ENUM(PageOptions);

    struct SiteSettings
    {
        Permission AllowCookies = Default;
        int ZoomLevel = -1;
        QMap<QWebEngineSettings::WebAttribute, Permission> attributes; /* Enable disable soem feature eg. Javascript, Images etc */
        QMap<QWebEnginePage::Feature, Permission> features; /* HTML permissions */
        QString server;
        bool operator==(const SiteSettings &other) const {
            for (auto it = attributes.begin(); it != attributes.end(); ++it) {
                if (it.value() != other.attributes[it.key()]) {
                    return false;
                }
            }
            for (auto it = features.begin(); it != features.end(); ++it) {
                if (it.value() != other.features[it.key()]) {
                    return false;
                }
            }
            if ((AllowCookies != other.AllowCookies)
                || (ZoomLevel != other.ZoomLevel)
                || (server != other.server)
            ) {
                return false;
            }
            return true;
        }
    };

    explicit SiteSettingsManager(QObject *parent = 0);
    ~SiteSettingsManager();

    void loadSettings();
    void saveSettings();

    QHash<QWebEngineSettings::WebAttribute, bool> getWebAttributes(const QUrl &url);

    void setJavascript(const QUrl &url, const int value);
    void setImages(const QUrl &url, const int value);

    Permission getPermission(const QString &column, const QString &host);
    Permission getPermission(const PageOptions option, const QString &host);
    Permission getPermission(const QWebEnginePage::Feature &feature, const QString &host);
    Permission getPermission(const QWebEngineSettings::WebAttribute &attribute, const QString &host);

    Permission getPermission(const QString &column, const QUrl &url);
    Permission getPermission(const PageOptions option, const QUrl &url);
    Permission getPermission(const QWebEnginePage::Feature &feature, const QUrl &url);
    Permission getPermission(const QWebEngineSettings::WebAttribute &attribute, const QUrl &url);

    void setOption(const QString &column, const QUrl &url, const int value);
    void setOption(const PageOptions option, const QUrl &url, const int value);
    void setOption(const QWebEnginePage::Feature &feature, const QUrl &url, const Permission &value);
    void setOption(const QWebEngineSettings::WebAttribute &attribute, const QUrl &url, const Permission &value);

    Permission getDefaultPermission(const PageOptions &option);
    Permission getDefaultPermission(const QWebEnginePage::Feature &feature);
    Permission getDefaultPermission(const QWebEngineSettings::WebAttribute &attribute);
    void setDefaultPermission(const PageOptions &option, const Permission &permission);
    void setDefaultPermission(const QWebEnginePage::Feature &feature, const Permission &value);
    void setDefaultPermission(const PageOptions &option, const int &value);

    QString getOptionName(const SiteSettingsManager::PageOptions &option);
    QString getOptionName(const QWebEnginePage::Feature &feature);
    QString getOptionName(const QWebEngineSettings::WebAttribute attribute);

    QString getPermissionName(const Permission permission);

    QString webAttributeToSqlColumn(const QWebEngineSettings::WebAttribute &attribute);
    QString featureToSqlColumn(const QWebEnginePage::Feature &feature);
    QString optionToSqlColumn(const PageOptions &option);

    QList<QWebEngineSettings::WebAttribute> getSupportedAttribute() const;
    QList<QWebEnginePage::Feature> getSupportedFeatures() const;

    SiteSettings getSiteSettings(QUrl &url);
    void setSiteSettings(SiteSettings &siteSettings);
    QString sqlTable();

private:
    void prepareSqls();

    Permission testAttribute(const QWebEngineSettings::WebAttribute attribute) const;
    Permission intToPermission(const int permission) const;

    QList<QWebEngineSettings::WebAttribute> supportedAttribute;
    QList<QWebEnginePage::Feature> supportedFeatures;

    QMap<QWebEnginePage::Feature, Permission> defaultFeatures;
    QMap<QWebEngineSettings::WebAttribute, Permission> defaultAttributes;
    QMap<PageOptions, Permission> defaultOptions;

    QString attributesSql;
    QString everythingSql;
    QString everythingInsertSql;
    QString everythingUpdateSql;
};


using SiteSettings = SiteSettingsManager::SiteSettings;

// Hint to QVector to use std::realloc on item moving
Q_DECLARE_TYPEINFO(SiteSettings, Q_MOVABLE_TYPE);


#endif // SITESETTINGS_MANAGER_H
