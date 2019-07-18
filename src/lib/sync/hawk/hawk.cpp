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

#include "hawk.h"
#include "synccrypto.h"

#include <QByteArray>
#include <QString>
#include <QCryptographicHash>
#include <QUrl>
#include <QDateTime>
#include <QDebug>

#include <nettle/hmac.h>
#include <nettle/sha2.h>

HawkOptions::HawkOptions(const char* app, const char* dlg, const char* ext, const char* contentType, const char* hash, const char* localTimeOffset, const char* nonce, const char* payload, const char* timestamp)
{
    m_app = new QByteArray(app);
    m_dlg = new QByteArray(dlg);
    m_ext = new QByteArray(ext);
    m_contentType = new QByteArray(contentType);
    m_hash = new QByteArray(hash);
    m_localTimeOffset = new QByteArray(localTimeOffset);
    m_nonce = new QByteArray(nonce);
    m_payload = new QByteArray(payload);
    m_timestamp = new QByteArray(timestamp);
}

HawkOptions::~HawkOptions()
{
    delete m_app;
    delete m_dlg;
    delete m_ext;
    delete m_contentType;
    delete m_hash;
    delete m_localTimeOffset;
    delete m_nonce;
    delete m_payload;
    delete m_timestamp;
}

HawkArtifacts::HawkArtifacts(const char* app, const char* dlg, const char* ext, const char* hash, const char* host, const char* method, const char* nonce, const quint16 port, const char* resource, quint64 timestamp)
{
    m_app = new QByteArray(app);
    m_dlg = new QByteArray(dlg);
    m_ext = new QByteArray(ext);
    m_hash = new QByteArray(hash);
    m_host = new QByteArray(host);
    m_method = new QByteArray(method);
    m_nonce = new QByteArray(nonce);
    m_resource = new QByteArray(resource);
    if (port) {
        QString tempPort = QString("%1").arg(port);
        m_port = new QByteArray(tempPort.toUtf8().data());
    } else {
        m_port = new QByteArray();
    }
    if (timestamp) {
        QString tempTS = QString("%1").arg(timestamp);
        m_timestamp = new QByteArray(tempTS.toUtf8().data());
    } else {
        m_timestamp = new QByteArray();
    }
}

HawkArtifacts::~HawkArtifacts()
{
    delete m_app;
    delete m_dlg;
    delete m_ext;
    delete m_hash;
    delete m_host;
    delete m_method;
    delete m_nonce;
    delete m_port;
    delete m_timestamp;
}

HawkHeader::HawkHeader(const char* url, const char* method, const char* id, const char* key, size_t keyLen, HawkOptions* option)
{
    qint64 ts = QDateTime::currentSecsSinceEpoch();

    QString *hash = new QString(option->m_hash->data());
    QString *payload = new QString(option->m_payload->data());
    QString *timestamp = new QString(option->m_timestamp->data());

    QUrl uri = QUrl(QString(url));
    QString *resource = (uri.query().length() == 0) ?
                        (new QString(uri.path())) :
                        (new QString(uri.path() + QString("?") + uri.query()));

    QString *nonce = nullptr;
    if (option->m_nonce->length() > 0) {
        nonce = new QString(option->m_nonce->data());
    } else {
        u_char *bytes = new u_char(NONCE_LEN / 2);
        generateRandomBytes(nullptr, NONCE_LEN / 2, bytes);
        QByteArray temp = QByteArray::fromRawData((const char *)bytes, NONCE_LEN / 2);
        nonce = new QString(temp.toHex().data());
        delete bytes;
    }

    if (timestamp->length() > 0) {
        QString *localTimeOffset = new QString(option->m_localTimeOffset->data());
        quint64 offset = 0;
        if (localTimeOffset->length() > 0) {
            offset = localTimeOffset->toInt(nullptr, 10);
        }
        ts = timestamp->toInt(nullptr, 10) + offset;
        delete localTimeOffset;
    }

    if (hash->length() == 0 && payload->length() > 0) {
        QByteArray *contentType = nullptr;
        if (option) {
            contentType = new QByteArray(option->m_contentType->data());
        } else {
            contentType = new QByteArray("text/plain");
        }

        QByteArray *tempPayload = new QByteArray();
        tempPayload->append(option->m_payload->data());
        QByteArray *tempHash = hawkComputePayloadHash(tempPayload, contentType);
        hash = new QString(tempHash->data());

        delete contentType;
        delete tempPayload;
        delete tempHash;
    }

    qint64 defaultPort = 0;
    if (uri.scheme() == QLatin1String("http")) {
        defaultPort = 80;
    } else if (uri.scheme() == QLatin1String("https")) {
        defaultPort = 443;
    }
    qint64 port = uri.port(defaultPort);

    m_artifacts = new HawkArtifacts(option ? option->m_app->data() : nullptr,
                                    option ? option->m_dlg->data() : nullptr,
                                    option ? option->m_ext->data() : nullptr,
                                    hash->toUtf8().data(),
                                    uri.host().toUtf8().data(),
                                    method,
                                    nonce->toUtf8().data(),
                                    port,
                                    resource->toUtf8().data(),
                                    ts);

    QByteArray *header = new QByteArray(QString("Hawk id=\"%1\", ts=\"%2\", nonce=\"%3\"").arg(id).arg(m_artifacts->m_timestamp->data()).arg(m_artifacts->m_nonce->data()).toUtf8().data());

    if (m_artifacts->m_hash->length() > 0) {
        QByteArray *name = new QByteArray("hash");
        QByteArray *value = new QByteArray(m_artifacts->m_hash->data());
        header = hawkAppendToHeader(header, name, value);
        delete name;
        delete value;
    }

    if (m_artifacts->m_ext->length() > 0) {
        QString ext(m_artifacts->m_ext->data());
        ext.replace(QString("\\"), QString("\\\\"));
        ext.replace(QString("\n"), QString("\\n"));

        QByteArray *name = new QByteArray("ext");
        QByteArray *value = new QByteArray(ext.toUtf8().data());
        header = hawkAppendToHeader(header, name, value);
        delete name;
        delete value;
    }

    if (true) {
        QByteArray tempType("header");
        QByteArray tempKey(key);
        QByteArray *tempMac = hawkComputeMac(&tempType, &tempKey, keyLen, m_artifacts);
        QByteArray *mac = new QByteArray(tempMac->toBase64(QByteArray::Base64Encoding).data());
        QByteArray *name = new QByteArray("mac");
        header = hawkAppendToHeader(header, name, mac);
        delete name;
        delete tempMac;
        delete mac;
    }

    if (m_artifacts->m_app->length() > 0) {
        QByteArray *name = new QByteArray("app");
        QByteArray *value = new QByteArray(m_artifacts->m_app->data());
        header = hawkAppendToHeader(header, name, value);
        delete name;
        delete value;

        if (m_artifacts->m_dlg->length() > 0) {
            name = new QByteArray("dlg");
            value = new QByteArray(m_artifacts->m_dlg->data());
            header = hawkAppendToHeader(header, name, value);
            delete name;
            delete value;
        }
    }

    m_header = new QByteArray(header->data());

    delete hash;
    delete payload;
    delete timestamp;
    delete resource;
    delete nonce;
    delete header;
}

