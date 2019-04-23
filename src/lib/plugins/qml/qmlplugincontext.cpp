/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 David Rosca <nowrep@gmail.com>
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
#include "qmlplugincontext.h"

#include <QtQml>
#include <QFileInfo>

QmlPluginContext::QmlPluginContext(const Plugins::Plugin &plugin, QQmlEngine *engine, QObject *parent)
    : QQmlContext(engine, parent)
    , m_plugin(plugin)
{
}

QString QmlPluginContext::pluginPath() const
{
    return m_plugin.pluginPath;
}

QString QmlPluginContext::pluginName() const
{
    return QFileInfo(m_plugin.pluginPath).fileName();
}

Plugins::Plugin QmlPluginContext::plugin() const
{
    return m_plugin;
}

// static
QmlPluginContext *QmlPluginContext::contextForObject(const QObject *object)
{
    QQmlContext *c = qmlContext(object);
    while (c) {
        QmlPluginContext *p = qobject_cast<QmlPluginContext*>(c);
        if (p) {
            return p;
        }
        c = c->parentContext();
    }
    qCritical() << "Failed to get plugin context for object" << object;
    Q_UNREACHABLE();
    return nullptr;
}
