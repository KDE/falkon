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
#include "qmlplugin.h"
#include "qmlplugins.h"
#include "qmlpluginloader.h"
#include "datapaths.h"
#include "desktopfile.h"

#include <QFileInfo>
#include <QDir>

QmlPlugin::QmlPlugin()
= default;

Plugins::Plugin QmlPlugin::loadPlugin(const QString &name)
{
    static bool qmlSupportLoaded = false;
    if (!qmlSupportLoaded) {
        QmlPlugins::registerQmlTypes();
        qmlSupportLoaded = true;
    }

    QString fullPath;
    if (QFileInfo(name).isAbsolute()) {
        fullPath = name;
    } else {
        fullPath = DataPaths::locate(DataPaths::Plugins, name);
        if (fullPath.isEmpty()) {
            qWarning() << "QML plugin" << name << "not found";
            return {};
        }
    }

    Plugins::Plugin plugin;
    plugin.type = Plugins::Plugin::QmlPlugin;
    plugin.pluginId = QSL("qml:%1").arg(QFileInfo(name).fileName());
    plugin.pluginPath = fullPath;
    DesktopFile desktopFile(fullPath + QSL("/metadata.desktop"));
    plugin.pluginSpec = Plugins::createSpec(desktopFile);
    plugin.data = QVariant::fromValue(new QmlPluginLoader(plugin.pluginSpec.name, fullPath));
    return plugin;
}

void QmlPlugin::initPlugin(Plugins::Plugin *plugin)
{
    Q_ASSERT(plugin->type == Plugins::Plugin::QmlPlugin);

    const QString name = plugin->pluginSpec.name;

    auto qmlPluginLoader = plugin->data.value<QmlPluginLoader*>();
    if (!qmlPluginLoader) {
        qWarning() << "Failed to cast from data";
        return;
    }
    qmlPluginLoader->createComponent();
    if (!qmlPluginLoader->instance()) {
        qWarning().noquote() << "Failed to create component for" << name << "plugin:" << qmlPluginLoader->component()->errorString();
        return;
    }

    plugin->instance = qobject_cast<PluginInterface*>(qmlPluginLoader->instance());
}
