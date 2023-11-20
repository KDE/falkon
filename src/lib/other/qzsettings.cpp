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
#include "qzsettings.h"
#include "webview.h"

QzSettings::QzSettings()
{
    loadSettings();
}

void QzSettings::loadSettings()
{
    Settings settings;
    settings.beginGroup(QSL("AddressBar"));
    selectAllOnDoubleClick = settings.value(QSL("SelectAllTextOnDoubleClick"), true).toBool();
    selectAllOnClick = settings.value(QSL("SelectAllTextOnClick"), false).toBool();
    showLoadingProgress = settings.value(QSL("ShowLoadingProgress"), false).toBool();
    showLocationSuggestions = settings.value(QSL("showSuggestions"), 0).toInt();
    showSwitchTab = settings.value(QSL("showSwitchTab"), true).toBool();
    alwaysShowGoIcon = settings.value(QSL("alwaysShowGoIcon"), false).toBool();
    useInlineCompletion = settings.value(QSL("useInlineCompletion"), true).toBool();
    showZoomLabel = settings.value(QSL("showZoomLabel"), true).toBool();
    completionPopupExpandToWindow = settings.value(QSL("CompletionPopupExpandToWindow"), false).toBool();
    settings.endGroup();

    settings.beginGroup(QSL("SearchEngines"));
    searchOnEngineChange = settings.value(QSL("SearchOnEngineChange"), true).toBool();
    searchFromAddressBar = settings.value(QSL("SearchFromAddressBar"), true).toBool();
    searchWithDefaultEngine = settings.value(QSL("SearchWithDefaultEngine"), true).toBool();
    showABSearchSuggestions = settings.value(QSL("showSearchSuggestions"), true).toBool();
    showWSBSearchSuggestions = settings.value(QSL("showSuggestions"), true).toBool();
    settings.endGroup();

    settings.beginGroup(QSL("Web-Browser-Settings"));
    defaultZoomLevel = settings.value(QSL("DefaultZoomLevel"), WebView::zoomLevels().indexOf(100)).toInt();
    loadTabsOnActivation = settings.value(QSL("LoadTabsOnActivation"), true).toBool();
    autoOpenProtocols = settings.value(QSL("AutomaticallyOpenProtocols"), QStringList()).toStringList();
    blockedProtocols = settings.value(QSL("BlockOpeningProtocols"), QStringList()).toStringList();
    allowedSchemes = settings.value(QSL("AllowedSchemes"), QStringList()).toStringList();
    blockedSchemes = settings.value(QSL("BlockedSchemes"), QStringList()).toStringList();
    settings.endGroup();

    settings.beginGroup(QSL("Browser-Tabs-Settings"));
    newTabPosition = settings.value(QSL("OpenNewTabsSelected"), false).toBool() ? Qz::NT_CleanSelectedTab : Qz::NT_CleanNotSelectedTab;
    tabsOnTop = settings.value(QSL("TabsOnTop"), true).toBool();
    openPopupsInTabs = settings.value(QSL("OpenPopupsInTabs"), false).toBool();
    alwaysSwitchTabsWithWheel = settings.value(QSL("AlwaysSwitchTabsWithWheel"), false).toBool();
    settings.endGroup();
}

void QzSettings::saveSettings()
{
    Settings settings;
    settings.beginGroup(QSL("Web-Browser-Settings"));
    settings.setValue(QSL("AutomaticallyOpenProtocols"), autoOpenProtocols);
    settings.setValue(QSL("BlockOpeningProtocols"), blockedProtocols);
    settings.endGroup();

    settings.beginGroup(QSL("Browser-Tabs-Settings"));
    settings.setValue(QSL("TabsOnTop"), tabsOnTop);
    settings.endGroup();
}
