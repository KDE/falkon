/* ============================================================
* KDEFrameworksIntegration - KDE support plugin for Falkon
* Copyright (C) 2013-2018 David Rosca <nowrep@gmail.com>
* Copyright (C) 2021-2025 Juraj Oravec <jurajoravec@mailo.com>
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
#include "kwalletpasswordbackend.h"
#include "kdeframeworksintegrationplugin.h"
#include "mainapplication.h"
#include "browserwindow.h"
#include "desktopnotificationsfactory.h"
#include "updater.h"

#include <QDateTime>

#include <kwallet_version.h>
#include <KWallet>

static PasswordEntry decodeEntry(const QByteArray &data)
{
    QDataStream stream(data);
    PasswordEntry entry;
    stream >> entry;
    return entry;
}

static QMap<QString, QString> encodeEntry(const PasswordEntry &entry)
{
    QMap<QString, QString> data = {
        {QSL("host"), entry.host},
        {QSL("username"), entry.username},
        {QSL("password"), entry.password},
        {QSL("updated"), QString::number(entry.updated)},
        {QSL("data"), QString::fromUtf8(entry.data)}
    };
    return data;
}

KWalletPasswordBackend::KWalletPasswordBackend(KDEFrameworksIntegrationPlugin *plugin)
    : PasswordBackend()
    , m_wallet(nullptr)
    , m_plugin(plugin)
    , m_entriesLoaded(false)
    , m_openingFailed(false)
{
    initialize();
}

QString KWalletPasswordBackend::name() const
{
    return KDEFrameworksIntegrationPlugin::tr("KWallet");
}

QStringList KWalletPasswordBackend::getUsernames(const QUrl& url)
{
    if (m_entriesLoaded) {
        return PasswordBackend::getUsernames(url);
    }

    if (!KWallet::Wallet::keyDoesNotExist(KWallet::Wallet::NetworkWallet(), QSL("FalkonPasswords"), PasswordManager::createHost(url))) {
        return {KDEFrameworksIntegrationPlugin::tr("Encrypted UserName")};
    }

    return {};
}

QVector<PasswordEntry> KWalletPasswordBackend::getEntries(const QUrl &url)
{
    loadEntries();

    const QString host = PasswordManager::createHost(url);

    QVector<PasswordEntry> list;

    for (const PasswordEntry &entry : std::as_const(m_allEntries)) {
        if (entry.host == host) {
            list.append(entry);
        }
    }

    // Sort to prefer last updated entries
    std::sort(list.begin(), list.end());

    return list;
}

QVector<PasswordEntry> KWalletPasswordBackend::getAllEntries()
{
    loadEntries();

    return m_allEntries;
}

// TODO QT6 - should we just start storing timestamps as 64-bit instead?
static uint Q_DATETIME_TOTIME_T(const QDateTime &dateTime)
{
    if (!dateTime.isValid())
        return uint(-1);
    qint64 retval = dateTime.toMSecsSinceEpoch() / 1000;
    if (quint64(retval) >= Q_UINT64_C(0xFFFFFFFF))
        return uint(-1);
    return uint(retval);
}

void KWalletPasswordBackend::addEntry(const PasswordEntry &entry)
{
    loadEntries();

    if (!m_wallet) {
        showErrorNotification();
        return;
    }

    PasswordEntry stored = entry;
    stored.id = QSL("%1/%2").arg(entry.host, entry.username);
    stored.updated = Q_DATETIME_TOTIME_T(QDateTime::currentDateTime());

    m_wallet->writeMap(stored.id.toString(), encodeEntry(stored));
    m_allEntries.append(stored);
    addExistFlag(stored.host);
}

bool KWalletPasswordBackend::updateEntry(const PasswordEntry &entry)
{
    loadEntries();

    if (!m_wallet) {
        showErrorNotification();
        return false;
    }

    m_wallet->removeEntry(entry.id.toString());
    m_wallet->writeMap(entry.id.toString(), encodeEntry(entry));

    int index = m_allEntries.indexOf(entry);

    if (index > -1) {
        m_allEntries[index] = entry;
    }

    return true;
}

void KWalletPasswordBackend::updateLastUsed(PasswordEntry &entry)
{
    loadEntries();

    if (!m_wallet) {
        showErrorNotification();
        return;
    }

    m_wallet->removeEntry(entry.id.toString());

    entry.updated = Q_DATETIME_TOTIME_T(QDateTime::currentDateTime());

    m_wallet->writeMap(entry.id.toString(), encodeEntry(entry));

    int index = m_allEntries.indexOf(entry);

    if (index > -1) {
        m_allEntries[index] = entry;
    }
}

void KWalletPasswordBackend::removeEntry(const PasswordEntry &entry)
{
    loadEntries();

    if (!m_wallet) {
        showErrorNotification();
        return; 
    }

    m_wallet->removeEntry(entry.id.toString());

    int index = m_allEntries.indexOf(entry);

    if (index > -1) {
        m_allEntries.remove(index);
    }

    removeExistFlag(entry.host);
}

void KWalletPasswordBackend::removeAll()
{
    openWallet();

    if (!m_wallet) {
        showErrorNotification();
        return; 
    }

    m_allEntries.clear();

    m_wallet->removeFolder(QSL("FalkonPasswords"));
    m_wallet->createFolder(QSL("FalkonPasswords"));
}

void KWalletPasswordBackend::showErrorNotification()
{
    static bool initialized;

    if (!initialized) {
        initialized = true;
        mApp->desktopNotifications()->showNotification(KDEFrameworksIntegrationPlugin::tr("KWallet disabled"), KDEFrameworksIntegrationPlugin::tr("Please enable KWallet to save password."));
    }
}

void KWalletPasswordBackend::initialize()
{
    if (m_wallet) {
        return;
    }

    folderMigration();
    updateVersion();
}

void KWalletPasswordBackend::folderMigration()
{
    if (!KWallet::Wallet::folderDoesNotExist(KWallet::Wallet::NetworkWallet(), QSL("FalkonPasswords"))) {
        return;
    }

    openWallet();

    if (!m_wallet) {
        return;
    }

    bool migrationFalkon = !m_wallet->hasFolder(QSL("FalkonPasswords")) && m_wallet->hasFolder(QSL("Falkon"));
    bool migrateQupzilla = !m_wallet->hasFolder(QSL("FalkonPasswords")) && !m_wallet->hasFolder(QSL("Falkon")) && m_wallet->hasFolder(QSL("QupZilla"));
    bool migration = false;

    if (!m_wallet->hasFolder(QSL("FalkonPasswords")) && !m_wallet->createFolder(QSL("FalkonPasswords"))) {
        qWarning() << "KWalletPasswordBackend::initialize Cannot create folder \"FalkonPasswords\"!";
        return;
    }

    if (migrationFalkon) {
        if (!m_wallet->setFolder(QSL("Falkon"))) {
            qWarning() << "KWalletPasswordBackend::initialize Cannot set folder \"Falkon\"!";
            return;
        }
        migration = true;
    }
    else if (migrateQupzilla) {
        if (!m_wallet->setFolder(QSL("QupZilla"))) {
            qWarning() << "KWalletPasswordBackend::initialize Cannot set folder \"QupZilla\"!";
            return;
        }
        migration = true;
    }
    else {
        if (!m_wallet->setFolder(QSL("FalkonPasswords"))) {
            qWarning() << "KWalletPasswordBackend::initialize Cannot set folder \"FalkonPasswords\"!";
            return;
        }
    }

    if (migration) {
        QMap<QString, QByteArray> entries;
        bool ok = false;
        entries = m_wallet->entriesList(&ok);
        if (!ok) {
            qWarning() << "KWalletPasswordBackend::initialize Cannot read entries!";
            return;
        }

        QMap<QString, QByteArray>::const_iterator i = entries.constBegin();
        while (i != entries.constEnd()) {
            PasswordEntry entry = decodeEntry(i.value());
            if (entry.isValid()) {
                m_allEntries.append(entry);
            }
            ++i;
        }

        if (!m_wallet->setFolder(QSL("FalkonPasswords"))) {
            qWarning() << "KWalletPasswordBackend::initialize Cannot set folder \"FalkonPasswords\"!";
            return;
        }

        for (const PasswordEntry &entry : std::as_const(m_allEntries)) {
            m_wallet->writeMap(entry.id.toString(), encodeEntry(entry));
        }
    }
}

void KWalletPasswordBackend::updateVersion()
{
    if (!m_wallet) {
        return;
    }

    Updater::Version lastVersion(m_plugin->lastFalkonVersion());
    Updater::Version currentVersion(QString::fromLatin1(Qz::VERSION));

    if (lastVersion == currentVersion) {
        return;
    }

    if (lastVersion < Updater::Version(QStringLiteral("26.03.71"))) {
        loadEntries();

        QSet<QString> hosts;
        for (const auto &entry : std::as_const(m_allEntries)) {
            hosts.insert(entry.host);
        }

        for (const auto &host : std::as_const(hosts)) {
            addExistFlag(host);
        }
    }
}

void KWalletPasswordBackend::loadEntries()
{
    if (m_entriesLoaded) {
        return;
    }

    openWallet();

    if (!m_wallet) {
        return;
    }

    if (!m_wallet->setFolder(QSL("FalkonPasswords"))) {
        qWarning() << "KWalletPasswordBackend::initialize Cannot set folder \"FalkonPasswords\"!";
        return;
    }

    QMap<QString, QMap<QString, QString>> entriesMap;
    bool ok = false;
    entriesMap = m_wallet->mapList(&ok);
    QMap<QString, QMap<QString, QString>>::const_iterator j = entriesMap.constBegin();
    while (j != entriesMap.constEnd()) {
        PasswordEntry entry;
        entry.id = j.key();
        entry.host = j.value()[QSL("host")];
        entry.username = j.value()[QSL("username")];
        entry.password = j.value()[QSL("password")];
        entry.updated = j.value()[QSL("updated")].toInt();
        entry.data = j.value()[QSL("data")].toUtf8();
        if (entry.isValid()) {
            m_allEntries.append(entry);
        }
        ++j;
    }
    m_entriesLoaded = true;
}

void KWalletPasswordBackend::addExistFlag(const QString &host)
{
    openWallet();

    if (!m_wallet) {
        return;
    }

    if (KWallet::Wallet::keyDoesNotExist(KWallet::Wallet::NetworkWallet(), QSL("FalkonPasswords"), host)) {
        m_wallet->writeMap(host, {{QSL("exists"), QSL("true")}});
    }
}

void KWalletPasswordBackend::removeExistFlag(const QString &host)
{
    openWallet();

    if (!m_wallet) {
        return;
    }

    if (KWallet::Wallet::keyDoesNotExist(KWallet::Wallet::NetworkWallet(), QSL("FalkonPasswords"), host)) {
        return;
    }

    for (const auto &entry : std::as_const(m_allEntries)) {
        if (entry.host == host) {
            return;
        }
    }

    m_wallet->removeEntry(host);
}

void KWalletPasswordBackend::openWallet()
{
    if (m_wallet || m_openingFailed) {
        return;
    }

    WId wid = 0;
    BrowserWindow *w = mApp->getWindow();
    if (w && w->window()) {
        wid = w->window()->winId();
    }
    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(), wid);

    if (!m_wallet) {
        m_openingFailed = true;
        qWarning() << "KWalletPasswordBackend::initialize Cannot open wallet!";
        return;
    }
}

KWalletPasswordBackend::~KWalletPasswordBackend()
{
    if (m_wallet) {
        delete m_wallet;
    }
}
