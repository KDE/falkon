/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "pluginproxy.h"
#include "plugininterface.h"
#include "mainapplication.h"
#include "speeddial.h"
#include "settings.h"
#include "datapaths.h"
#include "adblock/adblockplugin.h"
#include "../config.h"
#include "desktopfile.h"
#include "qml/qmlplugins.h"
#include "sqldatabase.h"

#include <iostream>
#include <QPluginLoader>
#include <QDir>
#include <QQmlEngine>
#include <QQmlComponent>

Plugins::Plugins(QObject* parent)
    : QObject(parent)
    , m_pluginsLoaded(false)
    , m_speedDial(new SpeedDial(this))
{
    loadSettings();

    if (!MainApplication::isTestModeEnabled()) {
        loadPythonSupport();
    }

    loadQmlSupport();
}

QList<Plugins::Plugin> Plugins::getAvailablePlugins()
{
    loadAvailablePlugins();

    return m_availablePlugins;
}

bool Plugins::loadPlugin(Plugins::Plugin* plugin)
{
    if (plugin->isLoaded()) {
        return true;
    }

    if (!initPlugin(PluginInterface::LateInitState, plugin)) {
        return false;
    }

    m_availablePlugins.removeOne(*plugin);
    m_availablePlugins.prepend(*plugin);

    refreshLoadedPlugins();

    return plugin->isLoaded();
}

void Plugins::unloadPlugin(Plugins::Plugin* plugin)
{
    if (!plugin->isLoaded()) {
        return;
    }

    plugin->instance->unload();
    emit pluginUnloaded(plugin->instance);
    plugin->instance = nullptr;

    m_availablePlugins.removeOne(*plugin);
    m_availablePlugins.append(*plugin);

    refreshLoadedPlugins();
}

void Plugins::loadSettings()
{
    QSqlQuery query(SqlDatabase::instance()->database());
    if (mApp->isPrivate()) {
        query.exec(QSL("SELECT * FROM allowed_plugins WHERE allowInPrivateMode=1"));
    } else {
        query.exec(QSL("SELECT * FROM allowed_plugins"));
    }
    while (query.next()) {
        m_allowedPlugins.append(query.value(0).toString());
    }
}

void Plugins::shutdown()
{
    foreach (PluginInterface* iPlugin, m_loadedPlugins) {
        iPlugin->unload();
    }
}

PluginSpec Plugins::createSpec(const DesktopFile &metaData)
{
    PluginSpec spec;
    spec.name = metaData.name();
    spec.description = metaData.comment();
    spec.version = metaData.value(QSL("X-Falkon-Version")).toString();
    spec.author = QSL("%1 <%2>").arg(metaData.value(QSL("X-Falkon-Author")).toString(), metaData.value(QSL("X-Falkon-Email")).toString());
    spec.entryPoint = metaData.value(QSL("X-Falkon-EntryPoint")).toString();
    spec.hasSettings = metaData.value(QSL("X-Falkon-Settings")).toBool();

    const QString iconName = metaData.icon();
    if (!iconName.isEmpty()) {
        if (QFileInfo::exists(iconName)) {
            spec.icon = QIcon(iconName).pixmap(32);
        } else {
            const QString relativeFile = QFileInfo(metaData.fileName()).dir().absoluteFilePath(iconName);
            if (QFileInfo::exists(relativeFile)) {
                spec.icon = QIcon(relativeFile).pixmap(32);
            } else {
                spec.icon = QIcon::fromTheme(iconName).pixmap(32);
            }
        }
    }

    return spec;
}

void Plugins::loadPlugins()
{
    QDir settingsDir(DataPaths::currentProfilePath() + "/extensions/");
    if (!settingsDir.exists()) {
        settingsDir.mkdir(settingsDir.absolutePath());
    }

    foreach (const QString &pluginId, m_allowedPlugins) {
        Plugin plugin = loadPlugin(pluginId);
        if (plugin.type == Plugin::Invalid) {
            continue;
        }
        if (plugin.pluginSpec.name.isEmpty()) {
            qWarning() << "Invalid plugin spec of" << pluginId << "plugin";
            continue;
        }
        if (!initPlugin(PluginInterface::StartupInitState, &plugin)) {
            qWarning() << "Failed to init" << pluginId << "plugin";
            continue;
        }
        registerAvailablePlugin(plugin);
    }

    refreshLoadedPlugins();

    std::cout << "Falkon: " << m_loadedPlugins.count() << " extensions loaded"  << std::endl;
}

