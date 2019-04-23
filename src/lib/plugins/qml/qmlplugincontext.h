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
#pragma once

#include <QQmlContext>

#include "plugins.h"

class QmlPluginContext : public QQmlContext
{
    Q_OBJECT

public:
    explicit QmlPluginContext(const Plugins::Plugin &plugin, QQmlEngine *engine, QObject *parent = nullptr);

    QString pluginPath() const;
    QString pluginName() const;

    Plugins::Plugin plugin() const;

    static QmlPluginContext *contextForObject(const QObject *object);

private:
    Plugins::Plugin m_plugin;
};
