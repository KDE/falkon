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
#ifndef QZCOMMON_H
#define QZCOMMON_H

#include <QDebug>
#include <QFlags>
#include <QtWebEngine/QtWebEngineVersion>

#ifdef FALKON_SHAREDLIBRARY
#define FALKON_EXPORT Q_DECL_EXPORT
#else
#define FALKON_EXPORT Q_DECL_IMPORT
#endif

#ifndef Q_UNLIKELY
#define Q_UNLIKELY(x) x
#endif

#ifndef Q_LIKELY
#define Q_LIKELY(x) x
#endif

#ifndef QSL
#define QSL(x) QStringLiteral(x)
#endif

#ifndef QL1S
#define QL1S(x) QLatin1String(x)
#endif

#ifndef QL1C
#define QL1C(x) QLatin1Char(x)
#endif

namespace Qz
{
// Version of session.dat file
extern const int sessionVersion;

FALKON_EXPORT extern const char *APPNAME;
FALKON_EXPORT extern const char *VERSION;
FALKON_EXPORT extern const char *AUTHOR;
FALKON_EXPORT extern const char *COPYRIGHT;
FALKON_EXPORT extern const char *WWWADDRESS;
FALKON_EXPORT extern const char *BUGSADDRESS;
FALKON_EXPORT extern const char *WIKIADDRESS;

enum BrowserWindowType {
    BW_FirstAppWindow,
    BW_OtherRestoredWindow,
    BW_NewWindow,
    BW_MacFirstWindow
};

enum CommandLineAction {
    CL_NoAction,
    CL_OpenUrl,
    CL_OpenUrlInCurrentTab,
    CL_OpenUrlInNewWindow,
    CL_StartWithProfile,
    CL_StartWithoutAddons,
    CL_NewTab,
    CL_NewWindow,
    CL_ShowDownloadManager,
    CL_ToggleFullScreen,
    CL_StartPrivateBrowsing,
    CL_StartNewInstance,
    CL_StartPortable,
    CL_ExitAction,
    CL_WMClass
};

enum ObjectName {
    ON_WebView,
    ON_TabBar,
    ON_TabWidget,
    ON_BrowserWindow
};

enum NewTabPositionFlag {
    NT_SelectedTab = 1,
    NT_NotSelectedTab = 2,
    NT_CleanTab = 4,
    NT_TabAtTheEnd = 8,
    NT_NewEmptyTab = 16,

    NT_SelectedNewEmptyTab = NT_SelectedTab | NT_TabAtTheEnd | NT_NewEmptyTab,
    NT_SelectedTabAtTheEnd = NT_SelectedTab | NT_TabAtTheEnd,
    NT_NotSelectedTabAtTheEnd = NT_NotSelectedTab | NT_TabAtTheEnd,
    NT_CleanSelectedTabAtTheEnd = NT_SelectedTab | NT_TabAtTheEnd | NT_CleanTab,
    NT_CleanSelectedTab = NT_CleanTab | NT_SelectedTab,
    NT_CleanNotSelectedTab = NT_CleanTab | NT_NotSelectedTab
};

Q_DECLARE_FLAGS(NewTabPositionFlags, NewTabPositionFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(Qz::NewTabPositionFlags)

}

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
#define DEFAULT_THEME_NAME QSL("windows")
#elif defined(Q_OS_MACOS)
#define DEFAULT_THEME_NAME QSL("mac")
#elif defined(Q_OS_UNIX)
#define DEFAULT_THEME_NAME QSL("linux")
#else
#define DEFAULT_THEME_NAME QSL("default")
#endif

#ifdef Q_OS_WIN
#define DISABLE_CHECK_UPDATES false
#else
#define DISABLE_CHECK_UPDATES true
#endif

#define DEFAULT_CHECK_DEFAULTBROWSER false

#ifdef Q_OS_WIN
#define DEFAULT_DOWNLOAD_USE_NATIVE_DIALOG false
#else
#define DEFAULT_DOWNLOAD_USE_NATIVE_DIALOG true
#endif

#endif // QZCOMMON_H
