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
#include "adblockmanager.h"
#include "adblockdialog.h"
#include "adblockmatcher.h"
#include "adblocksubscription.h"
#include "adblockurlinterceptor.h"
#include "datapaths.h"
#include "mainapplication.h"
#include "webpage.h"
#include "qztools.h"
#include "browserwindow.h"
#include "settings.h"
#include "networkmanager.h"

#include <QAction>
#include <QDateTime>
#include <QTextStream>
#include <QDir>
#include <QTimer>
#include <QMessageBox>
#include <QUrlQuery>
#include <QMutexLocker>
#include <QSaveFile>

//#define ADBLOCK_DEBUG

#ifdef ADBLOCK_DEBUG
#include <QElapsedTimer>
#endif

Q_GLOBAL_STATIC(AdBlockManager, qz_adblock_manager)

AdBlockManager::AdBlockManager(QObject* parent)
    : QObject(parent)
    , m_loaded(false)
    , m_enabled(true)
    , m_matcher(new AdBlockMatcher(this))
    , m_interceptor(new AdBlockUrlInterceptor(this))
{
    qRegisterMetaType<AdBlockedRequest>();

    load();
}

AdBlockManager::~AdBlockManager()
{
    qDeleteAll(m_subscriptions);
}

AdBlockManager* AdBlockManager::instance()
{
    return qz_adblock_manager();
}

void AdBlockManager::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }

    m_enabled = enabled;
    Q_EMIT enabledChanged(enabled);

    Settings settings;
    settings.beginGroup(QSL("AdBlock"));
    settings.setValue(QSL("enabled"), m_enabled);
    settings.endGroup();

    load();
    mApp->reloadUserStyleSheet();

    QMutexLocker locker(&m_mutex);

    if (m_enabled) {
        m_matcher->update();
    } else {
        m_matcher->clear();
    }
}

QList<AdBlockSubscription*> AdBlockManager::subscriptions() const
{
    return m_subscriptions;
}

bool AdBlockManager::block(QWebEngineUrlRequestInfo &request, QString &ruleFilter, QString &ruleSubscription)
{
    QMutexLocker locker(&m_mutex);

    if (!isEnabled()) {
        return false;
    }

#ifdef ADBLOCK_DEBUG
    QElapsedTimer timer;
    timer.start();
#endif
    const QString urlString = request.requestUrl().toEncoded().toLower();
    const QString urlDomain = request.requestUrl().host().toLower();
    const QString urlScheme = request.requestUrl().scheme().toLower();

    if (!canRunOnScheme(urlScheme) || !canBeBlocked(request.firstPartyUrl())) {
        return false;
    }

    const AdBlockRule* blockedRule = m_matcher->match(request, urlDomain, urlString);

    if (blockedRule) {
        ruleFilter = blockedRule->filter();
        ruleSubscription = blockedRule->subscription()->title();
#ifdef ADBLOCK_DEBUG
        qDebug() << "BLOCKED: " << timer.elapsed() << blockedRule->filter() << request.requestUrl();
#endif
    }

#ifdef ADBLOCK_DEBUG
    qDebug() << timer.elapsed() << request.requestUrl();
#endif

    return blockedRule;
}

QVector<AdBlockedRequest> AdBlockManager::blockedRequestsForUrl(const QUrl &url) const
{
    return m_blockedRequests.value(url);
}

void AdBlockManager::clearBlockedRequestsForUrl(const QUrl &url)
{
    if (m_blockedRequests.remove(url)) {
        Q_EMIT blockedRequestsChanged(url);
    }
}

QStringList AdBlockManager::disabledRules() const
{
    return m_disabledRules;
}

void AdBlockManager::addDisabledRule(const QString &filter)
{
    m_disabledRules.append(filter);
}

void AdBlockManager::removeDisabledRule(const QString &filter)
{
    m_disabledRules.removeOne(filter);
}

bool AdBlockManager::addSubscriptionFromUrl(const QUrl &url)
{
    const QList<QPair<QString, QString> > queryItems = QUrlQuery(url).queryItems(QUrl::FullyDecoded);

    QString subscriptionTitle;
    QString subscriptionUrl;

    for (int i = 0; i < queryItems.count(); ++i) {
        QPair<QString, QString> pair = queryItems.at(i);
        if (pair.first.endsWith(QL1S("location")))
            subscriptionUrl = pair.second;
        else if (pair.first.endsWith(QL1S("title")))
            subscriptionTitle = pair.second;
    }

    if (subscriptionTitle.isEmpty() || subscriptionUrl.isEmpty())
        return false;

    const QString message = AdBlockManager::tr("Do you want to add <b>%1</b> subscription?").arg(subscriptionTitle);

    QMessageBox::StandardButton result = QMessageBox::question(nullptr, AdBlockManager::tr("AdBlock Subscription"), message, QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        AdBlockManager::instance()->addSubscription(subscriptionTitle, subscriptionUrl);
        AdBlockManager::instance()->showDialog();
    }

    return true;
}

