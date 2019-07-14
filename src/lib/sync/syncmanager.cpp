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
#include "syncmanager.h"
#include "syncrequest.h"
#include "synccrypto.h"
#include "settings.h"
#include "hawk.h"
#include "mainapplication.h"
#include "networkmanager.h"

#include <QString>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>


SyncCredentials::SyncCredentials()
{
}

SyncCredentials::~SyncCredentials()
{
    qDebug() << "Deleting SyncCreds...";
}

void SyncCredentials::addSyncCredentials(QString key, QString value)
{
    Settings settings;
    settings.beginGroup(QSL("SyncCredentials"));
    settings.setValue(key, value);
    settings.endGroup();
}


QString SyncCredentials::getValue(QString key)
{
    Settings settings;
    settings.beginGroup(QSL("SyncCredentials"));
    QString value = settings.value(key, QString("")).toString();
    settings.endGroup();
    return value;
}


SyncState::SyncState()
{
    m_lastSyncTime = new QDateTime();

    Settings settings;
    settings.beginGroup(QSL("SyncState"));
    m_isInitialSync = settings.value(QSL("IsInitialSync"), true).toBool();
    *m_lastSyncTime = settings.value(QSL("LastSyncTime"), QDateTime()).toDateTime();
    settings.endGroup();
    qDebug() << "Loaded SyncState" << "isInitialSync = " << m_isInitialSync << "   m_lastSyncTime = " << m_lastSyncTime->toString();
}

SyncState::~SyncState()
{
    delete m_lastSyncTime;
    qDebug() << "Deleting SyncState...";
}

void SyncState::saveSyncState(bool syncSuccess)
{
    qDebug() << "Entered saveSyncState  syncSuccess=" << syncSuccess;
    if (syncSuccess) {
        *m_lastSyncTime = QDateTime::currentDateTimeUtc();
        m_isInitialSync = false;
        qDebug() << "lastSyncTime = " << m_lastSyncTime->toString() << "  isInitialSync= " << m_isInitialSync;
        Settings settings;
        settings.beginGroup(QSL("SyncState"));
        settings.setValue(QSL("IsInitialSync"), m_isInitialSync);
        settings.setValue(QSL("LastSyncTime"), *m_lastSyncTime);
        settings.endGroup();
    }
    qDebug() << "SyncState saved...";
}

bool SyncState::isInitialSync()
{
    return m_isInitialSync;
}

QDateTime SyncState::lastSyncTime()
{
    return *m_lastSyncTime;
}

SyncManager::SyncManager(QObject *parent)
    : QObject(parent)
{
    m_syncCreds = new SyncCredentials();
    m_syncState = new SyncState();
    m_networkManager = new QNetworkAccessManager(this);
}

SyncManager::~SyncManager()
{
    qDebug() << "Deleting SyncManager...";
    saveSyncState();
    delete m_syncCreds;
    delete m_syncState;
    //m_networkManager->deleteLater();
}

void SyncManager::saveSyncState()
{
    qDebug() << QSL("saveSyncState(%1)").arg(m_syncSuccess ? "true" : "false");
    m_syncState->saveSyncState(m_syncSuccess);
}

void SyncManager::sync()
{
    bool error = false;
    if (m_storageCredentialsExpired) {
        error = getBrowserSignedCertificate();
        if (!error) {
            m_storageCredentialsExpired = false;
        }
        qDebug() << "Got Browser Certs...";
    }
    /*
     if(true) {
         qDebug() << "Getting Crypto Keys";
         getCryptoKeys();
     }
     */
    if (error) {
        m_syncSuccess = false;
        qDebug() << "Sync Failed";
    } else {
        m_syncSuccess = true;
    }
    saveSyncState();
}

void SyncManager::startSync()
{
    sync();
}

bool SyncManager::getBrowserSignedCertificate()
{
    bool error = false;

    QString endpoint("certificate/sign");

    RSAKeyPair *keyPair = generateRSAKeyPair();
    QByteArray *sessionToken = new QByteArray(m_syncCreds->getValue("SessionToken").toUtf8().data());
    QByteArray *tokenId = new QByteArray();
    QByteArray *requestHMACKey = new QByteArray();
    QByteArray *requestKey = new QByteArray();
    deriveSessionToken(sessionToken, tokenId, requestHMACKey, requestKey);

    QByteArray *tokenIdHex = new QByteArray(tokenId->toHex().data());

    qDebug() << "TokenId hex: " << tokenIdHex->data();

    char *n;
    char *e;
    n = mpz_get_str(nullptr, 10, keyPair->m_publicKey.n);
    e = mpz_get_str(nullptr, 10, keyPair->m_publicKey.e);

    QJsonObject objBody;
    QJsonObject objKey;
    objBody.insert("duration", 1 * 60 * 60 * 1000); // 1 day
    objKey.insert("algorithm", "RS");
    objKey.insert("n", n);
    objKey.insert("e", e);
    objBody.insert("publicKey", objKey);

    QJsonDocument doc(objBody);
    QByteArray *requestBody = new QByteArray(doc.toJson(QJsonDocument::Compact));

    createHawkPostReqeuest(endpoint, tokenIdHex, requestHMACKey, 32, requestBody);

    delete keyPair;
    delete sessionToken;
    delete tokenId;
    delete requestHMACKey;
    delete requestKey;
    delete tokenIdHex;
    delete n;
    delete e;
    delete requestBody;

    return error;
}

