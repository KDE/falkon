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
#include "kwalletpasswordbackend.h"
#include "kdeframeworksintegrationplugin.h"
#include "mainapplication.h"
#include "browserwindow.h"
#include "desktopnotificationsfactory.h"

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

static QByteArray encodeEntry(const PasswordEntry &entry)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << entry;
    return data;
}

KWalletPasswordBackend::KWalletPasswordBackend()
    : PasswordBackend()
    , m_wallet(0)
{
}

QString KWalletPasswordBackend::name() const
{
    return KDEFrameworksIntegrationPlugin::tr("KWallet");
}

QVector<PasswordEntry> KWalletPasswordBackend::getEntries(const QUrl &url)
{
    initialize();

    const QString host = PasswordManager::createHost(url);

    QVector<PasswordEntry> list;

    for (const PasswordEntry &entry : qAsConst(m_allEntries)) {
        if (entry.host == host) {
            list.append(entry);
        }
    }

    // Sort to prefer last updated entries
    qSort(list.begin(), list.end());

    return list;
}

QVector<PasswordEntry> KWalletPasswordBackend::getAllEntries()
{
    initialize();

    return m_allEntries;
}

void KWalletPasswordBackend::addEntry(const PasswordEntry &entry)
{
    initialize();

    if (!m_wallet) {
        showErrorNotification();
        return;
    }

    PasswordEntry stored = entry;
    stored.id = QString("%1/%2").arg(entry.host, entry.username);
    stored.updated = QDateTime::currentDateTime().toTime_t();

    m_wallet->writeEntry(stored.id.toString(), encodeEntry(stored));
    m_allEntries.append(stored);
}

bool KWalletPasswordBackend::updateEntry(const PasswordEntry &entry)
{
    initialize();

    if (!m_wallet) {
        showErrorNotification();
        return false;
    }

    m_wallet->removeEntry(entry.id.toString());
    m_wallet->writeEntry(entry.id.toString(), encodeEntry(entry));

    int index = m_allEntries.indexOf(entry);

    if (index > -1) {
        m_allEntries[index] = entry;
    }

    return true;
}

void KWalletPasswordBackend::updateLastUsed(PasswordEntry &entry)
{
    initialize();

    if (!m_wallet) {
        showErrorNotification();
        return;        
    }

    m_wallet->removeEntry(entry.id.toString());

    entry.updated = QDateTime::currentDateTime().toTime_t();

    m_wallet->writeEntry(entry.id.toString(), encodeEntry(entry));

    int index = m_allEntries.indexOf(entry);

    if (index > -1) {
        m_allEntries[index] = entry;
    }
}

void KWalletPasswordBackend::removeEntry(const PasswordEntry &entry)
{
    initialize();

    if (!m_wallet) {
        showErrorNotification();
        return; 
    }

    m_wallet->removeEntry(entry.id.toString());

    int index = m_allEntries.indexOf(entry);

    if (index > -1) {
        m_allEntries.remove(index);
    }
}

void KWalletPasswordBackend::removeAll()
{
    initialize();

    if (!m_wallet) {
        showErrorNotification();
        return; 
    }

    m_allEntries.clear();

    m_wallet->removeFolder("Falkon");
    m_wallet->createFolder("Falkon");
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

    WId wid = 0;
    BrowserWindow *w = mApp->getWindow();
    if (w && w->window()) {
        wid = w->window()->winId();
    }
    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(), wid);

    if (!m_wallet) {
        qWarning() << "KWalletPasswordBackend::initialize Cannot open wallet!";
        return;
    }

    bool migrate = !m_wallet->hasFolder("Falkon") && m_wallet->hasFolder("QupZilla");

    if (!m_wallet->hasFolder("Falkon") && !m_wallet->createFolder("Falkon")) {
        qWarning() << "KWalletPasswordBackend::initialize Cannot create folder \"Falkon\"!";
        return;
    }

    if (migrate) {
        if (!m_wallet->setFolder("QupZilla")) {
            qWarning() << "KWalletPasswordBackend::initialize Cannot set folder \"QupZilla\"!";
            return;
        }
    } else {
        if (!m_wallet->setFolder("Falkon")) {
            qWarning() << "KWalletPasswordBackend::initialize Cannot set folder \"Falkon\"!";
            return;
        }
    }

    QMap<QString, QByteArray> entries;
    bool ok = false;
#if KWALLET_VERSION < QT_VERSION_CHECK(5, 72, 0)
    ok = m_wallet->readEntryList("*", entries) == 0;
#else
    entries = m_wallet->entriesList(&ok);
#endif
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

    if (migrate) {
        if (!m_wallet->setFolder("Falkon")) {
            qWarning() << "KWalletPasswordBackend::initialize Cannot set folder \"Falkon\"!";
            return;
        }

        for (const PasswordEntry &entry : qAsConst(m_allEntries)) {
            m_wallet->writeEntry(entry.id.toString(), encodeEntry(entry));
        }
    }
}

KWalletPasswordBackend::~KWalletPasswordBackend()
{
    delete m_wallet;
}
