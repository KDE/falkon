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
#include "qmlstaticdata.h"
#include "api/bookmarks/qmlbookmarktreenode.h"
#include "api/cookies/qmlcookie.h"
#include "api/history/qmlhistoryitem.h"
#include "api/tabs/qmltab.h"
#include "api/topsites/qmlmostvisitedurl.h"
#include "api/windows/qmlwindow.h"
#include "api/fileutils/qmlfileutils.h"
#include "pluginproxy.h"

QmlStaticData::QmlStaticData(QObject *parent)
    : QObject(parent)
{
    const QList<BrowserWindow*> windows = mApp->windows();
    for (BrowserWindow *window : windows) {
        m_windowIdHash.insert(window, m_newWindowId++);
    }

    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, [this](BrowserWindow *window) {
        m_windowIdHash.insert(window, m_newWindowId++);
    });

    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, this, [this](BrowserWindow *window) {
        m_windowIdHash.remove(window);
    });
}

QmlStaticData::~QmlStaticData()
{
    qDeleteAll(m_bookmarkTreeNodes);
    qDeleteAll(m_cookies);
    qDeleteAll(m_historyItems);
    qDeleteAll(m_tabs);
    qDeleteAll(m_urls);
    qDeleteAll(m_windows);
}

QmlStaticData &QmlStaticData::instance()
{
    static QmlStaticData qmlStaticData;
    return qmlStaticData;
}

QmlBookmarkTreeNode *QmlStaticData::getBookmarkTreeNode(BookmarkItem *item)
{
    QmlBookmarkTreeNode *node = m_bookmarkTreeNodes.value(item);
    if (!node) {
        node = new QmlBookmarkTreeNode(item);
        m_bookmarkTreeNodes.insert(item, node);
    }
    return node;
}

QmlCookie *QmlStaticData::getCookie(const QNetworkCookie &cookie)
{
    QmlCookie *qmlCookie = m_cookies.value(cookie);
    if (!qmlCookie) {
        qmlCookie = new QmlCookie(new QNetworkCookie(cookie));
        m_cookies.insert(cookie, qmlCookie);
    }
    return qmlCookie;
}

QmlHistoryItem *QmlStaticData::getHistoryItem(const HistoryEntry &entry)
{
    QmlHistoryItem *item = m_historyItems.value(entry);
    if (!item) {
        item = new QmlHistoryItem(entry);
        m_historyItems.insert(entry, item);
    }
    return item;
}

QmlTab *QmlStaticData::getTab(WebTab *webTab)
{
    QmlTab *tab = m_tabs.value(webTab);
    if (!tab) {
        tab = new QmlTab(webTab);
        m_tabs.insert(webTab, tab);
    }
    return tab;
}

QmlMostVisitedUrl *QmlStaticData::getMostVisitedUrl(const QString &title, const QString &url)
{
    QmlMostVisitedUrl *visitedUrl = m_urls.value({title, url});
    if (!visitedUrl) {
        visitedUrl = new QmlMostVisitedUrl(title, url);
        m_urls.insert({title, url}, visitedUrl);
    }
    return visitedUrl;
}

QmlWindow *QmlStaticData::getWindow(BrowserWindow *window)
{
    QmlWindow *qmlWindow = m_windows.value(window);
    if (!qmlWindow) {
        qmlWindow = new QmlWindow(window);
        m_windows.insert(window, qmlWindow);
    }
    return qmlWindow;
}

QHash<BrowserWindow*, int> QmlStaticData::windowIdHash()
{
    return m_windowIdHash;
}

QIcon QmlStaticData::getIcon(const QString &iconPath, const QString &pluginPath)
{
    QIcon icon;
    if (QIcon::hasThemeIcon(iconPath)) {
        icon = QIcon::fromTheme(iconPath);
    } else {
        QmlFileUtils fileUtils(pluginPath);
        icon = QIcon(fileUtils.resolve(iconPath));
    }
    return icon;
}

QmlBookmarks *QmlStaticData::getBookmarksSingleton()
{
    static QmlBookmarks *bookmarks = new QmlBookmarks(this);
    return bookmarks;
}

QmlHistory *QmlStaticData::getHistorySingleton()
{
    static QmlHistory *history = new QmlHistory(this);
    return history;
}

QmlCookies *QmlStaticData::getCookiesSingleton()
{
    static QmlCookies *cookies = new QmlCookies(this);
    return cookies;
}

QmlTopSites *QmlStaticData::getTopSitesSingleton()
{
    static QmlTopSites *topSites = new QmlTopSites(this);
    return topSites;
}

QmlTabs *QmlStaticData::getTabsSingleton()
{
    static QmlTabs *tabs = new QmlTabs(this);
    return tabs;
}

QmlClipboard *QmlStaticData::getClipboardSingleton()
{
    static QmlClipboard *clipboard = new QmlClipboard(this);
    return clipboard;
}

QmlWindows *QmlStaticData::getWindowsSingleton()
{
    static QmlWindows *windows = new QmlWindows(this);
    return windows;
}

QmlExternalJsObject *QmlStaticData::getExternalJsObjectSingleton()
{
    static QmlExternalJsObject *externalJsObject = new QmlExternalJsObject(this);
    return externalJsObject;
}

QmlUserScripts *QmlStaticData::getUserScriptsSingleton()
{
    static QmlUserScripts *userScripts = new QmlUserScripts(this);
    return userScripts;
}
