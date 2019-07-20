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
#include "synccrypto.h"
#include "syncutils.h"
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
}

void SyncState::saveSyncState(bool syncSuccess)
{
    if (syncSuccess) {
        *m_lastSyncTime = QDateTime::currentDateTime();
        m_isInitialSync = false;
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
    m_keyPair = generateRSAKeyPair();
    m_browserCertificate = new QByteArray();
    
    testDeriveMasterKey();
}

SyncManager::~SyncManager()
{
    saveSyncState();
    m_networkManager->deleteLater();
    delete m_syncCreds;
    delete m_syncState;
    delete m_keyPair;
    delete m_browserCertificate;
}

void SyncManager::saveSyncState()
{
    m_syncState->saveSyncState(m_syncSuccess);
}

void SyncManager::sync()
{
    switch(m_syncStep) {
        case enum_ERROR:
            qWarning() << "Error occured in Sync Process.";
            m_syncSuccess = false;
            saveSyncState();
            break;
        case enum_FetchBrowserId:
            getBrowserSignedCertificate();
            break;
        case enum_TradeBrowserIdAssertion:
            tradeBrowserIdAssertion();
            break;
        case enum_FetchAccountKeys:
            getCryptoKeys();
            break;
        case enum_UploadDevice:
            uploadDevice();
            break;
        case enum_NoRequestPending:
            qDebug() << "No sync requests pending.";
            m_syncSuccess = true;
            saveSyncState();
            break;
        default:
            saveSyncState();
            break;
    }
}

void SyncManager::startSync()
{
    if(m_syncState->isInitialSync()) {
        qDebug() << "is initial sync, going to uploadDevice";
        m_syncStep = enum_UploadDevice;
    } else {
        qDebug() << "not initial sync, going to fetchBrowserId";
        m_syncStep = enum_FetchBrowserId;
    }
    sync();
}

void SyncManager::getBrowserSignedCertificate()
{
    QString endpoint("certificate/sign");

    QByteArray *sessionToken = new QByteArray(QByteArray::fromHex(m_syncCreds->getValue("SessionToken").toUtf8().data()));
    QByteArray *tokenId = new QByteArray();
    QByteArray *requestHMACKey = new QByteArray();
    QByteArray *requestKey = new QByteArray();
    deriveSessionToken(sessionToken, tokenId, requestHMACKey, requestKey);

    QByteArray *tokenIdHex = new QByteArray(tokenId->toHex().data());

    char *n;
    char *e;
    n = mpz_get_str(nullptr, 10, m_keyPair->m_publicKey.n);
    e = mpz_get_str(nullptr, 10, m_keyPair->m_publicKey.e);

    QJsonObject objBody;
    QJsonObject objKey;
    objBody.insert("duration", 1 * 60 * 60 * 1000); // 1 day
    objKey.insert("algorithm", "RS");
    objKey.insert("n", n);
    objKey.insert("e", e);
    objBody.insert("publicKey", objKey);

    QJsonDocument doc(objBody);
    QByteArray *data = new QByteArray(doc.toJson(QJsonDocument::Compact));

    QNetworkRequest request = createHawkPostReqeuest(endpoint, tokenIdHex, requestHMACKey, 32, data);

    QNetworkReply *reply = m_networkManager->post(request, *data);
    connect(reply, &QNetworkReply::finished, this, &SyncManager::callback_getBrowserSignedCertificate);

    qDebug() << "Hawk POST Request sent to /certificate/sign endpoint.";

    delete sessionToken;
    delete tokenId;
    delete requestHMACKey;
    delete requestKey;
    delete tokenIdHex;
    delete n;
    delete e;
    delete data;
}

void SyncManager::callback_getBrowserSignedCertificate()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        m_syncStep = enum_ERROR;
        sync();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in receiving Browser Signed Certificate for Firefox Sync.";
        reply->close();
        reply->deleteLater();
        m_syncStep = enum_ERROR;
        sync();
        return;
    }

    QByteArray response(reply->readAll().data());
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();

    if (obj.contains(QString("cert"))) {
        QByteArray certificate(obj.value(QString("cert")).toString().toUtf8());
        if (verifyBrowserSignedCertificate(&certificate)) {
            qDebug() << "Valid Browser Signed Certificate Response.";
            m_storageCredentialsExpired = false;
            m_browserCertificate->clear();
            m_browserCertificate->append(certificate.data());
            if (m_syncState->isInitialSync()) {
                m_syncStep = enum_FetchAccountKeys;
            } else {
                m_syncStep = enum_TradeBrowserIdAssertion;
            }
        } else {
            qWarning() << "Invalid Browser ID Assertion Response Received.";
            m_syncStep = enum_ERROR;
        }
    } else {
        qWarning() << "Invalid Browser ID Assertion Response Received.";
        m_syncStep = enum_ERROR;
    }

    reply->deleteLater();
    sync();
}

