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

#include "desktopfile.h"
#include "plugininterface.h"
#include "api/browseraction/qmlbrowseraction.h"
#include "api/sidebar/qmlsidebar.h"

class QmlPluginInterface : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_ENUMS(InitState)
    Q_PROPERTY(QJSValue init READ readInit WRITE setInit)
    Q_PROPERTY(QJSValue unload READ readUnload WRITE setUnload)
    Q_PROPERTY(QJSValue testPlugin READ readTestPlugin WRITE setTestPlugin)
    Q_PROPERTY(QmlBrowserAction* browserAction READ browserAction WRITE setBrowserAction)
    Q_PROPERTY(QmlSideBar* sideBar READ sideBar WRITE setSideBar)
    Q_PROPERTY(QJSValue populateWebViewMenu READ readPopulateWebViewMenu WRITE setPopulateWebViewMenu)
    Q_PROPERTY(QQmlListProperty<QObject> childItems READ childItems)
    Q_CLASSINFO("DefaultProperty", "childItems")

public:
    explicit QmlPluginInterface();
    DesktopFile metaData() const;
    void init(InitState state, const QString &settingsPath);
    void unload();
    bool testPlugin();
    void setEngine(QQmlEngine *engine);
    void setName(const QString &name);
    void populateWebViewMenu(QMenu *menu, WebView *webview, const WebHitTestResult &webHitTestResult) override;

Q_SIGNALS:
    void qmlPluginUnloaded();

private:
    QQmlEngine *m_engine;
    QString m_name;
    QJSValue m_init;
    QJSValue m_unload;
    QJSValue m_testPlugin;
    QmlBrowserAction *m_browserAction;
    QmlSideBar *m_sideBar;
    QJSValue m_populateWebViewMenu;
    QList<QObject*> m_childItems;

    QJSValue readInit() const;
    void setInit(const QJSValue &init);
    QJSValue readUnload() const;
    void setUnload(const QJSValue &unload);
    QJSValue readTestPlugin() const;
    void setTestPlugin(const QJSValue &testPlugin);
    QmlBrowserAction *browserAction() const;
    void setBrowserAction(QmlBrowserAction *browserAction);
    QmlSideBar *sideBar() const;
    void setSideBar(QmlSideBar *sideBar);
    QJSValue readPopulateWebViewMenu() const;
    void setPopulateWebViewMenu(const QJSValue &value);
    QQmlListProperty<QObject> childItems();

    void addButton(BrowserWindow *window);
    void removeButton(BrowserWindow *window);
};