bool SyncManager::getCryptoKeys()
{
    QString endpoint("account/keys");
    QByteArray *keyFetchToken = new QByteArray(m_syncCreds->getValue("KeyFetchToken").toUtf8().data());
    QByteArray *tokenId = new QByteArray();
    QByteArray *reqHMACKey = new QByteArray();
    QByteArray *respHMACKey = new QByteArray();
    QByteArray *respXorKey = new QByteArray();
    deriveKeyFetchToken(keyFetchToken, tokenId, reqHMACKey, respHMACKey, respXorKey);

    QByteArray *tokenIdHex = new QByteArray(tokenId->toHex().data());

    createHawkGetRequest(endpoint, tokenIdHex, reqHMACKey, 32);

    delete keyFetchToken;
    delete tokenId;
    delete reqHMACKey;
    delete respHMACKey;
    delete respXorKey;
    delete tokenIdHex;

    return false;
}


void SyncManager::createHawkGetRequest(QString endpoint, QByteArray* id, QByteArray* key, size_t keyLen)
{
    QString url = m_FxAServerUrl + QString("/") + endpoint;

    QNetworkRequest request;
    request.setUrl(QUrl(url.toUtf8()));
    qDebug() << "Hawk GET req";

    HawkOptions *nullOption = new HawkOptions(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    HawkHeader *header = new HawkHeader(url.toUtf8(), "GET", id->data(), key->data(), keyLen, nullOption);

    request.setRawHeader("Authorization", header->m_header->data());

    QList<QByteArray> headerList = request.rawHeaderList();
    for (int i = 0; i < headerList.size(); ++i) {
        QByteArray item = headerList[i];
        qDebug() << "  " << item.data() << " : " << request.rawHeader(item).data();
    }

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [ = ]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Reply Error" << reply->error() << reply->errorString();
            qDebug() << "Reply: " << reply->readAll();
            reply->deleteLater();
            return;
        }
        qDebug() << "Reply: " << reply->readAll();
        reply->deleteLater();
    });

    delete nullOption;
    delete header;
}

void SyncManager::createHawkPostReqeuest(QString endpoint, QByteArray *id, QByteArray *key, size_t keyLen, QByteArray *data)
{
    QString contentType = QString("application/json; charset=utf-8");

    QString url = m_FxAServerUrl + QString("/") + endpoint;

    QNetworkRequest request;
    request.setUrl(QUrl(url.toUtf8()));

    qDebug() << "Creating Hawk Option";
    HawkOptions *options = new HawkOptions(nullptr, nullptr, nullptr, contentType.toUtf8(), nullptr, nullptr, nullptr, data->data(), nullptr);
    qDebug() << "Creating Hawk Header";
    HawkHeader *header = new HawkHeader(url.toUtf8(), "POST", id->data(), key->data(), keyLen, options);
    qDebug() << "Inside createHawkPostReqeuest header data: " << header->m_header->data();
    request.setRawHeader("Authorization", header->m_header->data());
    request.setRawHeader("Content-Type", contentType.toUtf8());
    QByteArray contentLength(QSL("%1").arg(data->size()).toUtf8());
    request.setRawHeader("Content-Length", contentLength);

    qDebug() << "Request content:";
    QList<QByteArray> headerList = request.rawHeaderList();
    for (int i = 0; i < headerList.size(); ++i) {
        QByteArray item = headerList[i];
        qDebug() << "  " << item.data() << " : " << request.rawHeader(item).data();
    }
    qDebug() << "   Data: " << data->data();
    qDebug() << "   Url: " << url;

    QNetworkReply *reply = m_networkManager->post(request, *data);

    connect(reply, &QNetworkReply::finished, this, [ = ]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Reply Error" << reply->error() << reply->errorString();
            qDebug() << "Reply: " << reply->readAll();
            return;
        }
        qDebug() << "Reply: " << reply->readAll();
    });

    delete options;
    delete header;
}
