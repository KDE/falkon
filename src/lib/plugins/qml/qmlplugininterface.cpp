/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmlplugininterface.h"
#include "mainapplication.h"
#include "pluginproxy.h"
#include "statusbar.h"
#include "browserwindow.h"
#include "navigationbar.h"
#include "sidebar.h"
#include <QDebug>

QmlPluginInterface::QmlPluginInterface()
    : m_browserAction(nullptr)
    , m_sideBar(nullptr)
{
}

void QmlPluginInterface::init(InitState state, const QString &settingsPath)
{
    if (!m_jsInit.isCallable()) {
        qWarning() << "Unable to call" << __FUNCTION__ << "on" << m_name << "plugin";
        return;
    }

    QJSValueList args;
    args.append(state);
    args.append(settingsPath);
    m_jsInit.call(args);

    if (m_browserAction) {
        for (BrowserWindow *window : mApp->windows()) {
            addButton(window);
        }

        connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, &QmlPluginInterface::addButton);
    }

    if (m_sideBar) {
        SideBarManager::addSidebar(m_sideBar->name(), m_sideBar->sideBar());
    }
}

DesktopFile QmlPluginInterface::metaData() const
{
    return DesktopFile();
}

void QmlPluginInterface::unload()
{
    if (!m_jsUnload.isCallable()) {
        qWarning() << "Unable to call" << __FUNCTION__ << "on" << m_name << "plugin";
        return;
    }

    m_jsUnload.call();

    if (m_browserAction) {
        for (BrowserWindow *window : mApp->windows()) {
            removeButton(window);
        }

        disconnect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, &QmlPluginInterface::addButton);
    }

    if (m_sideBar) {
        SideBarManager::removeSidebar(m_sideBar->sideBar());
    }

    emit qmlPluginUnloaded();
}

bool QmlPluginInterface::testPlugin()
{
    if (!m_jsTestPlugin.isCallable()) {
        qWarning() << "Unable to call" << __FUNCTION__ << "on" << m_name << "plugin";
        return false;
    }

    QJSValue ret = m_jsTestPlugin.call();
    return ret.toBool();
}

QJSValue QmlPluginInterface::jsInit() const
{
    return m_jsInit;
}

void QmlPluginInterface::setJsInit(const QJSValue &init)
{
    m_jsInit = init;
}

QJSValue QmlPluginInterface::jsUnload() const
{
    return m_jsUnload;
}

void QmlPluginInterface::setJsUnload(const QJSValue &unload)
{
    m_jsUnload = unload;
}

QJSValue QmlPluginInterface::jsTestPlugin() const
{
    return m_jsTestPlugin;
}

void QmlPluginInterface::setJsTestPlugin(const QJSValue &testPlugin)
{
    m_jsTestPlugin = testPlugin;
}

void QmlPluginInterface::setName(const QString &name)
{
    m_name = name;
}

QmlBrowserAction *QmlPluginInterface::browserAction() const
{
    return m_browserAction;
}

void QmlPluginInterface::setBrowserAction(QmlBrowserAction *browserAction)
{
    m_browserAction = browserAction;
}

QmlSideBar *QmlPluginInterface::sideBar() const
{
    return m_sideBar;
}

void QmlPluginInterface::setSideBar(QmlSideBar *sideBar)
{
    m_sideBar = sideBar;
}

void QmlPluginInterface::addButton(BrowserWindow *window)
{
    if (m_browserAction->location().testFlag(QmlBrowserAction::NavigationToolBar)) {
        window->navigationBar()->addToolButton(m_browserAction);
    }

    if (m_browserAction->location().testFlag(QmlBrowserAction::StatusBar)) {
        window->statusBar()->addButton(m_browserAction);
    }
}

void QmlPluginInterface::removeButton(BrowserWindow *window)
{
    if (m_browserAction->location().testFlag(QmlBrowserAction::NavigationToolBar)) {
        window->navigationBar()->removeToolButton(m_browserAction);
    }

    if (m_browserAction->location().testFlag(QmlBrowserAction::StatusBar)) {
        window->statusBar()->removeButton(m_browserAction);
    }
}
