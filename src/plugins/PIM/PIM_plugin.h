/* ============================================================
* Personal Information Manager plugin for Falkon
* Copyright (C) 2012-2014  David Rosca <nowrep@gmail.com>
* Copyright (C) 2012-2014  Mladen Pejaković <pejakm@autistici.org>
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
#ifndef PIM_PLUGIN_H
#define PIM_PLUGIN_H

#include "plugininterface.h"

class WebPage;
class BrowserWindow;
class PIM_Handler;

class PIM_Plugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.PIM" FILE "PIM.json")

public:
    PIM_Plugin();

    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;
    void showSettings(QWidget *parent) override;

    void populateWebViewMenu(QMenu *menu, WebView *view, const WebHitTestResult &r) override;
    bool keyPress(Qz::ObjectName type, QObject *obj, QKeyEvent *event) override;

private:
    PIM_Handler* m_handler;
};

#endif // PIM_PLUGIN_H
