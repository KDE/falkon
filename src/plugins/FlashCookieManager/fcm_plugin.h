/* ============================================================
* FlashCookieManager plugin for Falkon
* Copyright (C) 2014  S. Razi Alavizadeh <s.r.alavizadeh@gmail.com>
* Copyright (C) 2018  David Rosca <nowrep@gmail.com>
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
#ifndef FLASHCOOKIEMANAGER_H
#define FLASHCOOKIEMANAGER_H

#include "plugininterface.h"

#include <QPointer>
#include <QDateTime>

class BrowserWindow;
class FCM_Dialog;
class QTimer;
class AbstractButtonInterface;

struct FlashCookie {
    QString name;
    QString origin;
    int size;
    QString path;
    QString contents;
    QDateTime lastModification;

    bool operator ==(const FlashCookie &other) const {
        return (this->name == other.name && this->path == other.path);
    }
};

class FCM_Plugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.FlashCookieManager" FILE "flashcookiemanager.json")

public:
    explicit FCM_Plugin();

    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;
    void showSettings(QWidget *parent) override;

    void populateExtensionsMenu(QMenu *menu) override;

    void setFlashCookies(const QList<FlashCookie> &flashCookies);
    QList<FlashCookie> flashCookies();
    QStringList newCookiesList();
    void clearNewOrigins();
    void clearCache();
    QString flashPlayerDataPath() const;
    QVariantHash readSettings() const;
    void writeSettings(const QVariantHash &hashSettings);

    void removeCookie(const FlashCookie &flashCookie);

private Q_SLOTS:
    void autoRefresh();
    void showFlashCookieManager();
    void mainWindowCreated(BrowserWindow* window);
    void mainWindowDeleted(BrowserWindow* window);
    void startStopTimer();

private:
    AbstractButtonInterface* createStatusBarIcon(BrowserWindow* mainWindow);
    void loadFlashCookies();
    void loadFlashCookies(QString path);
    void insertFlashCookie(const QString &path);
    QString extractOriginFrom(const QString &path);
    bool isBlacklisted(const FlashCookie &flashCookie);
    bool isWhitelisted(const FlashCookie &flashCookie);
    void removeAllButWhitelisted();
    QString sharedObjectDirName() const;

    QHash<BrowserWindow*, AbstractButtonInterface*> m_statusBarIcons;
    QPointer<FCM_Dialog> m_fcmDialog;

    QString m_settingsPath;
    QList<FlashCookie> m_flashCookies;
    QTimer* m_timer;

    mutable QVariantHash m_settingsHash;
    bool m_autoMode;
    bool m_deleteOnClose;
    bool m_enableNotification;
    QStringList m_blacklist;
    QStringList m_whitelist;
    QStringList m_newCookiesList;
};

Q_DECLARE_METATYPE(FlashCookie);
#endif // FLASHCOOKIEMANAGER_H
