/* ============================================================
* KDEFrameworksIntegration - KDE support plugin for Falkon
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
#pragma once

#include "plugininterface.h"

#include <Purpose/Menu>

class KWalletPasswordBackend;
class KIOSchemeHandler;
class KUiServerJobTracker;

class KDEFrameworksIntegrationPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.KDEFrameworksIntegration" FILE "kdeframeworksintegration.json")

public:
    explicit KDEFrameworksIntegrationPlugin();

    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;
    void populateWebViewMenu(QMenu *menu, WebView *view, const WebHitTestResult &r) override;

private:
    KWalletPasswordBackend *m_backend = nullptr;
    QVector<KIOSchemeHandler*> m_kioSchemeHandlers;
    Purpose::Menu *m_sharePageMenu = nullptr;
    KUiServerJobTracker *m_jobTracker = nullptr;
};
