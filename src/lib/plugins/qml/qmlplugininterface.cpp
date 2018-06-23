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
#include "api/menus/qmlmenu.h"
#include "api/menus/qmlwebhittestresult.h"
#include <QDebug>

QmlPluginInterface::QmlPluginInterface()
    : m_browserAction(nullptr)
    , m_sideBar(nullptr)
{
}

void QmlPluginInterface::init(InitState state, const QString &settingsPath)
{
    if (!m_init.isCallable()) {
        qWarning() << "Unable to call" << __FUNCTION__ << "on" << m_name << "plugin";
        return;
    }

    QJSValueList args;
    args.append(state);
    args.append(settingsPath);
    m_init.call(args);

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
    if (!m_unload.isCallable()) {
        qWarning() << "Unable to call" << __FUNCTION__ << "on" << m_name << "plugin";
        return;
    }

    m_unload.call();

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
    if (!m_testPlugin.isCallable()) {
        qWarning() << "Unable to call" << __FUNCTION__ << "on" << m_name << "plugin";
        return false;
    }

    QJSValue ret = m_testPlugin.call();
    return ret.toBool();
}

void QmlPluginInterface::populateWebViewMenu(QMenu *menu, WebView *webview, const WebHitTestResult &webHitTestResult)
{
    Q_UNUSED(webview)

    if (!m_populateWebViewMenu.isCallable()) {
        return;
    }

    QmlMenu *qmlMenu = new QmlMenu(menu);
    QmlWebHitTestResult *qmlWebHitTestResult = new QmlWebHitTestResult(webHitTestResult);

    QJSValueList args;
    args.append(m_engine->newQObject(qmlMenu));
    args.append(m_engine->newQObject(qmlWebHitTestResult));
    m_populateWebViewMenu.call(args);
    menu->addSeparator();

    qmlMenu->deleteLater();
    qmlWebHitTestResult->deleteLater();
}

QJSValue QmlPluginInterface::readInit() const
{
    return m_init;
}

void QmlPluginInterface::setInit(const QJSValue &init)
{
    m_init = init;
}

QJSValue QmlPluginInterface::readUnload() const
{
    return m_unload;
}

void QmlPluginInterface::setUnload(const QJSValue &unload)
{
    m_unload = unload;
}

QJSValue QmlPluginInterface::readTestPlugin() const
{
    return m_testPlugin;
}

void QmlPluginInterface::setTestPlugin(const QJSValue &testPlugin)
{
    m_testPlugin = testPlugin;
}

void QmlPluginInterface::setEngine(QQmlEngine *engine)
{
    m_engine = engine;
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

QJSValue QmlPluginInterface::readPopulateWebViewMenu() const
{
    return m_populateWebViewMenu;
}

void QmlPluginInterface::setPopulateWebViewMenu(const QJSValue &value)
{
    m_populateWebViewMenu = value;
}

QQmlListProperty<QObject> QmlPluginInterface::childItems()
{
    return QQmlListProperty<QObject>(this, m_childItems);
}

void QmlPluginInterface::addButton(BrowserWindow *window)
{
    if (m_browserAction->location().testFlag(QmlBrowserAction::NavigationToolBar)) {
        window->navigationBar()->addToolButton(m_browserAction->button());
    }

    if (m_browserAction->location().testFlag(QmlBrowserAction::StatusBar)) {
        window->statusBar()->addButton(m_browserAction->button());
    }
}

void QmlPluginInterface::removeButton(BrowserWindow *window)
{
    if (m_browserAction->location().testFlag(QmlBrowserAction::NavigationToolBar)) {
        window->navigationBar()->removeToolButton(m_browserAction->button());
    }

    if (m_browserAction->location().testFlag(QmlBrowserAction::StatusBar)) {
        window->statusBar()->removeButton(m_browserAction->button());
    }
}
