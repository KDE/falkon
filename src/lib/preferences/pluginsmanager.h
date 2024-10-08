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
#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QWidget>

#include "plugins.h"
#include "qzcommon.h"

namespace Ui
{
class PluginsList;
}

class QListWidgetItem;

class FALKON_EXPORT PluginsManager : public QWidget
{
    Q_OBJECT

public:
    enum PluginRoles {
        Version = Qt::UserRole,
        Author = Qt::UserRole + 1,
        Description = Qt::UserRole + 2,
        PluginId = Qt::UserRole + 3
    };

    explicit PluginsManager(QWidget* parent = nullptr);
    ~PluginsManager();

    void load();
    void save();

private Q_SLOTS:
    void settingsClicked();
    void removeClicked();
    void currentChanged(QListWidgetItem* item);
    void itemChanged(QListWidgetItem* item);
    void addFilter(const QString& newText);

    void refresh();

private:
    void sortItems();

    Ui::PluginsList* ui;
    bool m_loaded;
    bool m_blockRefresh = false;
    QHash<QString, Plugins::Plugin> m_pluginList;
};

#endif // PLUGINSMANAGER_H
