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
#pragma once

#include "mainapplication.h"
#include "browserwindow.h"
#include "bookmarkitem.h"
#include "historyitem.h"
#include "api/bookmarks/qmlbookmarks.h"
#include "api/history/qmlhistory.h"
#include "api/cookies/qmlcookies.h"
#include "api/topsites/qmltopsites.h"
#include "api/tabs/qmltabs.h"
#include "api/clipboard/qmlclipboard.h"
#include "api/windows/qmlwindows.h"
#include "api/userscript/qmlexternaljsobject.h"
#include "api/userscript/qmluserscripts.h"
#include <QObject>
#include <QString>
#include <QNetworkCookie>

class QmlBookmarkTreeNode;
class QmlCookie;
class QmlHistoryItem;
class QmlTab;
class QmlMostVisitedUrl;
class QmlWindow;

class QmlStaticData : public QObject
{
    Q_OBJECT

public:
    explicit QmlStaticData(QObject *parent = nullptr);
    ~QmlStaticData();

    static QmlStaticData &instance();
    QmlBookmarkTreeNode *getBookmarkTreeNode(BookmarkItem *item);
    QmlCookie *getCookie(const QNetworkCookie &cookie);
    QmlHistoryItem *getHistoryItem(const HistoryEntry &entry);
    QmlTab *getTab(WebTab *webTab);
    QmlMostVisitedUrl *getMostVisitedUrl(const QString &title = QString(), const QString &url = QString());
    QmlWindow *getWindow(BrowserWindow *window);

    QHash<BrowserWindow*, int> windowIdHash();
    QIcon getIcon(const QString &iconPath, const QString &pluginPath);

    QmlBookmarks *getBookmarksSingleton();
    QmlHistory *getHistorySingleton();
    QmlCookies *getCookiesSingleton();
    QmlTopSites *getTopSitesSingleton();
    QmlTabs *getTabsSingleton();
    QmlClipboard *getClipboardSingleton();
    QmlWindows *getWindowsSingleton();
    QmlExternalJsObject *getExternalJsObjectSingleton();
    QmlUserScripts *getUserScriptsSingleton();
private:
    QHash<BookmarkItem*, QmlBookmarkTreeNode*> m_bookmarkTreeNodes;
    QHash<QNetworkCookie, QmlCookie*> m_cookies;
    QHash<HistoryEntry, QmlHistoryItem*> m_historyItems;
    QHash<WebTab*, QmlTab*> m_tabs;
    QHash<QPair<QString, QString>, QmlMostVisitedUrl*> m_urls;
    QHash<BrowserWindow*, QmlWindow*> m_windows;

    int m_newWindowId = 0;
    QHash<BrowserWindow*, int> m_windowIdHash;
};

inline bool operator ==(const HistoryEntry &x, const HistoryEntry &y) {
    return x.title == y.title && x.urlString == y.urlString && x.date == y.date && x.count == y.count;
}

inline uint qHash(const HistoryEntry &entry) {
    return qHash(QSL("%1 %2 %3 %4").arg(entry.title, entry.urlString, entry.date.toString(), QString::number(entry.count)));
}

inline uint qHash(const QNetworkCookie &cookie) {
    return qHash(QString::fromUtf8(cookie.toRawForm()));
}