AdBlockSubscription* AdBlockManager::addSubscription(const QString &title, const QString &url)
{
    if (title.isEmpty() || url.isEmpty()) {
        return nullptr;
    }

    QString fileName = QzTools::filterCharsFromFilename(title.toLower()) + QSL(".txt");
    QString filePath = QzTools::ensureUniqueFilename(DataPaths::currentProfilePath() + QSL("/adblock/") + fileName);

    QByteArray data = QSL("Title: %1\nUrl: %2\n[Adblock Plus 1.1.1]").arg(title, url).toLatin1();

    QSaveFile file(filePath);
    if (!file.open(QFile::WriteOnly)) {
        qWarning() << "AdBlockManager: Cannot write to file" << filePath;
        return nullptr;
    }
    file.write(data);
    file.commit();

    auto* subscription = new AdBlockSubscription(title, this);
    subscription->setUrl(QUrl(url));
    subscription->setFilePath(filePath);
    subscription->loadSubscription(m_disabledRules);

    m_subscriptions.insert(m_subscriptions.count() - 1, subscription);
    connect(subscription, &AdBlockSubscription::subscriptionUpdated, mApp, &MainApplication::reloadUserStyleSheet);
    connect(subscription, &AdBlockSubscription::subscriptionChanged, this, &AdBlockManager::updateMatcher);

    return subscription;
}

bool AdBlockManager::removeSubscription(AdBlockSubscription* subscription)
{
    QMutexLocker locker(&m_mutex);

    if (!m_subscriptions.contains(subscription) || !subscription->canBeRemoved()) {
        return false;
    }

    QFile(subscription->filePath()).remove();
    m_subscriptions.removeOne(subscription);

    m_matcher->update();
    delete subscription;

    return true;
}

AdBlockCustomList* AdBlockManager::customList() const
{
    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        auto* list = qobject_cast<AdBlockCustomList*>(subscription);

        if (list) {
            return list;
        }
    }

    return nullptr;
}

void AdBlockManager::load()
{
    QMutexLocker locker(&m_mutex);

    if (m_loaded) {
        return;
    }

#ifdef ADBLOCK_DEBUG
    QElapsedTimer timer;
    timer.start();
#endif

    Settings settings;
    settings.beginGroup(QSL("AdBlock"));
    m_enabled = settings.value(QSL("enabled"), m_enabled).toBool();
    m_disabledRules = settings.value(QSL("disabledRules"), QStringList()).toStringList();
    QDateTime lastUpdate = settings.value(QSL("lastUpdate"), QDateTime()).toDateTime();
    settings.endGroup();

    if (!m_enabled) {
        return;
    }

    QDir adblockDir(DataPaths::currentProfilePath() + QSL("/adblock"));
    // Create if necessary
    if (!adblockDir.exists()) {
        QDir(DataPaths::currentProfilePath()).mkdir(QSL("adblock"));
    }

    const auto fileNames = adblockDir.entryList(QStringList(QSL("*.txt")), QDir::Files);
    for (const QString &fileName : fileNames) {
        if (fileName == QLatin1String("customlist.txt")) {
            continue;
        }

        const QString absolutePath = adblockDir.absoluteFilePath(fileName);
        QFile file(absolutePath);
        if (!file.open(QFile::ReadOnly)) {
            continue;
        }

        QTextStream textStream(&file);
        textStream.setCodec("UTF-8");
        QString title = textStream.readLine(1024).remove(QLatin1String("Title: "));
        QUrl url = QUrl(textStream.readLine(1024).remove(QLatin1String("Url: ")));

        if (title.isEmpty() || !url.isValid()) {
            qWarning() << "AdBlockManager: Invalid subscription file" << absolutePath;
            continue;
        }

        auto* subscription = new AdBlockSubscription(title, this);
        subscription->setUrl(url);
        subscription->setFilePath(absolutePath);

        m_subscriptions.append(subscription);
    }

    // Add EasyList + NoCoinList if subscriptions are empty
    if (m_subscriptions.isEmpty()) {
        auto *easyList = new AdBlockSubscription(tr("EasyList"), this);
        easyList->setUrl(QUrl(ADBLOCK_EASYLIST_URL));
        easyList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/easylist.txt"));
        m_subscriptions.append(easyList);

        auto *noCoinList = new AdBlockSubscription(tr("NoCoin List"), this);
        noCoinList->setUrl(QUrl(ADBLOCK_NOCOINLIST_URL));
        noCoinList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/nocoinlist.txt"));
        m_subscriptions.append(noCoinList);
    }

    // Append CustomList
    auto* customList = new AdBlockCustomList(this);
    m_subscriptions.append(customList);

    // Load all subscriptions
    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        subscription->loadSubscription(m_disabledRules);

        connect(subscription, &AdBlockSubscription::subscriptionUpdated, mApp, &MainApplication::reloadUserStyleSheet);
        connect(subscription, &AdBlockSubscription::subscriptionChanged, this, &AdBlockManager::updateMatcher);
    }

    if (lastUpdate.addDays(5) < QDateTime::currentDateTime()) {
        QTimer::singleShot(1000 * 60, this, &AdBlockManager::updateAllSubscriptions);
    }

