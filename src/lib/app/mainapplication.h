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
#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#define mApp MainApplication::instance()

#include <QList>
#include <QPointer>

#include "qtsingleapplication/qtsingleapplication.h"
#include "restoremanager.h"
#include "qzcommon.h"

class QMenu;
class QWebEngineProfile;
class QWebEngineSettings;
class QNetworkAccessManager;
class QWebEngineDownloadRequest;

class History;
class AutoFill;
class MainMenu;
class Bookmarks;
class CookieJar;
class AutoSaver;
class PluginProxy;
class BrowserWindow;
class NetworkManager;
class BrowsingLibrary;
class DownloadManager;
class UserAgentManager;
class SearchEnginesManager;
class HTML5PermissionsManager;
class RegisterQAppAssociation;
class DesktopNotificationsFactory;
class ProxyStyle;
class SessionManager;
class ClosedWindowsManager;
class ProtocolHandlerManager;
class SiteSettingsManager;

class FALKON_EXPORT MainApplication : public QtSingleApplication
{
    Q_OBJECT

public:
    enum AfterLaunch {
        OpenBlankPage = 0,
        OpenHomePage = 1,
        OpenSpeedDial = 2,
        RestoreSession = 3,
        SelectSession = 4
    };

    explicit MainApplication(int &argc, char** argv);
    ~MainApplication();

    void setChromiumFlags();

    bool isClosing() const;
    bool isPrivate() const;
    bool isPortable() const;
    bool isStartingAfterCrash() const;

    int windowCount() const;
    QList<BrowserWindow*> windows() const;

    BrowserWindow* getWindow() const;
    BrowserWindow* createWindow(Qz::BrowserWindowType type, const QUrl &startUrl);
    BrowserWindow* createWindow(Qz::BrowserWindowType type, const QList<QUrl> &startUrls = {});

    AfterLaunch afterLaunch() const;

    void openSession(BrowserWindow* window, RestoreData &restoreData);
    bool restoreSession(BrowserWindow* window, RestoreData restoreData);
    void destroyRestoreManager();
    void reloadSettings();

    // Name of current Qt style
    QString styleName() const;
    void setProxyStyle(ProxyStyle *style);

    QByteArray wmClass() const;

    History* history();
    Bookmarks* bookmarks();

    AutoFill* autoFill();
    CookieJar* cookieJar();
    PluginProxy* plugins();
    BrowsingLibrary* browsingLibrary();

    NetworkManager* networkManager();
    RestoreManager* restoreManager();
    SessionManager* sessionManager();
    DownloadManager* downloadManager();
    UserAgentManager* userAgentManager();
    SearchEnginesManager* searchEnginesManager();
    ClosedWindowsManager* closedWindowsManager();
    ProtocolHandlerManager *protocolHandlerManager();
    HTML5PermissionsManager* html5PermissionsManager();
    SiteSettingsManager* siteSettingsManager();
    DesktopNotificationsFactory* desktopNotifications();
    QWebEngineProfile* webProfile() const;
    QWebEngineSettings *webSettings() const;

    QByteArray saveState() const;

    static MainApplication* instance();

    static bool isTestModeEnabled();
    static void setTestModeEnabled(bool enabled);

public Q_SLOTS:
    void addNewTab(const QUrl &url = QUrl());
    void startPrivateBrowsing(const QUrl &startUrl = QUrl());

    void reloadUserStyleSheet();
    void restoreOverrideCursor();

    void changeOccurred();
    void quitApplication();

Q_SIGNALS:
    void settingsReloaded();
    void activeWindowChanged(BrowserWindow* window);

private Q_SLOTS:
    void postLaunch();

    void saveSettings();

    void messageReceived(const QString &message);
    void windowDestroyed(QObject* window);
    void onFocusChanged();
    void runDeferredPostLaunchActions();

    void downloadRequested(QWebEngineDownloadRequest *download);

private:
    enum PostLaunchAction {
        OpenDownloadManager,
        OpenNewTab,
        ToggleFullScreen
    };

    void loadSettings();
    void loadTheme(const QString &name);

    void setupUserScripts();
    void setUserStyleSheet(const QString &filePath);

    void checkDefaultWebBrowser();
    void checkOptimizeDatabase();

    void registerAllowedSchemes();
    void initNetworkInformation();

    bool m_isPrivate;
    bool m_isPortable;
    bool m_isClosing;
    bool m_isStartingAfterCrash;

    History* m_history;
    Bookmarks* m_bookmarks;

    AutoFill* m_autoFill;
    CookieJar* m_cookieJar;
    PluginProxy* m_plugins;
    BrowsingLibrary* m_browsingLibrary;

    NetworkManager* m_networkManager;
    RestoreManager* m_restoreManager;
    SessionManager* m_sessionManager;
    DownloadManager* m_downloadManager;
    UserAgentManager* m_userAgentManager;
    SearchEnginesManager* m_searchEnginesManager;
    ClosedWindowsManager* m_closedWindowsManager;
    ProtocolHandlerManager *m_protocolHandlerManager;
    HTML5PermissionsManager* m_html5PermissionsManager;
    SiteSettingsManager* m_siteSettingsManager;
    DesktopNotificationsFactory* m_desktopNotifications;
    QWebEngineProfile* m_webProfile;

    AutoSaver* m_autoSaver;
    ProxyStyle *m_proxyStyle = nullptr;

    QByteArray m_wmClass;

    QList<BrowserWindow*> m_windows;
    QPointer<BrowserWindow> m_lastActiveWindow;

    QList<PostLaunchAction> m_postLaunchActions;

    void initPulseSupport();

#if defined(Q_OS_WIN) && !defined(Q_OS_OS2)
public:
    RegisterQAppAssociation* associationManager();

private:
    RegisterQAppAssociation* m_registerQAppAssociation;
#endif

#ifdef Q_OS_MACOS
public:
    bool event(QEvent* e);
#endif
};

#endif // MAINAPPLICATION_H
