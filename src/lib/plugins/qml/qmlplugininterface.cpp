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
#include <QDebug>

QmlPluginInterface::QmlPluginInterface()
{
}

void QmlPluginInterface::init(InitState state, const QString &settingsPath)
{
    if (m_jsInit.isCallable()) {
        QJSValueList args;
        args.append(state);
        args.append(settingsPath);
        m_jsInit.call(args);
    } else {
        qWarning() << "Unable to call init on" << m_name << "plugin";
    }
}

DesktopFile QmlPluginInterface::metaData() const
{
    return DesktopFile();
}

void QmlPluginInterface::unload()
{
    if (m_jsUnload.isCallable()) {
        m_jsUnload.call();
    } else {
        qWarning() << "Unable to call unload on" << m_name << "plugin";
    }
}

bool QmlPluginInterface::testPlugin()
{
    if (m_jsTestPlugin.isCallable()) {
        QJSValue ret = m_jsTestPlugin.call();
        return ret.toBool();
    } else {
        qWarning() << "Unable to call testPlugin on" << m_name << "plugin";
        return false;
    }
}

QString QmlPluginInterface::name()
{
    return m_name;
}

void QmlPluginInterface::setName(const QString &name)
{
    m_name = name;
}

QJSValue QmlPluginInterface::jsInit()
{
    return m_jsInit;
}

void QmlPluginInterface::setJsInit(const QJSValue &init)
{
    m_jsInit = init;
}

QJSValue QmlPluginInterface::jsUnload()
{
    return m_jsUnload;
}

void QmlPluginInterface::setJsUnload(const QJSValue &unload)
{
    m_jsUnload = unload;
}

QJSValue QmlPluginInterface::jsTestPlugin()
{
    return m_jsTestPlugin;
}

void QmlPluginInterface::setJsTestPlugin(const QJSValue &testPlugin)
{
    m_jsTestPlugin = testPlugin;
}
