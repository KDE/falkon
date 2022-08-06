/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 David Rosca <nowrep@gmail.com>
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

// Make "signals:", "slots:" visible as access specifiers
#define QT_ANNOTATE_ACCESS_SPECIFIER(a) __attribute__((annotate(#a)))

// PYSIDE-711
#include <QHstsPolicy>
#include <QWebEngineContextMenuRequest>

// 3rdparty
#include "lineedit.h"
#include "squeezelabelv1.h"
#include "squeezelabelv2.h"
#include "qtsingleapplication/qtsingleapplication.h"

// app
#include "autosaver.h"
#include "browserwindow.h"
#include "datapaths.h"
#include "mainapplication.h"
#include "settings.h"

// autofill
#include "autofill.h"
#include "passwordmanager.h"
#include "passwordbackends/passwordbackend.h"

// bookmarks
#include "bookmarkitem.h"
#include "bookmarks.h"
#include "bookmarksmodel.h"
#include "bookmarkstools.h"

// cookies
#include "cookiejar.h"
#include "cookiemanager.h"

// downloads
#include "downloaditem.h"
#include "downloadmanager.h"

// history
#include "history.h"
#include "historyitem.h"
#include "historymodel.h"

// navigation
#include "locationbar.h"
#include "navigationbar.h"

// network
#include "networkmanager.h"
#include "urlinterceptor.h"
#include "schemehandlers/extensionschemehandler.h"

// notifications
#include "desktopnotificationsfactory.h"

// opensearch
#include "searchenginesdialog.h"
#include "searchenginesmanager.h"

// other
#include "checkboxdialog.h"
#include "qzsettings.h"
#include "statusbar.h"

// plugins
#include "pluginproxy.h"
#include "plugininterface.h"

// sidebar
#include "sidebar.h"
#include "sidebarinterface.h"

// siteSettings
#include "sitesettingsmanager.h"

// tabwidget
#include "combotabbar.h"
#include "tabbar.h"
#include "tabcontextmenu.h"
#include "tabicon.h"
#include "tabmodel.h"
#include "tabmrumodel.h"
#include "tabstackedwidget.h"
#include "tabtreemodel.h"
#include "tabwidget.h"
#include <QItemSelection>

// tools
#include "abstractbuttoninterface.h"
#include "clickablelabel.h"
#include "delayedfilewatcher.h"
#include "desktopfile.h"
#include "enhancedmenu.h"
#include "iconprovider.h"
#include "qztools.h"
#include "sqldatabase.h"
#include "toolbutton.h"
#include "wheelhelper.h"

// webengine
#include "loadrequest.h"
#include "webhittestresult.h"
#include "webinspector.h"
#include "webpage.h"
#include "webview.h"
#include "javascript/externaljsobject.h"

// webtab
#include "searchtoolbar.h"
#include "tabbedwebview.h"
#include "webtab.h"
