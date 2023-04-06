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
#include "downloadmanager.h"
#include "kioschemehandler.h"
#include "webpage.h"
#include "webview.h"
#include "downloadkjob.h"
#include "downloaditem.h"
#include "settings.h"

#include <KCrash>
#include <KAboutData>
#include <KProtocolInfo>
#include <Purpose/AlternativesModel>
#include <KUiServerJobTracker>

#include <QWebEngineProfile>
#include <QWebEngineUrlScheme>
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
    
    m_jobTracker = new KUiServerJobTracker(this);
    
    auto manager = mApp->downloadManager();
    connect(manager, &DownloadManager::downloadAdded, this, [=](DownloadItem *item) {
        auto job = new DownloadKJob(item->url(), item->path(), item->fileName(), this);
        m_jobTracker->registerJob(job);
        job->start();
        job->updateDescription();

        connect(item, &DownloadItem::progressChanged, job, &DownloadKJob::progress);
        connect(manager, QOverload<>::of(&DownloadManager::downloadFinished), m_jobTracker, [=]() {
            m_jobTracker->unregisterJob(job);
        });
    });


    QStringList newSchemes;
    const auto protocols = KProtocolInfo::protocols();
    for (const QString &protocol : protocols) {
        if (WebPage::internalSchemes().contains(protocol)) {
            continue;
        }
        if (!QWebEngineUrlScheme::schemeByName(protocol.toUtf8()).name().isEmpty()) {
            auto *handler = new KIOSchemeHandler(protocol, this);
            m_kioSchemeHandlers.append(handler);
            mApp->webProfile()->installUrlSchemeHandler(protocol.toUtf8(), handler);
            WebPage::addSupportedScheme(protocol);
        }
        else {
            newSchemes.append(protocol);
            qInfo() << QSL("KDEFrameworksIntegration: Custom scheme '%1' will be available after browser restart.").arg(protocol);
        }
    }

    if (!newSchemes.isEmpty()) {
        Settings settings;
        settings.beginGroup(QSL("Web-Browser-Settings"));

        QStringList allowedSchemes = settings.value(QSL("AllowedSchemes"), QStringList()).toStringList();
        allowedSchemes.append(newSchemes);
        allowedSchemes.removeDuplicates();
        settings.setValue(QSL("AllowedSchemes"), allowedSchemes);

        settings.endGroup();
    }

    m_sharePageMenu = new Purpose::Menu();
    m_sharePageMenu->setTitle(tr("Share page"));
    m_sharePageMenu->setIcon(QIcon::fromTheme(QStringLiteral("document-share")));
    m_sharePageMenu->model()->setInputData(QJsonObject{
        { QStringLiteral("urls"), QJsonArray {QJsonValue(QSL("falkon"))} },
        { QStringLiteral("title"), QJsonValue(QSL("falkon")) }
    });
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
