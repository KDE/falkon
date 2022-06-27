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
#include "api/events/qmlqzobjects.h"
#include "api/events/qmlmouseevent.h"
#include "api/events/qmlwheelevent.h"
#include "api/events/qmlkeyevent.h"
#include "api/tabs/qmltab.h"
#include "webpage.h"
#include "qztools.h"
#include "qml/qmlengine.h"
#include <QDebug>
#include <QQuickWidget>
#include <QDialog>
#include <QVBoxLayout>

QmlPluginInterface::QmlPluginInterface()
    : m_qmlReusableTab(new QmlTab())
{
}

QmlPluginInterface::~QmlPluginInterface()
{
    m_qmlReusableTab->deleteLater();
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
}

void QmlPluginInterface::unload()
{
    if (!m_unload.isCallable()) {
        qWarning() << "Unable to call" << __FUNCTION__ << "on" << m_name << "plugin";
        return;
    }

    m_unload.call();

    for (QObject *childItem : qAsConst(m_childItems)) {
        childItem->deleteLater();
    }

    Q_EMIT qmlPluginUnloaded();
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

    auto *qmlMenu = new QmlMenu(menu, m_engine);
    auto *qmlWebHitTestResult = new QmlWebHitTestResult(webHitTestResult);
    QJSValueList args;
    args.append(m_engine->newQObject(qmlMenu));
    args.append(m_engine->newQObject(qmlWebHitTestResult));
    m_populateWebViewMenu.call(args);
    menu->addSeparator();
}

void QmlPluginInterface::showSettings(QWidget *parent)
{
    if (!m_settingsWindow) {
        qWarning() << "No dialog to show";
        return;
    }

    auto *widget = new QQuickWidget();
    widget->setContent(m_settingsWindow->url(), m_settingsWindow, m_settingsWindow->create(m_settingsWindow->creationContext()));
    widget->show();

    QzTools::centerWidgetToParent(widget, parent);
}

bool QmlPluginInterface::mouseDoubleClick(Qz::ObjectName type, QObject *obj, QMouseEvent *event)
{
    Q_UNUSED(obj)
    if (!m_mouseDoubleClick.isCallable()) {
        return false;
    }
    auto qmlMouseEvent = new QmlMouseEvent(event);
    QJSValueList args;
    args.append(QmlQzObjects::ObjectName(type));
    args.append(m_engine->newQObject(qmlMouseEvent));
    m_mouseDoubleClick.call(args);
    qmlMouseEvent->clear();
    return false;
}

bool QmlPluginInterface::mousePress(Qz::ObjectName type, QObject *obj, QMouseEvent *event)
{
    Q_UNUSED(obj)
    if (!m_mousePress.isCallable()) {
        return false;
    }
    auto qmlMouseEvent = new QmlMouseEvent(event);
    QJSValueList args;
    args.append(QmlQzObjects::ObjectName(type));
    args.append(m_engine->newQObject(qmlMouseEvent));
    m_mousePress.call(args);
    qmlMouseEvent->clear();
    return false;
}

bool QmlPluginInterface::mouseRelease(Qz::ObjectName type, QObject *obj, QMouseEvent *event)
{
    Q_UNUSED(obj)
    if (!m_mouseRelease.isCallable()) {
        return false;
    }
    auto qmlMouseEvent = new QmlMouseEvent(event);
    QJSValueList args;
    args.append(QmlQzObjects::ObjectName(type));
    args.append(m_engine->newQObject(qmlMouseEvent));
    m_mouseRelease.call(args);
    qmlMouseEvent->clear();
    return false;
}

bool QmlPluginInterface::mouseMove(Qz::ObjectName type, QObject *obj, QMouseEvent *event)
{
    Q_UNUSED(obj)
    if (!m_mouseMove.isCallable()) {
        return false;
    }
    auto qmlMouseEvent = new QmlMouseEvent(event);
    QJSValueList args;
    args.append(QmlQzObjects::ObjectName(type));
    args.append(m_engine->newQObject(qmlMouseEvent));
    m_mouseMove.call(args);
    qmlMouseEvent->clear();
    return false;
}

bool QmlPluginInterface::wheelEvent(Qz::ObjectName type, QObject *obj, QWheelEvent *event)
{
    Q_UNUSED(obj)
    if (!m_wheelEvent.isCallable()) {
        return false;
    }
    auto qmlWheelEvent = new QmlWheelEvent(event);
    QJSValueList args;
    args.append(QmlQzObjects::ObjectName(type));
    args.append(m_engine->newQObject(qmlWheelEvent));
    m_wheelEvent.call(args);
    qmlWheelEvent->clear();
    return false;
}

