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

class QLibrary;
class QPluginLoader;

class SpeedDial;

struct PluginSpec {
    QString name;
    QString description;
    QString author;
    QString version;
    QPixmap icon;
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
        QString pluginPath;
        PluginSpec pluginSpec;
        PluginInterface *instance = nullptr;

        // InternalPlugin
        PluginInterface *internalInstance = nullptr;

        // SharedLibraryPlugin
        QPluginLoader *pluginLoader = nullptr;

        // Other
        QVariant data;

        bool isLoaded() const;
        bool isRemovable() const;
        bool operator==(const Plugin &other) const;
    };

    explicit Plugins(QObject* parent = nullptr);

    QList<Plugin> availablePlugins();

    bool loadPlugin(Plugin* plugin);
    void unloadPlugin(Plugin* plugin);
    void removePlugin(Plugin *plugin);

    bool addPlugin(const QString &id);

    void shutdown();

    // SpeedDial
    SpeedDial* speedDial() { return m_speedDial; }

    static PluginSpec createSpec(const QJsonObject &metaData);
    static PluginSpec createSpec(const DesktopFile &metaData);

public Q_SLOTS:
    void loadSettings();

    void loadPlugins();

protected:
    QList<PluginInterface*> m_loadedPlugins;

Q_SIGNALS:
    void pluginUnloaded(PluginInterface* plugin);
    void availablePluginsChanged();

private:
    void loadPythonSupport();
    Plugin loadPlugin(const QString &id);
    Plugin loadInternalPlugin(const QString &name);
    Plugin loadSharedLibraryPlugin(const QString &name);
    Plugin loadPythonPlugin(const QString &name);
    bool initPlugin(PluginInterface::InitState state, Plugin *plugin);
    void initInternalPlugin(Plugin *plugin);
    void initSharedLibraryPlugin(Plugin *plugin);
    void initPythonPlugin(Plugin *plugin);

    void registerAvailablePlugin(const Plugin &plugin);

    void refreshLoadedPlugins();
    void loadAvailablePlugins();

    QList<Plugin> m_availablePlugins;
    QStringList m_allowedPlugins;

    bool m_pluginsLoaded;

    SpeedDial* m_speedDial;
    QList<PluginInterface*> m_internalPlugins;

    QLibrary *m_pythonPlugin = nullptr;
};

Q_DECLARE_METATYPE(Plugins::Plugin)

#endif // PLUGINLOADER_H
