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
#pragma once

#include <QJSValue>
#include <QObject>
#include <QQmlComponent>

#include "desktopfile.h"
#include "plugininterface.h"

class QmlTab;

class QmlPluginInterface : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_ENUM(InitState)
    Q_PROPERTY(QJSValue init READ readInit WRITE setInit)
    Q_PROPERTY(QJSValue unload READ readUnload WRITE setUnload)
    Q_PROPERTY(QJSValue testPlugin READ readTestPlugin WRITE setTestPlugin)
    Q_PROPERTY(QJSValue populateWebViewMenu READ readPopulateWebViewMenu WRITE setPopulateWebViewMenu)
    Q_PROPERTY(QQmlComponent* settingsWindow READ settingsWindow WRITE setSettingsWindow)
    Q_PROPERTY(QJSValue mouseDoubleClick READ readMouseDoubleClick WRITE setMouseDoubleClick)
    Q_PROPERTY(QJSValue mousePress READ readMousePress WRITE setMousePress)
    Q_PROPERTY(QJSValue mouseRelease READ readMouseRelease WRITE setMouseRelease)
    Q_PROPERTY(QJSValue mouseMove READ readMouseMove WRITE setMouseMove)
    Q_PROPERTY(QJSValue wheelEvent READ readWheelEvent WRITE setWheelEvent)
    Q_PROPERTY(QJSValue keyPress READ readKeyPress WRITE setKeyPress)
    Q_PROPERTY(QJSValue keyRelease READ readKeyRelease WRITE setKeyRelease)
    Q_PROPERTY(QJSValue acceptNavigationRequest READ readAcceptNavigationRequest WRITE setAcceptNavigationRequest)
    Q_PROPERTY(QQmlListProperty<QObject> childItems READ childItems)
    Q_CLASSINFO("DefaultProperty", "childItems")

public:
    explicit QmlPluginInterface();
    ~QmlPluginInterface() override;
    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;
    void setEngine(QQmlEngine *engine);
    void setName(const QString &name);
    void populateWebViewMenu(QMenu *menu, WebView *webview, const WebHitTestResult &webHitTestResult) override;
    void showSettings(QWidget *parent = nullptr) override;

    bool mouseDoubleClick(Qz::ObjectName type, QObject *obj, QMouseEvent *event) override;
    bool mousePress(Qz::ObjectName type, QObject *obj, QMouseEvent *event) override;
    bool mouseRelease(Qz::ObjectName type, QObject *obj, QMouseEvent *event) override;
    bool mouseMove(Qz::ObjectName type, QObject *obj, QMouseEvent *event) override;

    bool wheelEvent(Qz::ObjectName type, QObject *obj, QWheelEvent *event) override;

    bool keyPress(Qz::ObjectName type, QObject *obj, QKeyEvent *event) override;
    bool keyRelease(Qz::ObjectName type, QObject *obj, QKeyEvent *event) override;

    bool acceptNavigationRequest(WebPage *page, const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;

Q_SIGNALS:
    void qmlPluginUnloaded();

private:
    QQmlEngine *m_engine = nullptr;
    QString m_name;
    QJSValue m_init;
    QJSValue m_unload;
    QJSValue m_testPlugin;
    QJSValue m_populateWebViewMenu;
    QQmlComponent *m_settingsWindow = nullptr;
    QJSValue m_mouseDoubleClick;
    QJSValue m_mousePress;
    QJSValue m_mouseRelease;
    QJSValue m_mouseMove;
    QJSValue m_wheelEvent;
    QJSValue m_keyPress;
    QJSValue m_keyRelease;
    QJSValue m_acceptNavigationRequest;
    QList<QObject*> m_childItems;
    QmlTab *m_qmlReusableTab = nullptr;
    QJSValue readInit() const;
    void setInit(const QJSValue &init);
    QJSValue readUnload() const;
    void setUnload(const QJSValue &unload);
    QJSValue readTestPlugin() const;
    void setTestPlugin(const QJSValue &testPlugin);
    QJSValue readPopulateWebViewMenu() const;
    void setPopulateWebViewMenu(const QJSValue &value);
    QQmlComponent *settingsWindow() const;
    void setSettingsWindow(QQmlComponent *settingsWindow);
    QJSValue readMouseDoubleClick() const;
    void setMouseDoubleClick(const QJSValue &mouseDoubleClick);
    QJSValue readMousePress() const;
    void setMousePress(const QJSValue &mousePress);
    QJSValue readMouseRelease() const;
    void setMouseRelease(const QJSValue &mouseRelease);
    QJSValue readMouseMove() const;
    void setMouseMove(const QJSValue &mouseMove);
    QJSValue readWheelEvent() const;
    void setWheelEvent(const QJSValue &wheelEvent);
    QJSValue readKeyPress() const;
    void setKeyPress(const QJSValue &keyPress);
    QJSValue readKeyRelease() const;
    void setKeyRelease(const QJSValue &keyRelease);
    QJSValue readAcceptNavigationRequest() const;
    void setAcceptNavigationRequest(const QJSValue &acceptNavigationRequest);
    QQmlListProperty<QObject> childItems();
};