HawkHeader::~HawkHeader()
{
    delete m_artifacts;
    delete m_header;
}

QByteArray * HawkHeader::hawkParseContentType(QByteArray* contentType)
{
    QByteArray temp;
    int index = contentType->indexOf(';');
    temp.append(contentType->data());
    if (index > 0) {
        temp.truncate(index);
    }
    QByteArray *ret = new QByteArray();
    ret->append(temp.toLower());
    return ret;
}

QByteArray * HawkHeader::hawkComputePayloadHash(QByteArray* payload, QByteArray* contentType)
{
    QByteArray *content = hawkParseContentType(contentType);
    QString tempString = QString("hawk.%1.payload\n%2\n%3\n").arg(HAWK_VERSION).arg(content->data()).arg(payload->data());

    QByteArray update(tempString.toUtf8().data());
    size_t length = update.size();
    u_char *digestOut = new u_char[SHA256_DIGEST_SIZE];

    sha256_ctx *ctx = new sha256_ctx;
    sha256_init(ctx);
    sha256_update(ctx, length, (u_char *)(update.data()));
    sha256_digest(ctx, SHA256_DIGEST_SIZE, digestOut);

    QByteArray *outTemp = new QByteArray(QByteArray::fromRawData((const char *)digestOut, SHA256_DIGEST_SIZE));
    QByteArray *out = new QByteArray(outTemp->toBase64().data());

    delete ctx;
    delete content;
    delete outTemp;

    return out;
}

QByteArray * HawkHeader::hawkAppendToHeader(QByteArray* header, QByteArray* name, QByteArray* value)
{
    QString tempString = QString("%1, %2=\"%3\"").arg(header->data()).arg(name->data()).arg(value->data());
    QByteArray *out = new QByteArray(tempString.toUtf8().data());
    return out;
}

QByteArray * HawkHeader::hawkNormalizeString(QByteArray* type, HawkArtifacts* artifact)
{
    QString host(artifact->m_host->toLower().data());
    QString info = QString("hawk.%1.%2").arg(HAWK_VERSION).arg(type->data());
    QString method(artifact->m_method->toUpper().data());

    QString normalized = info + QString("\n") + QString(artifact->m_timestamp->data())
                         + QString("\n") + QString(artifact->m_nonce->data()) + QString("\n")
                         + method + QString("\n") + QString(artifact->m_resource->data())
                         + QString("\n") + host + QString("\n") + QString(artifact->m_port->data())
                         + QString("\n");

    if (artifact->m_hash->length() > 0) {
        normalized = normalized + QString(artifact->m_hash->data()) + QString("\n");
    } else {
        normalized = normalized + QString("") + QString("\n");
    }

    if (artifact->m_ext->length() > 0) {
        QString ext(artifact->m_ext->data());
        ext.replace(QString("\\"), QString("\\\\"));
        ext.replace(QString("\n"), QString("\\n"));
        normalized = normalized + ext + QString("\n");
    } else {
        normalized = normalized + QString("\n");
    }

    if (artifact->m_app->length() > 0) {
        normalized = normalized + QString(artifact->m_app->data()) + QString("\n");

        if (artifact->m_dlg->length() > 0) {
            normalized = normalized + QString(artifact->m_dlg->data()) + QString("\n");
        }
    }

    QByteArray *out = new QByteArray(normalized.toUtf8().data());
    return out;
}

QByteArray * HawkHeader::hawkComputeMac(QByteArray* type, QByteArray* key, size_t keyLen, HawkArtifacts* artifact)
{
    QByteArray *normalized = hawkNormalizeString(type, artifact);

    hmac_sha256_ctx *ctx = new hmac_sha256_ctx;
    hmac_sha256_set_key(ctx, keyLen, (u_char *)key->data());
    hmac_sha256_update(ctx, normalized->length(), (u_char *)normalized->data());
    u_char *out = new u_char(SHA256_DIGEST_SIZE);
    hmac_sha256_digest(ctx, SHA256_DIGEST_SIZE, out);
    QByteArray *mac = new QByteArray(QByteArray::fromRawData((const char *)out, SHA256_DIGEST_SIZE));

    return mac;
}
