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
#include "api/notifications/qmlnotifications.h"
#include "api/clipboard/qmlclipboard.h"

#include <QQmlEngine>

// static
void QmlPlugins::registerQmlTypes()
{
    // PluginInterface
    qmlRegisterType<QmlPluginInterface>("org.kde.falkon", 1, 0, "PluginInterface");

    // Bookmarks
    qmlRegisterUncreatableType<QmlBookmarkTreeNode>("org.kde.falkon", 1, 0, "BookmarkTreeNode", "Unable to create Type: BookmarkTreeNode");

    qmlRegisterSingletonType<QmlBookmarks>("org.kde.falkon", 1, 0, "Bookmarks", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlBookmarks();
        return object;
    });

    // TopSites
    qmlRegisterUncreatableType<QmlMostVisitedUrl>("org.kde.falkon", 1, 0, "MostVisitedURL", "Unable to create Type: MostVisitedURL");

    qmlRegisterSingletonType<QmlTopSites>("org.kde.falkon", 1, 0, "TopSites", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlTopSites();
        return object;
    });

    // History
    qmlRegisterUncreatableType<QmlHistoryItem>("org.kde.falkon", 1, 0, "HistoryItem", "Unable to create Type: HistoryItem");

    qmlRegisterSingletonType<QmlHistory>("org.kde.falkon", 1, 0, "History", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlHistory();
        return object;
    });

    // Cookies
    qmlRegisterUncreatableType<QmlCookie>("org.kde.falkon", 1, 0, "Cookie", "Unable to create Type: Cookie");

    qmlRegisterSingletonType<QmlCookies>("org.kde.falkon", 1, 0, "Cookies", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlCookies();
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
    });}
