/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2016 David Rosca <nowrep@gmail.com>
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
#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QtPlugin>
#include <QWebEnginePage>

#include "qzcommon.h"
#include "webhittestresult.h"

class QMenu;
class QMouseEvent;
class QKeyEvent;
class QWheelEvent;

class WebView;
class WebPage;
class DesktopFile;

class PluginInterface
{
public:
    enum InitState {
        StartupInitState,
        LateInitState
    };

    virtual ~PluginInterface() = default;

    virtual void init(InitState state, const QString &settingsPath) = 0;
    virtual void unload() = 0;
    virtual bool testPlugin() = 0;

    virtual void showSettings(QWidget* parent = 0) { Q_UNUSED(parent) }

    virtual void populateWebViewMenu(QMenu* menu, WebView* view, const WebHitTestResult &r) { Q_UNUSED(menu) Q_UNUSED(view) Q_UNUSED(r) }
    virtual void populateExtensionsMenu(QMenu *menu) { Q_UNUSED(menu) }

    virtual bool mouseDoubleClick(Qz::ObjectName type, QObject* obj, QMouseEvent* event) { Q_UNUSED(type) Q_UNUSED(obj) Q_UNUSED(event) return false; }
    virtual bool mousePress(Qz::ObjectName type, QObject* obj, QMouseEvent* event) { Q_UNUSED(type) Q_UNUSED(obj) Q_UNUSED(event) return false; }
    virtual bool mouseRelease(Qz::ObjectName type, QObject* obj, QMouseEvent* event) { Q_UNUSED(type) Q_UNUSED(obj) Q_UNUSED(event) return false; }
    virtual bool mouseMove(Qz::ObjectName type, QObject* obj, QMouseEvent* event) { Q_UNUSED(type) Q_UNUSED(obj) Q_UNUSED(event) return false; }

    virtual bool wheelEvent(Qz::ObjectName type, QObject* obj, QWheelEvent* event) { Q_UNUSED(type) Q_UNUSED(obj) Q_UNUSED(event) return false; }

    virtual bool keyPress(Qz::ObjectName type, QObject* obj, QKeyEvent* event) { Q_UNUSED(type) Q_UNUSED(obj) Q_UNUSED(event) return false; }
    virtual bool keyRelease(Qz::ObjectName type, QObject* obj, QKeyEvent* event) { Q_UNUSED(type) Q_UNUSED(obj) Q_UNUSED(event) return false; }

    virtual bool acceptNavigationRequest(WebPage *page, const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) { Q_UNUSED(page); Q_UNUSED(url); Q_UNUSED(type); Q_UNUSED(isMainFrame); return true; }
};

Q_DECLARE_INTERFACE(PluginInterface, "Falkon.Browser.PluginInterface/2.4")

#endif // PLUGININTERFACE_H
