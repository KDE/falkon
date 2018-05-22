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

#include <QQmlEngine>

// static
void QmlPlugins::registerQmlTypes()
{
    registerQmlPluginInterface();

    registerQmlBookmarkTreeNode();
    registerQmlBookmarks();

    registerQmlMostVisitedUrl();
    registerQmlTopSites();

    registerQmlHistoryItem();
    registerQmlHistory();
}

// private static
void QmlPlugins::registerQmlPluginInterface()
{
    qmlRegisterType<QmlPluginInterface>("org.kde.falkon", 1, 0, "PluginInterface");
}

// private static
void QmlPlugins::registerQmlBookmarkTreeNode()
{
    qmlRegisterSingletonType<QmlBookmarkTreeNode>("org.kde.falkon", 1, 0, "BookmarkTreeNode", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlBookmarkTreeNode();
        return object;
    });
}

// private static
void QmlPlugins::registerQmlBookmarks()
{
    qmlRegisterSingletonType<QmlBookmarks>("org.kde.falkon", 1, 0, "Bookmarks", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlBookmarks();
        return object;
    });
}

// private static
void QmlPlugins::registerQmlMostVisitedUrl()
{
    qmlRegisterSingletonType<QmlMostVisitedUrl>("org.kde.falkon", 1, 0, "MostVisitedURL", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlMostVisitedUrl();
        return object;
    });
}

// private static
void QmlPlugins::registerQmlTopSites()
{
    qmlRegisterSingletonType<QmlTopSites>("org.kde.falkon", 1, 0, "TopSites", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlTopSites();
        return object;
    });
}

// private static
void QmlPlugins::registerQmlHistoryItem()
{
    qmlRegisterSingletonType<QmlHistoryItem>("org.kde.falkon", 1, 0, "HistoryItem", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlHistoryItem();
        return object;
    });
}

// private static
void QmlPlugins::registerQmlHistory()
{
    qmlRegisterSingletonType<QmlHistory>("org.kde.falkon", 1, 0, "History", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *object = new QmlHistory();
        return object;
    });
}