bool SyncManager::verifyBrowserSignedCertificate(QByteArray* certificate)
{
    QList<QByteArray> list = certificate->split('.');

    QByteArray header(QByteArray::fromBase64(list[0]));
    QByteArray payload(QByteArray::fromBase64(list[1]));
    qDebug() << "header: " << header << "\n" << "payload: " << payload;

    if (!header.size() || !payload.size()) {
        qDebug() << "header or payoad empty";
        return false;
    }

    QJsonDocument headerDoc = QJsonDocument::fromJson(header);
    QJsonObject headerJson = headerDoc.object();
    QJsonDocument payloadDoc = QJsonDocument::fromJson(payload);
    QJsonObject payloadJson = payloadDoc.object();

    if (!headerJson.contains("alg") || !payloadJson.contains("principal")) {
        qDebug() << "invalid header or payload";
        return false;
    }
    if (headerJson.value("alg").toString() != QString("RS256")) {
        return false;
    }

    QJsonObject princial = payloadJson.value("principal").toObject();
    QString email = princial.value("email").toString();
    QString uid = m_syncCreds->getValue("UID");
    QString expected = QSL("%1@%2").arg(uid).arg(QUrl(m_FxAServerUrl).host());
    if (expected != email) {
        return false;
    }
    return true;
}

void SyncManager::tradeBrowserIdAssertion()
{
    
    m_syncStep = enum_NoRequestPending;
    sync();
}

void SyncManager::getCryptoKeys()
{
    qDebug() << "getCryptoKeys";
    QString endpoint("account/keys");
    QByteArray *keyFetchToken = new QByteArray(QByteArray::fromHex(m_syncCreds->getValue("KeyFetchToken").toUtf8().data()));
    QByteArray *tokenId = new QByteArray();
    QByteArray *reqHMACKey = new QByteArray();
    QByteArray *respHMACKey = new QByteArray();
    QByteArray *respXorKey = new QByteArray();
    deriveKeyFetchToken(keyFetchToken, tokenId, reqHMACKey, respHMACKey, respXorKey);
    
    QByteArray respHMACKeyHex(respHMACKey->toHex().data());
    QByteArray reqHMACKeyHex(reqHMACKey->toHex().data());
    QByteArray respXorKeyHex(respXorKey->toHex().data());
    
    m_syncCreds->addSyncCredentials(QString("RespHMACKey"), QString(respHMACKeyHex.data()));
    m_syncCreds->addSyncCredentials(QString("ReqHMACKey"), QString(reqHMACKeyHex.data()));
    m_syncCreds->addSyncCredentials(QString("RespXORKey"), QString(respXorKeyHex.data()));
    qDebug() << "derived keyFetchToken";
    QByteArray *tokenIdHex = new QByteArray(tokenId->toHex().data());

    QNetworkRequest request = createHawkGetRequest(endpoint, tokenIdHex, reqHMACKey, 32);
    qDebug() << "Request created";
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &SyncManager::callback_getCryptoKeys);

    qDebug() << "Hawk POST Request sent to /account/keys endpoint.";
    
    delete keyFetchToken;
    delete tokenId;
    delete reqHMACKey;
    delete respHMACKey;
    delete respXorKey;
    delete tokenIdHex;
}

void SyncManager::callback_getCryptoKeys()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "Received reply";
    if (!reply) {
        qDebug()<< "No reply";
        m_syncStep = enum_ERROR;
        sync();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in receiving Crypto Keys for Firefox Sync.";
        qDebug() << reply->readAll();
        reply->close();
        reply->deleteLater();
        m_syncStep = enum_ERROR;
        sync();
        return;
    }

    qDebug() << "received bundle";
    QByteArray response(reply->readAll().data());
    qDebug() << "account/keys response:\n" << response;
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();

    if (obj.contains(QString("bundle"))) {
        QByteArray *bundle = new QByteArray(obj.value(QString("bundle")).toString().toUtf8());
        qDebug() << "bundle: " << bundle->data();
        m_syncCreds->addSyncCredentials(QString("Bundle"), QString(bundle->data()));
        QByteArray *ka = new QByteArray();
        QByteArray *kb = new QByteArray();
        QByteArray *unwrapKb = new QByteArray(QByteArray::fromHex(m_syncCreds->getValue(QString("UnwrapBKey")).toUtf8()));
        QByteArray *respHMACKey = new QByteArray(QByteArray::fromHex(m_syncCreds->getValue(QString("RespHMACKey")).toUtf8()));
        QByteArray *respXorKey = new QByteArray(QByteArray::fromHex(m_syncCreds->getValue(QString("RespXORKey")).toUtf8()));
        if(!deriveMasterKey(bundle, respHMACKey, respXorKey, unwrapKb, ka, kb)) {
            qWarning() << "Failed to retrieve Sync Key.";
            m_syncStep = enum_ERROR;
        } else {
            m_syncCreds->addSyncCredentials(QString("MasterKey"), QString(kb->toHex().data()));
            m_syncStep = enum_TradeBrowserIdAssertion;
        }
        delete bundle;
        delete ka;
        delete kb;
        delete unwrapKb;
        delete respHMACKey;
        delete respXorKey;
    } else {
        qWarning() << "Invalid Crypto Keys Response Recieved.";
        m_syncStep = enum_ERROR;
    }

    reply->deleteLater();
    sync();
}


