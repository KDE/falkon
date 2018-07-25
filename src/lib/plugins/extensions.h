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
#include <QObject>
#include "plugins.h"
#include "mainapplication.h"
#include "pluginproxy.h"
#include "qzcommon.h"

class FALKON_EXPORT Extensions : public QObject
{
    Q_OBJECT
public:
    explicit Extensions(QObject *parent = nullptr);
Q_SIGNALS:
    void reload();
public Q_SLOTS:
    void requestSync();
    void requestReload();
    void pluginStateChanged(const QString &pluginId);
    void showSettings(const QString &pluginId);
    void removeExtension(const QString &pluginId);
    void allowInIncognito(const QString &pluginId, bool allowed);
private:
    QList<Plugins::Plugin> m_availablePlugins;
};
