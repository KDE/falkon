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
#include "extensions.h"
#include "mainapplication.h"
#include "pluginproxy.h"
#include "sqldatabase.h"
#include <QMessageBox>
#include <QDebug>

Extensions::Extensions(QObject *parent)
    : QObject(parent)
{
}

void Extensions::requestSync()
{
    m_availablePlugins = mApp->plugins()->getAvailablePlugins();
}

void Extensions::requestReload()
{
    emit reload();
}

void Extensions::pluginStateChanged(const QString &pluginId)
{
    foreach (Plugins::Plugin plugin, m_availablePlugins) {
        if (plugin.pluginId == pluginId) {
            if (plugin.isLoaded()) {
                mApp->plugins()->unloadPlugin(&plugin);
                auto job = new SqlQueryJob(QSL("DELETE FROM allowed_plugins WHERE pluginId = ?"), this);
                job->addBindValue(plugin.pluginId);
                job->start();
            } else {
                mApp->plugins()->loadPlugin(&plugin);
                auto job = new SqlQueryJob(QSL("INSERT OR REPLACE INTO allowed_plugins (pluginId, allowInPrivateMode) VALUES (?, ?)"), this);
                job->addBindValue(plugin.pluginId);
                job->addBindValue(0);
                job->start();
            }
            return;
        }
    }
}

void Extensions::showSettings(const QString &pluginId)
{
    foreach (const Plugins::Plugin &plugin, m_availablePlugins) {
        if (plugin.pluginId == pluginId && plugin.pluginSpec.hasSettings) {
            plugin.instance->showSettings();
            return;
        }
    }
}

void Extensions::removeExtension(const QString &pluginId)
{
    Plugins::Plugin requiredPlugin;
    foreach (const Plugins::Plugin &plugin, m_availablePlugins) {
        if (plugin.pluginId == pluginId) {
            requiredPlugin = plugin;
            break;
        }
    }
    if (requiredPlugin.type == Plugins::Plugin::Invalid) {
        return;
    }
    QMessageBox dialog;
    dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    dialog.setText(QString(tr("Are you sure to permanently remove %1 plugin").arg(requiredPlugin.pluginSpec.name)));
    dialog.setIcon(QMessageBox::Question);
    if (dialog.exec() != QMessageBox::Yes) {
        return;
    }

    mApp->plugins()->removePlugin(&requiredPlugin);
}

void Extensions::allowInIncognito(const QString &pluginId, bool allowed)
{
    foreach (const Plugins::Plugin &plugin, m_availablePlugins) {
        if (plugin.pluginId == pluginId && plugin.isLoaded()) {
            auto job = new SqlQueryJob(QSL("UPDATE allowed_plugins SET allowInPrivateMode=? WHERE pluginId=?"), this);
            job->addBindValue(allowed ? 1 : 0);
            job->addBindValue(plugin.pluginId);
            job->start();
            break;
        }
    }
}