bool QmlPluginInterface::keyPress(Qz::ObjectName type, QObject *obj, QKeyEvent *event)
{
    Q_UNUSED(obj)
    if (!m_keyPress.isCallable()) {
        return false;
    }
    auto qmlKeyEvent = new QmlKeyEvent(event);
    QJSValueList args;
    args.append(QmlQzObjects::ObjectName(type));
    args.append(m_engine->newQObject(qmlKeyEvent));
    m_keyPress.call(args);
    qmlKeyEvent->clear();
    return false;
}

bool QmlPluginInterface::keyRelease(Qz::ObjectName type, QObject *obj, QKeyEvent *event)
{
    Q_UNUSED(obj)
    if (!m_keyRelease.isCallable()) {
        return false;
    }
    auto qmlKeyEvent = new QmlKeyEvent(event);
    QJSValueList args;
    args.append(QmlQzObjects::ObjectName(type));
    args.append(m_engine->newQObject(qmlKeyEvent));
    m_keyRelease.call(args);
    qmlKeyEvent->clear();
    return false;
}

bool QmlPluginInterface::acceptNavigationRequest(WebPage *page, const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    if (!m_acceptNavigationRequest.isCallable()) {
        return true;
    }
    m_qmlReusableTab->setWebPage(page);
    QJSValueList args;
    args.append(m_engine->newQObject(m_qmlReusableTab));
    args.append(QString::fromUtf8(url.toEncoded()));
    args.append(type);
    args.append(isMainFrame);
    return m_acceptNavigationRequest.call(args).toBool();
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

QJSValue QmlPluginInterface::readPopulateWebViewMenu() const
{
    return m_populateWebViewMenu;
}

void QmlPluginInterface::setPopulateWebViewMenu(const QJSValue &value)
{
    m_populateWebViewMenu = value;
}

QQmlComponent *QmlPluginInterface::settingsWindow() const
{
    return m_settingsWindow;
}

void QmlPluginInterface::setSettingsWindow(QQmlComponent *settingsWindow)
{
    m_settingsWindow = settingsWindow;
}

QJSValue QmlPluginInterface::readMouseDoubleClick() const
{
    return m_mouseDoubleClick;
}

void QmlPluginInterface::setMouseDoubleClick(const QJSValue &mouseDoubleClick)
{
    m_mouseDoubleClick = mouseDoubleClick;
    mApp->plugins()->registerAppEventHandler(PluginProxy::MouseDoubleClickHandler, this);
}

QJSValue QmlPluginInterface::readMousePress() const
{
    return m_mousePress;
}

void QmlPluginInterface::setMousePress(const QJSValue &mousePress)
{
    m_mousePress = mousePress;
    mApp->plugins()->registerAppEventHandler(PluginProxy::MousePressHandler, this);
}

QJSValue QmlPluginInterface::readMouseRelease() const
{
    return m_mouseRelease;
}

void QmlPluginInterface::setMouseRelease(const QJSValue &mouseRelease)
{
    m_mouseRelease = mouseRelease;
    mApp->plugins()->registerAppEventHandler(PluginProxy::MouseReleaseHandler, this);
}

QJSValue QmlPluginInterface::readMouseMove() const
{
    return m_mouseMove;
}

void QmlPluginInterface::setMouseMove(const QJSValue &mouseMove)
{
    m_mouseMove = mouseMove;
    mApp->plugins()->registerAppEventHandler(PluginProxy::MouseMoveHandler, this);
}

QJSValue QmlPluginInterface::readWheelEvent() const
{
    return m_wheelEvent;
}

void QmlPluginInterface::setWheelEvent(const QJSValue &wheelEvent)
{
    m_wheelEvent = wheelEvent;
    mApp->plugins()->registerAppEventHandler(PluginProxy::WheelEventHandler, this);
}

QJSValue QmlPluginInterface::readKeyPress() const
{
    return m_keyPress;
}

void QmlPluginInterface::setKeyPress(const QJSValue &keyPress)
{
    m_keyPress = keyPress;
    mApp->plugins()->registerAppEventHandler(PluginProxy::KeyPressHandler, this);
}

QJSValue QmlPluginInterface::readKeyRelease() const
{
    return m_keyRelease;
}

void QmlPluginInterface::setKeyRelease(const QJSValue &keyRelease)
{
    m_keyRelease = keyRelease;
    mApp->plugins()->registerAppEventHandler(PluginProxy::KeyReleaseHandler, this);
}

QJSValue QmlPluginInterface::readAcceptNavigationRequest() const
{
    return m_acceptNavigationRequest;
}

void QmlPluginInterface::setAcceptNavigationRequest(const QJSValue &acceptNavigationRequest)
{
    m_acceptNavigationRequest = acceptNavigationRequest;
}

QQmlListProperty<QObject> QmlPluginInterface::childItems()
{
    return QQmlListProperty<QObject>(this, m_childItems);
}
