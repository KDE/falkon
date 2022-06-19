/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmlplugins.h"
#include "qmlplugininterface.h"
#include "qmlengine.h"
#include "api/bookmarks/qmlbookmarktreenode.h"
#include "api/bookmarks/qmlbookmarks.h"
#include "api/topsites/qmlmostvisitedurl.h"
#include "api/topsites/qmltopsites.h"
#include "api/history/qmlhistoryitem.h"
#include "api/history/qmlhistory.h"
#include "api/cookies/qmlcookie.h"
#include "api/cookies/qmlcookies.h"
#include "api/tabs/qmltab.h"
#include "api/tabs/qmltabs.h"
#include "api/notifications/qmlnotifications.h"
#include "api/clipboard/qmlclipboard.h"
#include "api/windows/qmlwindow.h"
#include "api/windows/qmlwindows.h"
#include "api/browseraction/qmlbrowseraction.h"
#include "api/sidebar/qmlsidebar.h"
#include "api/menus/qmlmenu.h"
#include "api/menus/qmlaction.h"
#include "api/menus/qmlwebhittestresult.h"
#include "api/settings/qmlsettings.h"
#include "api/events/qmlqzobjects.h"
#include "api/events/qmlkeyevent.h"
#include "api/events/qmlmouseevent.h"
#include "api/events/qmlwheelevent.h"
#include "api/userscript/qmluserscript.h"
#include "api/userscript/qmluserscripts.h"
#include "api/userscript/qmlexternaljsobject.h"
#include "api/extensionscheme/qmlextensionscheme.h"
#include "api/extensionscheme/qmlwebengineurlrequestjob.h"
#include "api/fileutils/qmlfileutils.h"
#include "api/qmlenums.h"
#include "qml/qmlstaticdata.h"
#include <QQmlEngine>
#include <QQmlContext>

