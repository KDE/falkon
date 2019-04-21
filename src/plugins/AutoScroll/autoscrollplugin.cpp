/* ============================================================
* AutoScroll - Autoscroll for Falkon
* Copyright (C) 2014-2017 David Rosca <nowrep@gmail.com>
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
#include "autoscrollplugin.h"
#include "autoscrollsettings.h"
#include "autoscroller.h"
#include "browserwindow.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "../config.h"

AutoScrollPlugin::AutoScrollPlugin()
    : QObject()
    , m_scroller(0)
{
}

void AutoScrollPlugin::init(InitState state, const QString &settingsPath)
{
    Q_UNUSED(state)

    m_scroller = new AutoScroller(settingsPath + QL1S("/extensions.ini"), this);

    mApp->plugins()->registerAppEventHandler(PluginProxy::MouseMoveHandler, this);
    mApp->plugins()->registerAppEventHandler(PluginProxy::MousePressHandler, this);
    mApp->plugins()->registerAppEventHandler(PluginProxy::MouseReleaseHandler, this);
    mApp->plugins()->registerAppEventHandler(PluginProxy::WheelEventHandler, this);
}

void AutoScrollPlugin::unload()
{
    m_scroller->deleteLater();
}

bool AutoScrollPlugin::testPlugin()
{
    // Require the version that the plugin was built with
    return (Qz::VERSION == QLatin1String(FALKON_VERSION));
}

void AutoScrollPlugin::showSettings(QWidget* parent)
{
    if (!m_settings) {
        m_settings = new AutoScrollSettings(m_scroller, parent);
    }

    m_settings.data()->show();
    m_settings.data()->raise();
}

bool AutoScrollPlugin::mouseMove(Qz::ObjectName type, QObject* obj, QMouseEvent* event)
{
    if (type == Qz::ON_WebView) {
        return m_scroller->mouseMove(obj, event);
    }

    return false;
}

bool AutoScrollPlugin::mousePress(Qz::ObjectName type, QObject* obj, QMouseEvent* event)
{
    if (type == Qz::ON_WebView) {
        return m_scroller->mousePress(obj, event);
    }

    return false;
}

bool AutoScrollPlugin::mouseRelease(Qz::ObjectName type, QObject* obj, QMouseEvent* event)
{
    if (type == Qz::ON_WebView) {
        return m_scroller->mouseRelease(obj, event);
    }

    return false;
}

bool AutoScrollPlugin::wheelEvent(Qz::ObjectName type, QObject *obj, QWheelEvent *event)
{
    if (type == Qz::ON_WebView) {
        return m_scroller->wheel(obj, event);
    }

    return false;
}