void Plugins::loadAvailablePlugins()
{
    if (m_pluginsLoaded) {
        return;
    }

    m_pluginsLoaded = true;

    const QStringList dirs = DataPaths::allPaths(DataPaths::Plugins);

    // InternalPlugin
    registerAvailablePlugin(loadInternalPlugin(QSL("adblock")));

    // SharedLibraryPlugin
    for (const QString &dir : dirs) {
        const auto files = QDir(dir).entryInfoList(QDir::Files);
        for (const QFileInfo &info : files) {
            if (info.baseName() == QL1S("PyFalkon")) {
                continue;
            }
            Plugin plugin = loadSharedLibraryPlugin(info.absoluteFilePath());
            if (plugin.type == Plugin::Invalid) {
                continue;
            }
            if (plugin.pluginSpec.name.isEmpty()) {
                qWarning() << "Invalid plugin spec of" << info.absoluteFilePath() << "plugin";
                continue;
            }
            registerAvailablePlugin(plugin);
        }
    }

    // PythonPlugin
    if (m_pythonPlugin) {
        auto f = (QVector<Plugin>(*)()) m_pythonPlugin->resolve("pyfalkon_load_available_plugins");
        if (!f) {
            qWarning() << "Failed to resolve" << "pyfalkon_load_available_plugins";
        } else {
            const auto plugins = f();
            for (const auto &plugin : plugins) {
                registerAvailablePlugin(plugin);
            }
        }
    }

    // QmlPlugin
    for (QString dir: dirs) {
        // qml plugins will be loaded from subdirectory qml
        dir.append(QSL("/qml"));
        const auto qmlDirs = QDir(dir).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &info : qmlDirs) {
            Plugin plugin = loadQmlPlugin(info.absoluteFilePath());
            if (plugin.type == Plugin::Invalid) {
                continue;
            }
            if (plugin.pluginSpec.name.isEmpty()) {
                qWarning() << "Invalid plugin spec of" << info.absoluteFilePath() << "plugin";
                continue;
            }
            registerAvailablePlugin(plugin);
        }
    }
}

void Plugins::registerAvailablePlugin(const Plugin &plugin)
{
    if (!m_availablePlugins.contains(plugin)) {
        m_availablePlugins.append(plugin);
    }
}

void Plugins::refreshLoadedPlugins()
{
    m_loadedPlugins.clear();

    foreach (const Plugin &plugin, m_availablePlugins) {
        if (plugin.isLoaded()) {
            m_loadedPlugins.append(plugin.instance);
        }
    }
}

void Plugins::loadPythonSupport()
{
    const QStringList dirs = DataPaths::allPaths(DataPaths::Plugins);
    for (const QString &dir : dirs) {
        const auto files = QDir(dir).entryInfoList({QSL("PyFalkon*")}, QDir::Files);
        for (const QFileInfo &info : files) {
            m_pythonPlugin = new QLibrary(info.absoluteFilePath(), this);
            m_pythonPlugin->setLoadHints(QLibrary::ExportExternalSymbolsHint);
            if (!m_pythonPlugin->load()) {
                qWarning() << "Failed to load python support plugin" << m_pythonPlugin->errorString();
                delete m_pythonPlugin;
                m_pythonPlugin = nullptr;
            } else {
                std::cout << "Falkon: Python plugin support initialized" << std::endl;
                return;
            }
        }
    }
}

void Plugins::loadQmlSupport()
{
    QmlPlugins::registerQmlTypes();
}

Plugins::Plugin Plugins::loadPlugin(const QString &id)
{
    QString name;
    Plugin::Type type = Plugin::Invalid;

    const int colon = id.indexOf(QL1C(':'));
    if (colon > -1) {
        const auto t = id.leftRef(colon);
        if (t == QL1S("internal")) {
            type = Plugin::InternalPlugin;
        } else if (t == QL1S("lib")) {
            type = Plugin::SharedLibraryPlugin;
        } else if (t == QL1S("python")) {
            type = Plugin::PythonPlugin;
        } else if (t == QL1S("qml")) {
            type = Plugin::QmlPlugin;
        }
        name = id.mid(colon + 1);
    } else {
        name = id;
        type = Plugin::SharedLibraryPlugin;
    }

    switch (type) {
    case Plugin::InternalPlugin:
        return loadInternalPlugin(name);

    case Plugin::SharedLibraryPlugin:
        return loadSharedLibraryPlugin(name);

    case Plugin::PythonPlugin:
        return loadPythonPlugin(name);

    case Plugin::QmlPlugin:
        return loadQmlPlugin(name);

    default:
        return Plugin();
    }
}

Plugins::Plugin Plugins::loadInternalPlugin(const QString &name)
{
    if (name == QL1S("adblock")) {
        Plugin plugin;
        plugin.type = Plugin::InternalPlugin;
        plugin.pluginId = QSL("internal:adblock");
        plugin.internalInstance = new AdBlockPlugin();
        plugin.pluginSpec = createSpec(plugin.internalInstance->metaData());
        return plugin;
    } else {
        return Plugin();
    }
}

