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

#ifndef SITESETTINGSVIEW_PLUGIN_H
#define SITESETTINGSVIEW_PLUGIN_H

#include "plugininterface.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPointer>

class SiteSettingsView_Controller;

class SiteSettingsView : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.SiteSettingsView" FILE "sitesettingsview.json")

public:
    explicit SiteSettingsView();

    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;

private Q_SLOTS:

private:
    WebView* m_view;
    SiteSettingsView_Controller *m_sideBar = nullptr;
};

#endif // SITESETTINGSVIEW_PLUGIN_H
