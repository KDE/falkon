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
    Q_PROPERTY(QJSValue init READ jsInit WRITE setJsInit)
    Q_PROPERTY(QJSValue unload READ jsUnload WRITE setJsUnload)
    Q_PROPERTY(QJSValue testPlugin READ jsTestPlugin WRITE setJsTestPlugin)
    Q_PROPERTY(QmlBrowserAction* browserAction READ browserAction WRITE setBrowserAction)
    Q_PROPERTY(QmlSideBar* sideBar READ sideBar WRITE setSideBar)

public:
    explicit QmlPluginInterface();
    DesktopFile metaData() const;
    void init(InitState state, const QString &settingsPath);
    void unload();
    bool testPlugin();
    void setName(const QString &name);

Q_SIGNALS:
    void qmlPluginUnloaded();

private:
    QString m_name;
    QJSValue m_jsInit;
    QJSValue m_jsUnload;
    QJSValue m_jsTestPlugin;
    QmlBrowserAction *m_browserAction;
    QmlSideBar *m_sideBar;
    bool loaded;

    QJSValue jsInit() const;
    void setJsInit(const QJSValue &init);
    QJSValue jsUnload() const;
    void setJsUnload(const QJSValue &unload);
    QJSValue jsTestPlugin() const;
    void setJsTestPlugin(const QJSValue &testPlugin);
    QmlBrowserAction *browserAction() const;
    void setBrowserAction(QmlBrowserAction *browserAction);
    QmlSideBar *sideBar() const;
    void setSideBar(QmlSideBar *sideBar);

    void addButton(BrowserWindow *window);
    void removeButton(BrowserWindow *window);
};