Plugins::Plugin Plugins::loadSharedLibraryPlugin(const QString &name)
{
    QString fullPath;
    if (QFileInfo(name).isAbsolute()) {
        fullPath = name;
    } else {
        fullPath = DataPaths::locate(DataPaths::Plugins, name);
        if (fullPath.isEmpty()) {
            qWarning() << "Plugin" << name << "not found";
            return Plugin();
        }
    }

    QPluginLoader *loader = new QPluginLoader(fullPath);
    PluginInterface *iPlugin = qobject_cast<PluginInterface*>(loader->instance());

    if (!iPlugin) {
        qWarning() << "Loading" << fullPath << "failed:" << loader->errorString();
        return Plugin();
    }

    Plugin plugin;
    plugin.type = Plugin::SharedLibraryPlugin;
    plugin.pluginId = QSL("lib:%1").arg(QFileInfo(fullPath).fileName());
    plugin.libraryPath = fullPath;
    plugin.pluginLoader = loader;
    plugin.pluginSpec = createSpec(iPlugin->metaData());
    return plugin;
}

Plugins::Plugin Plugins::loadPythonPlugin(const QString &name)
{
    if (!m_pythonPlugin) {
        qWarning() << "Python support plugin is not loaded";
        return Plugin();
    }

    auto f = (Plugin(*)(const QString &)) m_pythonPlugin->resolve("pyfalkon_load_plugin");
    if (!f) {
        qWarning() << "Failed to resolve" << "pyfalkon_load_plugin";
        return Plugin();
    }

    return f(name);
}

Plugins::Plugin Plugins::loadQmlPlugin(const QString &name)
{
    QString fullPath;
    if (QFileInfo(name).isAbsolute()) {
        fullPath = name;
    } else {
        fullPath = DataPaths::locate(DataPaths::Plugins, QSL("qml/") + name);
        if (fullPath.isEmpty()) {
            qWarning() << "Plugin" << name << "not found";
            return Plugin();
        }
    }

    Plugin plugin;
    plugin.type = Plugin::QmlPlugin;
    plugin.pluginId = QSL("qml:%1").arg(QFileInfo(name).fileName());
    plugin.pluginSpec = createSpec(DesktopFile(fullPath + QSL("/metadata.desktop")));
    plugin.qmlPluginLoader = new QmlPluginLoader(QDir(fullPath).filePath(plugin.pluginSpec.entryPoint));
    if (!plugin.qmlPluginLoader->instance()) {
        qWarning() << "Loading" << fullPath << "failed:" << plugin.qmlPluginLoader->component()->errorString();
        return Plugin();
    }

    return plugin;
}

bool Plugins::initPlugin(PluginInterface::InitState state, Plugin *plugin)
{
    if (!plugin) {
        return false;
    }

    switch (plugin->type) {
    case Plugin::InternalPlugin:
        initInternalPlugin(plugin);
        break;

    case Plugin::SharedLibraryPlugin:
        initSharedLibraryPlugin(plugin);
        break;

    case Plugin::PythonPlugin:
        initPythonPlugin(plugin);
        break;

    case Plugin::QmlPlugin:
        initQmlPlugin(plugin);
        break;

    default:
        return false;
    }

    if (!plugin->instance) {
        return false;
    }

    plugin->instance->init(state, DataPaths::currentProfilePath() + QL1S("/extensions"));

    if (!plugin->instance->testPlugin()) {
        emit pluginUnloaded(plugin->instance);
        plugin->instance = nullptr;
        return false;
    }

    return true;
}

void Plugins::initInternalPlugin(Plugin *plugin)
{
    Q_ASSERT(plugin->type == Plugin::InternalPlugin);

    plugin->instance = plugin->internalInstance;
}

void Plugins::initSharedLibraryPlugin(Plugin *plugin)
{
    Q_ASSERT(plugin->type == Plugin::SharedLibraryPlugin);

    plugin->instance = qobject_cast<PluginInterface*>(plugin->pluginLoader->instance());
}

void Plugins::initPythonPlugin(Plugin *plugin)
{
    Q_ASSERT(plugin->type == Plugin::PythonPlugin);

    if (!m_pythonPlugin) {
        qWarning() << "Python support plugin is not loaded";
        return;
    }

    auto f = (void(*)(Plugin *)) m_pythonPlugin->resolve("pyfalkon_init_plugin");
    if (!f) {
        qWarning() << "Failed to resolve" << "pyfalkon_init_plugin";
        return;
    }

    f(plugin);
}

void Plugins::initQmlPlugin(Plugin *plugin)
{
    Q_ASSERT(plugin->type == Plugin::QmlPlugin);

    plugin->qmlPluginLoader->setName(plugin->pluginSpec.name);
    plugin->instance = qobject_cast<PluginInterface*>(plugin->qmlPluginLoader->instance());
}

// static
QStringList Plugins::getDefaultAllowedPlugins()
{
    QStringList defaultAllowedPlugins = {
        QSL("internal:adblock")
    };

    // Enable KDE Frameworks Integration when running inside KDE session
    if (qgetenv("KDE_FULL_SESSION") == QByteArray("true")) {
        defaultAllowedPlugins.append(QSL("lib:KDEFrameworksIntegration.so"));
    }

    return defaultAllowedPlugins;
}
