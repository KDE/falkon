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
#include "api/windows/qmlwindowstate.h"
#include "api/windows/qmlwindowtype.h"
#include "api/browseraction/qmlbrowseraction.h"
#include "api/sidebar/qmlsidebar.h"
#include "api/menus/qmlmenu.h"
#include "api/menus/qmlaction.h"
#include "api/menus/qmlwebhittestresult.h"
#include "api/settings/qmlsettings.h"
#include "api/events/qmlqzobjects.h"
#include "api/events/qmlmouseevent.h"
#include "api/events/qmlwheelevent.h"
#include "api/i18n/qmli18n.h"

#include <QQmlEngine>
#include <QQmlContext>

// static
void QmlPlugins::registerQmlTypes()
{
    // PluginInterface
    qmlRegisterType<QmlPluginInterface>("org.kde.falkon", 1, 0, "PluginInterface");

    // Bookmarks
    qmlRegisterUncreatableType<QmlBookmarkTreeNode>("org.kde.falkon", 1, 0, "BookmarkTreeNode", "Unable to register type: BookmarkTreeNode");

    qmlRegisterSingletonType<QmlBookmarks>("org.kde.falkon", 1, 0, "Bookmarks", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlBookmarks();
        return object;
    });

    // TopSites
    qmlRegisterUncreatableType<QmlMostVisitedUrl>("org.kde.falkon", 1, 0, "MostVisitedURL", "Unable to register type: MostVisitedURL");

    qmlRegisterSingletonType<QmlTopSites>("org.kde.falkon", 1, 0, "TopSites", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlTopSites();
        return object;
    });

    // History
    qmlRegisterUncreatableType<QmlHistoryItem>("org.kde.falkon", 1, 0, "HistoryItem", "Unable to register type: HistoryItem");

    qmlRegisterSingletonType<QmlHistory>("org.kde.falkon", 1, 0, "History", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlHistory();
        return object;
    });

    // Cookies
    qmlRegisterUncreatableType<QmlCookie>("org.kde.falkon", 1, 0, "Cookie", "Unable to register type: Cookie");

    qmlRegisterSingletonType<QmlCookies>("org.kde.falkon", 1, 0, "Cookies", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlCookies();
        return object;
    });

    // Tabs
    qmlRegisterUncreatableType<QmlTab>("org.kde.falkon", 1, 0, "Tab", "Unable to register type: Tab");

    qmlRegisterSingletonType<QmlTabs>("org.kde.falkon", 1, 0, "Tabs", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlTabs();
        return object;
    });

    // Notifications
    qmlRegisterSingletonType<QmlNotifications>("org.kde.falkon", 1, 0, "Notifications", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlNotifications();
        return object;
    });

    // Clipboard
    qmlRegisterSingletonType<QmlClipboard>("org.kde.falkon", 1, 0, "Clipboard", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlClipboard();
        return object;
    });

    // Windows
    qmlRegisterUncreatableType<QmlWindow>("org.kde.falkon", 1, 0, "Window", "Unable to register type: Window");

    qmlRegisterUncreatableType<QmlWindowState>("org.kde.falkon", 1, 0, "WindowState", "Unable to register type: WindowState");

    qmlRegisterUncreatableType<QmlWindowType>("org.kde.falkon", 1, 0, "WindowType", "Unable to register type: WindowType");

    qmlRegisterSingletonType<QmlWindows>("org.kde.falkon", 1, 0, "Windows", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlWindows();
        return object;
    });

    // BrowserAction
    qmlRegisterType<QmlBrowserAction>("org.kde.falkon", 1, 0, "BrowserAction");

    // SideBar
    qmlRegisterType<QmlSideBar>("org.kde.falkon", 1, 0, "SideBar");

    // Menu
    qmlRegisterUncreatableType<QmlMenu>("org.kde.falkon", 1, 0, "Menu", "Unable to register type: Menu");

    // Action
    qmlRegisterUncreatableType<QmlAction>("org.kde.falkon", 1, 0, "Action", "Unable to register type: Action");

    // WebHitTestResult
    qmlRegisterUncreatableType<QmlWebHitTestResult>("org.kde.falkon", 1, 0, "WebHitTestResult", "Unable to register type: WebHitTestResult");

    // Settings
    qmlRegisterType<QmlSettings>("org.kde.falkon", 1, 0, "Settings");

    // Qz::Objects
    qmlRegisterUncreatableType<QmlQzObjects>("org.kde.falkon", 1, 0, "QzObjects", "Unable to register type: QzObjects");

    // MouseEvents
    qmlRegisterUncreatableType<QmlMouseEvent>("org.kde.falkon", 1, 0, "MouseEvent", "Unable to register type: MouseEvent");

    // WheelEvents
    qmlRegisterUncreatableType<QmlWheelEvent>("org.kde.falkon", 1, 0, "WheelEvent", "Unable to register type: WheelEvent");

    // i18n
    qmlRegisterSingletonType<QmlI18n>("org.kde.falkon", 1, 0, "I18n", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(scriptEngine)
        QString pluginName = engine->rootContext()->contextProperty("__name__").toString();
        auto *object = new QmlI18n(pluginName);
        return object;
    });
}
