/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 Prasenjit Kumar Shaw <shawprasenjit07@gmail.com>
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
#include "hawk.h"

#include <QVariant>
#include <QHash>
#include <QObject>

class QDateTime;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QByteArray;

class SyncCredentials
{
public:
    SyncCredentials();      // load the sync credentials from settings
    ~SyncCredentials();
    void addSyncCredentials(QString key, QString value);
    QString getValue(QString key);
};

class SyncState
{
public:
    SyncState();
    ~SyncState();
    void saveSyncState(bool syncSuccess);
    bool isInitialSync();
    QDateTime lastSyncTime();

private:
    bool m_isInitialSync;
    QDateTime *m_lastSyncTime;
};


class SyncManager : public QObject
{
public:
    explicit SyncManager(QObject *parent = nullptr);      // load m_syncCreds
    ~SyncManager();
    void sync();    // call it from fxalogin.cpp/mainapplication.cpp after recieving sessionToken
    void saveSyncState();

public Q_SLOTS:
    void startSync();

private:
    bool getBrowserSignedCertificate();
    bool getCryptoKeys();

    void createHawkPostReqeuest(QString endpoint, QByteArray *id, QByteArray *key, size_t keyLen, QByteArray *data);
    void createHawkGetRequest(QString endpoint, QByteArray *id, QByteArray *key, size_t keyLen);


    bool m_storageCredentialsExpired = true;
    bool m_syncSuccess = false;
    bool m_syncReady = false;

    const QString m_FxAServerUrl = QString("https://api.accounts.firefox.com/v1");

    QNetworkAccessManager *m_networkManager;
    SyncCredentials *m_syncCreds;
    SyncState *m_syncState;
};
