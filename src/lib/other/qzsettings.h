/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2016  David Rosca <nowrep@gmail.com>
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
#ifndef QZSETTINGS_H
#define QZSETTINGS_H

#include "qzcommon.h"
#include "settings.h"

#include <QStringList>
#include <QColor>

class FALKON_EXPORT QzSettings
{
public:
    QzSettings();

    void loadSettings();
    void saveSettings();

    // AddressBar
    bool selectAllOnDoubleClick;
    bool selectAllOnClick;
    bool showLoadingProgress;
    int showLocationSuggestions;
    bool showSwitchTab;
    bool alwaysShowGoIcon;
    bool useInlineCompletion;
    bool showZoomLabel;
    bool completionPopupExpandToWindow;

    // SearchEngines
    bool searchOnEngineChange;
    bool searchFromAddressBar;
    bool searchWithDefaultEngine;
    bool showABSearchSuggestions;
    bool showWSBSearchSuggestions;

    // Web-Browser-Settings
    int defaultZoomLevel;
    bool loadTabsOnActivation;

    QStringList autoOpenProtocols;
    QStringList blockedProtocols;

    QStringList allowedSchemes;
    QStringList blockedSchemes;

    // Browser-Tabs-Settings
    Qz::NewTabPositionFlags newTabPosition;
    bool tabsOnTop;
    bool openPopupsInTabs;
    bool blockAutomaticPopups;
    bool alwaysSwitchTabsWithWheel;

    /* Browser-View-Settings */
    QColor backgroundColorLoading;
    QColor backgroundColorLoaded;
};

#define qzSettings Settings::staticSettings()

#endif // QZSETTINGS_H