void SyncManager::uploadDevice()
{
    QString endpoint("account/device");
    
    QJsonObject obj;
    obj.insert(QString("name"), QString("Falkon"));
    obj.insert(QString("type"), QString("desktop"));
    QJsonDocument doc(obj);
    QByteArray *data = new QByteArray(doc.toJson(QJsonDocument::Compact));
    
    QByteArray *sessionToken = new QByteArray(QByteArray::fromHex(m_syncCreds->getValue("SessionToken").toUtf8().data()));
    QByteArray *tokenId = new QByteArray();
    QByteArray *requestHMACKey = new QByteArray();
    QByteArray *requestKey = new QByteArray();
    deriveSessionToken(sessionToken, tokenId, requestHMACKey, requestKey);

    QByteArray *tokenIdHex = new QByteArray(tokenId->toHex().data());
    
    QNetworkRequest request = createHawkPostReqeuest(endpoint, tokenIdHex, requestHMACKey, 32, data);
    QNetworkReply *reply = m_networkManager->post(request, *data);
    connect(reply, &QNetworkReply::finished, this, &SyncManager::callback_uploadDevice);
    
    qDebug() << "Hawk POST Request sent to /account/device endpoint.";

    
    delete sessionToken;
    delete tokenId;
    delete requestHMACKey;
    delete requestKey;
    delete tokenIdHex;
    delete data;
}

void SyncManager::callback_uploadDevice()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "Received reply";
    if (!reply) {
        qDebug()<< "No reply";
        m_syncStep = enum_ERROR;
        sync();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in uploading Device Name to Firefox Sync.";
        qDebug() << reply->readAll();
        reply->close();
        reply->deleteLater();
        m_syncStep = enum_ERROR;
        sync();
        return;
    }

    QByteArray response(reply->readAll().data());
    qDebug() << "account/device response:\n" << response;
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();

    if (obj.contains(QString("id"))) {
        QString id = obj.value(QString("id")).toString();
        qDebug() << "id of device:" << id;
        m_syncCreds->addSyncCredentials(QString("DeviceId"), id);
        m_syncStep = enum_FetchBrowserId;
    } else {
        qWarning() << "Invalid Upload Device Response Recieved.";
        m_syncStep = enum_ERROR;
    }

    reply->deleteLater();
    sync();
}

QNetworkRequest SyncManager::createHawkGetRequest(QString endpoint, QByteArray* id, QByteArray* key, size_t keyLen)
{
    QString url = m_FxAServerUrl + QString("/") + endpoint;

    QNetworkRequest request;
    request.setUrl(QUrl(url.toUtf8()));

    HawkOptions *nullOption = new HawkOptions(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    HawkHeader *header = new HawkHeader(url.toUtf8(), "GET", id->data(), key->data(), keyLen, nullOption);

    request.setRawHeader(QByteArray("Authorization"), *(header->m_header));

    delete nullOption;
    delete header;

    return request;
}

QNetworkRequest SyncManager::createHawkPostReqeuest(QString endpoint, QByteArray *id, QByteArray *key, size_t keyLen, QByteArray *data)
{
    QString contentType = QString("application/json; charset=utf-8");

    QString url = m_FxAServerUrl + QString("/") + endpoint;

    QNetworkRequest request;
    request.setUrl(QUrl(url.toUtf8()));

    HawkOptions *options = new HawkOptions(nullptr, nullptr, nullptr, contentType.toUtf8(), nullptr, nullptr, nullptr, data->data(), nullptr);
    HawkHeader *header = new HawkHeader(url.toUtf8(), "POST", id->data(), key->data(), keyLen, options);
    request.setRawHeader(QByteArray("Authorization"), *(header->m_header));
    request.setRawHeader(QByteArray("Content-Type"), QByteArray(contentType.toUtf8()));
    QByteArray contentLength(QSL("%1").arg(data->size()).toUtf8());
    request.setRawHeader(QByteArray("Content-Length"), QByteArray(contentLength));

    delete options;
    delete header;

    return request;
}
