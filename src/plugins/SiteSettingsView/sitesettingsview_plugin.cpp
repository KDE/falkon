/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2024 Juraj Oravec <jurajoravec@mailo.com>
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

#include "sitesettingsview_plugin.h"
#include "sitesettingsview_controller.h"
#include "browserwindow.h"
#include "webview.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "sidebar.h"
#include "webhittestresult.h"
#include "../config.h"

#include <QMenu>
#include <QPushButton>

SiteSettingsView::SiteSettingsView()
    : QObject()
    , m_view(nullptr)
{
}

void SiteSettingsView::init(InitState state, const QString &settingsPath)
{
    Q_UNUSED(settingsPath)

    // State can be either StartupInitState or LateInitState, and it
    // indicates when the plugin have been loaded.
    // Currently, it can be from preferences, or automatically at startup.
    // Plugins are loaded before first BrowserWindow is created.
    Q_UNUSED(state)

    // Adding new sidebar into application
    m_sideBar = new SiteSettingsView_Controller(this);
    SideBarManager::addSidebar(QSL("sitesettingsview-sidebar"), m_sideBar);

    connect(mApp->plugins(), &PluginProxy::webPageCreated, m_sideBar, &SiteSettingsView_Controller::webPageCreated);
    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, m_sideBar, &SiteSettingsView_Controller::mainWindowDeleted);
}

void SiteSettingsView::unload()
{
    // Removing sidebar from application
    SideBarManager::removeSidebar(m_sideBar);
    delete m_sideBar;
}

bool SiteSettingsView::testPlugin()
{
    return (QString::fromLatin1(Qz::VERSION) == QLatin1String(FALKON_VERSION));
}