// static
void QmlPlugins::registerQmlTypes()
{
    const char *url = "org.kde.falkon";
    const int majorVersion = 1;
    const int minorVersion = 0;
    // PluginInterface
    qmlRegisterType<QmlPluginInterface>(url, majorVersion, minorVersion, "PluginInterface");

    // Bookmarks
    qmlRegisterUncreatableType<QmlBookmarkTreeNode>(url, majorVersion, minorVersion, "BookmarkTreeNode", QSL("Unable to register type: BookmarkTreeNode"));

    qmlRegisterSingletonType<QmlBookmarks>(url, majorVersion, minorVersion, "Bookmarks", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return QmlStaticData::instance().getBookmarksSingleton();
    });

    // TopSites
    qmlRegisterUncreatableType<QmlMostVisitedUrl>(url, majorVersion, minorVersion, "MostVisitedURL", QSL("Unable to register type: MostVisitedURL"));

    qmlRegisterSingletonType<QmlTopSites>(url, majorVersion, minorVersion, "TopSites", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return QmlStaticData::instance().getTopSitesSingleton();
    });

    // History
    qmlRegisterUncreatableType<QmlHistoryItem>(url, majorVersion, minorVersion, "HistoryItem", QSL("Unable to register type: HistoryItem"));

    qmlRegisterSingletonType<QmlHistory>(url, majorVersion, minorVersion, "History", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return QmlStaticData::instance().getHistorySingleton();
    });

    // Cookies
    qmlRegisterUncreatableType<QmlCookie>(url, majorVersion, minorVersion, "Cookie", "Unable to register type: Cookie");

    qmlRegisterSingletonType<QmlCookies>(url, majorVersion, minorVersion, "Cookies", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return QmlStaticData::instance().getCookiesSingleton();
    });

    // Tabs
    qmlRegisterUncreatableType<QmlTab>(url, majorVersion, minorVersion, "Tab", QSL("Unable to register type: Tab"));

    qmlRegisterSingletonType<QmlTabs>(url, majorVersion, minorVersion, "Tabs", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return QmlStaticData::instance().getTabsSingleton();
    });

    // Notifications
    qmlRegisterSingletonType<QmlNotifications>(url, majorVersion, minorVersion, "Notifications", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(scriptEngine)

        auto *qmlEngine = qobject_cast<QmlEngine*>(engine);
        if (!qmlEngine) {
            qWarning() << "Unable to cast QQmlEngine * to QmlEngine *";
            return nullptr;
        }
        QString filePath = qmlEngine->extensionPath();

        auto *object = new QmlNotifications();
        object->setPluginPath(filePath);
        return object;
    });

    // Clipboard
    qmlRegisterSingletonType<QmlClipboard>(url, majorVersion, minorVersion, "Clipboard", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return QmlStaticData::instance().getClipboardSingleton();
    });

    // Windows
    qmlRegisterUncreatableType<QmlWindow>(url, majorVersion, minorVersion, "Window", QSL("Unable to register type: Window"));

    qmlRegisterSingletonType<QmlWindows>(url, majorVersion, minorVersion, "Windows", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return QmlStaticData::instance().getWindowsSingleton();
    });

    // BrowserAction
    qmlRegisterType<QmlBrowserAction>(url, majorVersion, minorVersion, "BrowserAction");

    // SideBar
    qmlRegisterType<QmlSideBar>(url, majorVersion, minorVersion, "SideBar");

    // Menu
    qmlRegisterUncreatableType<QmlMenu>(url, majorVersion, minorVersion, "Menu", QSL("Unable to register type: Menu"));

    // Action
    qmlRegisterUncreatableType<QmlAction>(url, majorVersion, minorVersion, "Action", QSL("Unable to register type: Action"));

    // WebHitTestResult
    qmlRegisterUncreatableType<QmlWebHitTestResult>(url, majorVersion, minorVersion, "WebHitTestResult", QSL("Unable to register type: WebHitTestResult"));

    // Settings
    qmlRegisterType<QmlSettings>(url, majorVersion, minorVersion, "Settings");

    // Qz::Objects
    qmlRegisterUncreatableType<QmlQzObjects>(url, majorVersion, minorVersion, "QzObjects", QSL("Unable to register type: QzObjects"));

    // KeyEvents
    qmlRegisterUncreatableType<QmlKeyEvent>(url, majorVersion, minorVersion, "KeyEvent", QSL("Unable to register type: KeyEvent"));

    // MouseEvents
    qmlRegisterUncreatableType<QmlMouseEvent>(url, majorVersion, minorVersion, "MouseEvent", QSL("Unable to register type: MouseEvent"));

    // WheelEvents
    qmlRegisterUncreatableType<QmlWheelEvent>(url, majorVersion, minorVersion, "WheelEvent", QSL("Unable to register type: WheelEvent"));

    // UserScripts
    qmlRegisterType<QmlUserScript>(url, majorVersion, minorVersion, "UserScript");

    qmlRegisterSingletonType<QmlUserScripts>(url, majorVersion, minorVersion, "UserScripts", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return QmlStaticData::instance().getUserScriptsSingleton();
    });

    qmlRegisterSingletonType<QmlExternalJsObject>(url, majorVersion, minorVersion, "ExternalJsObject", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return QmlStaticData::instance().getExternalJsObjectSingleton();
    });

    // ExtensionScheme
    qmlRegisterType<QmlExtensionScheme>(url, majorVersion, minorVersion, "ExtensionScheme");

    qmlRegisterUncreatableType<QmlWebEngineUrlRequestJob>(url, majorVersion, minorVersion, "WebEngineUrlRequestJob", QSL("Unable to register type: WebEngineUrlRequestJob"));

    // FileUtils
    qmlRegisterSingletonType<QmlFileUtils>(url, majorVersion, minorVersion, "FileUtils", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(scriptEngine)

        auto *qmlEngine = qobject_cast<QmlEngine*>(engine);
        if (!qmlEngine) {
            qWarning() << "Unable to cast QQmlEngine * to QmlEngine *";
            return nullptr;
        }
        QString filePath = qmlEngine->extensionPath();
        return new QmlFileUtils(filePath);
    });

    qmlRegisterUncreatableType<QmlEnums>(url, majorVersion, minorVersion, "Enums", QSL("Unable to register type: Enums"));
}
