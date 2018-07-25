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
#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QVariant>
#include <QPointer>

#include "qzcommon.h"
#include "plugininterface.h"
#include "qml/qmlpluginloader.h"

class QLibrary;
class QPluginLoader;

class SpeedDial;
class Extensions;
class QmlPluginLoader;

struct PluginSpec {
    QString name;
    QString description;
    QString author;
    QString version;
    QPixmap icon;
    QString entryPoint;
    bool hasSettings = false;

    bool operator==(const PluginSpec &other) const {
        return (this->name == other.name &&
                this->description == other.description &&
                this->author == other.author &&
                this->version == other.version);
    }
};

class FALKON_EXPORT Plugins : public QObject
{
    Q_OBJECT
public:
    struct Plugin {
        enum Type {
            Invalid = 0,
            InternalPlugin,
            SharedLibraryPlugin,
            PythonPlugin,
            QmlPlugin
        };
        Type type = Invalid;
        QString pluginId;
        PluginSpec pluginSpec;
        PluginInterface *instance = nullptr;

        // InternalPlugin
        PluginInterface *internalInstance = nullptr;

        // SharedLibraryPlugin
        QString libraryPath;
        QPluginLoader *pluginLoader = nullptr;

        // QmlPlugin
        QmlPluginLoader *qmlPluginLoader = nullptr;

        // Other
        QVariant data;

        bool isLoaded() const {
            return instance;
        }

        bool operator==(const Plugin &other) const {
            return this->type == other.type &&
                   this->pluginId == other.pluginId;
        }
    };

    explicit Plugins(QObject* parent = 0);

    QList<Plugin> getAvailablePlugins();

    bool loadPlugin(Plugin* plugin);
    void unloadPlugin(Plugin* plugin);
    void removePlugin(Plugin *plugin);

    void shutdown();

    // SpeedDial
    SpeedDial* speedDial() { return m_speedDial; }

    // Extensions
    Extensions *extensions() { return m_extensions; }

    static PluginSpec createSpec(const DesktopFile &metaData);

    static QStringList getDefaultAllowedPlugins();

public Q_SLOTS:
    void loadSettings();

    void loadPlugins();

protected:
    QList<PluginInterface*> m_loadedPlugins;

Q_SIGNALS:
    void pluginUnloaded(PluginInterface* plugin);
    void refreshedLoadedPlugins();

private:
    void loadPythonSupport();
    void loadQmlSupport();
    Plugin loadPlugin(const QString &id);
    Plugin loadInternalPlugin(const QString &name);
    Plugin loadSharedLibraryPlugin(const QString &name);
    Plugin loadPythonPlugin(const QString &name);
    Plugin loadQmlPlugin(const QString &name);
    bool initPlugin(PluginInterface::InitState state, Plugin *plugin);
    void initInternalPlugin(Plugin *plugin);
    void initSharedLibraryPlugin(Plugin *plugin);
    void initPythonPlugin(Plugin *plugin);
    void initQmlPlugin(Plugin *plugin);

    void registerAvailablePlugin(const Plugin &plugin);

    void refreshLoadedPlugins();
    void loadAvailablePlugins();

    QList<Plugin> m_availablePlugins;
    QStringList m_allowedPlugins;

    bool m_pluginsLoaded;

    SpeedDial* m_speedDial;
    Extensions *m_extensions;
    QList<PluginInterface*> m_internalPlugins;

    QLibrary *m_pythonPlugin = nullptr;
};

Q_DECLARE_METATYPE(Plugins::Plugin)

#endif // PLUGINLOADER_H
