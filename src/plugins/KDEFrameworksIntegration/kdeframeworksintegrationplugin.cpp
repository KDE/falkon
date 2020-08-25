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
#include "kdeframeworksintegrationplugin.h"
#include "kwalletpasswordbackend.h"
#include "pluginproxy.h"
#include "browserwindow.h"
#include "../config.h"
#include "mainapplication.h"
#include "autofill.h"
#include "passwordmanager.h"
#include "kioschemehandler.h"
#include "webpage.h"
#include "webview.h"

#include <KCrash>
#include <KAboutData>
#include <KProtocolInfo>
#include <PurposeWidgets/Menu>
#include <Purpose/AlternativesModel>

#include <QWebEngineProfile>
#include <QMenu>
#include <QJsonArray>

KDEFrameworksIntegrationPlugin::KDEFrameworksIntegrationPlugin()
    : QObject()
{
}

void KDEFrameworksIntegrationPlugin::init(InitState state, const QString &settingsPath)
{
    Q_UNUSED(state);
    Q_UNUSED(settingsPath);

    m_backend = new KWalletPasswordBackend;
    mApp->autoFill()->passwordManager()->registerBackend(QSL("KWallet"), m_backend);

    // Enable KWallet password backend inside KDE session
    if (qgetenv("KDE_FULL_SESSION") == QByteArray("true")) {
        mApp->autoFill()->passwordManager()->switchBackend(QSL("KWallet"));
    }

    const auto protocols = KProtocolInfo::protocols();
    for (const QString &protocol : protocols) {
        if (WebPage::internalSchemes().contains(protocol)) {
            continue;
        }
        KIOSchemeHandler *handler = new KIOSchemeHandler(protocol, this);
        m_kioSchemeHandlers.append(handler);
        mApp->webProfile()->installUrlSchemeHandler(protocol.toUtf8(), handler);
        WebPage::addSupportedScheme(protocol);
    }

    m_sharePageMenu = new Purpose::Menu();
    m_sharePageMenu->setTitle(tr("Share page"));
    m_sharePageMenu->setIcon(QIcon::fromTheme(QStringLiteral("document-share")));
    m_sharePageMenu->model()->setPluginType(QStringLiteral("ShareUrl"));

    KAboutData aboutData(QSL("falkon"), QSL("Falkon"), QCoreApplication::applicationVersion());
    KAboutData::setApplicationData(aboutData);

    KCrash::initialize();
    KCrash::setFlags(KCrash::KeepFDs);
}

void KDEFrameworksIntegrationPlugin::unload()
{
    mApp->autoFill()->passwordManager()->unregisterBackend(m_backend);
    delete m_backend;
    delete m_sharePageMenu;

    for (KIOSchemeHandler *handler : qAsConst(m_kioSchemeHandlers)) {
        mApp->webProfile()->removeUrlSchemeHandler(handler);
        WebPage::removeSupportedScheme(handler->protocol());
        delete handler;
    }
    m_kioSchemeHandlers.clear();
}

void KDEFrameworksIntegrationPlugin::populateWebViewMenu(QMenu *menu, WebView *view, const WebHitTestResult &r)
{
    Q_UNUSED(r)

    m_sharePageMenu->model()->setInputData(QJsonObject{
        { QStringLiteral("urls"), QJsonArray {QJsonValue(view->url().toString())} },
        { QStringLiteral("title"), QJsonValue(view->title()) }
    });
    m_sharePageMenu->reload();

    menu->addAction(m_sharePageMenu->menuAction());
}

bool KDEFrameworksIntegrationPlugin::testPlugin()
{
    // Require the version that the plugin was built with
    return (Qz::VERSION == QLatin1String(FALKON_VERSION));
}