#ifdef ADBLOCK_DEBUG
    qDebug() << "AdBlock loaded in" << timer.elapsed();
#endif

    m_matcher->update();
    m_loaded = true;

    connect(m_interceptor, &AdBlockUrlInterceptor::requestBlocked, this, [this](const AdBlockedRequest &request) {
        m_blockedRequests[request.firstPartyUrl].append(request);
        Q_EMIT blockedRequestsChanged(request.firstPartyUrl);
    });

    mApp->networkManager()->installUrlInterceptor(m_interceptor);
}

void AdBlockManager::updateMatcher()
{
    QMutexLocker locker(&m_mutex);

    mApp->networkManager()->removeUrlInterceptor(m_interceptor);
    m_matcher->update();
    mApp->networkManager()->installUrlInterceptor(m_interceptor);
}

void AdBlockManager::updateAllSubscriptions()
{
    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        subscription->updateSubscription();
    }

    Settings settings;
    settings.beginGroup(QSL("AdBlock"));
    settings.setValue(QSL("lastUpdate"), QDateTime::currentDateTime());
    settings.endGroup();
}

void AdBlockManager::save()
{
    if (!m_loaded) {
        return;
    }

    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        subscription->saveSubscription();
    }

    Settings settings;
    settings.beginGroup(QSL("AdBlock"));
    settings.setValue(QSL("enabled"), m_enabled);
    settings.setValue(QSL("disabledRules"), m_disabledRules);
    settings.endGroup();
}

bool AdBlockManager::isEnabled() const
{
    return m_enabled;
}

bool AdBlockManager::canRunOnScheme(const QString &scheme) const
{
    return !(scheme == QL1S("file") || scheme == QL1S("qrc") || scheme == QL1S("view-source")
             || scheme == QL1S("falkon") || scheme == QL1S("data") || scheme == QL1S("abp"));
}

bool AdBlockManager::canBeBlocked(const QUrl &url) const
{
    return !m_matcher->adBlockDisabledForUrl(url);
}

QString AdBlockManager::elementHidingRules(const QUrl &url) const
{
    if (!isEnabled() || !canRunOnScheme(url.scheme()) || m_matcher->genericElemHideDisabledForUrl(url))
        return {};

    return m_matcher->elementHidingRules();
}

QString AdBlockManager::elementHidingRulesForDomain(const QUrl &url) const
{
    if (!isEnabled() || !canRunOnScheme(url.scheme()) || m_matcher->elemHideDisabledForUrl(url))
        return {};

    return m_matcher->elementHidingRulesForDomain(url.host());
}

AdBlockSubscription* AdBlockManager::subscriptionByName(const QString &name) const
{
    for (AdBlockSubscription* subscription : qAsConst(m_subscriptions)) {
        if (subscription->title() == name) {
            return subscription;
        }
    }

    return nullptr;
}

AdBlockDialog *AdBlockManager::showDialog(QWidget *parent)
{
    if (!m_adBlockDialog) {
        m_adBlockDialog = new AdBlockDialog(parent ? parent : mApp->getWindow());
    }

    m_adBlockDialog.data()->show();
    m_adBlockDialog.data()->raise();
    m_adBlockDialog.data()->activateWindow();

    return m_adBlockDialog.data();
}

void AdBlockManager::showRule()
{
    if (auto* action = qobject_cast<QAction*>(sender())) {
        const AdBlockRule* rule = static_cast<const AdBlockRule*>(action->data().value<void*>());

        if (rule) {
            showDialog()->showRule(rule);
        }
    }
}
