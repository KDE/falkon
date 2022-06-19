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
#include "PIM_plugin.h"
#include "PIM_handler.h"
#include "PIM_settings.h"
#include "mainapplication.h"
#include "pluginproxy.h"
#include "browserwindow.h"
#include "webview.h"
#include "../config.h"

PIM_Plugin::PIM_Plugin()
    : QObject()
    , m_handler(0)
{
}

void PIM_Plugin::init(InitState state, const QString &settingsPath)
{
    Q_UNUSED(state)

    m_handler = new PIM_Handler(settingsPath, this);

    mApp->plugins()->registerAppEventHandler(PluginProxy::KeyPressHandler, this);

    connect(mApp->plugins(), SIGNAL(webPageCreated(WebPage*)), m_handler, SLOT(webPageCreated(WebPage*)));
}

void PIM_Plugin::unload()
{
    m_handler->unloadPlugin();
    m_handler->deleteLater();
}

bool PIM_Plugin::testPlugin()
{
    // Require the version that the plugin was built with
    return (Qz::VERSION == QLatin1String(FALKON_VERSION));
}

void PIM_Plugin::showSettings(QWidget* parent)
{
    m_handler->showSettings(parent);
}

void PIM_Plugin::populateWebViewMenu(QMenu* menu, WebView* view, const WebHitTestResult &r)
{
    m_handler->populateWebViewMenu(menu, view, r);
}

bool PIM_Plugin::keyPress(Qz::ObjectName type, QObject* obj, QKeyEvent* event)
{
    if (type == Qz::ON_WebView) {
        auto* view = qobject_cast<WebView*>(obj);
        return m_handler->keyPress(view, event);
    }

    return false;
}
