/* ============================================================
* KDESupport - KDE support plugin for Falkon
* Copyright (C) 2013-2018 David Rosca <nowrep@gmail.com>
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
#ifndef KDESUPPORTPLUGIN_H
#define KDESUPPORTPLUGIN_H

#include "plugininterface.h"

class KWalletPasswordBackend;

class KDESupportPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.KDESupport")

public:
    explicit KDESupportPlugin();

    DesktopFile metaData() const override;
    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;

private:
    KWalletPasswordBackend* m_backend;

};

#endif // KDESUPPORTPLUGIN_H
