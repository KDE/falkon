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

#include <QByteArray>

#define HAWK_VERSION 1
#define NONCE_LEN 6

class HawkOptions
{
public:
    HawkOptions(const char *app, const char *dlg, const char *ext, const char *contentType, const char *hash, const char *localTimeOffset, const char *nonce, const char *payload, const char *timestamp);
    ~HawkOptions();

    QByteArray *m_app;
    QByteArray *m_dlg;
    QByteArray *m_ext;
    QByteArray *m_contentType;
    QByteArray *m_hash;
    QByteArray *m_localTimeOffset;
    QByteArray *m_nonce;
    QByteArray *m_payload;
    QByteArray *m_timestamp;
};

class HawkArtifacts
{
public:
    HawkArtifacts(const char *app, const char *dlg, const char *ext, const char *hash, const char *host, const char *method, const char *nonce, const quint16 port, const char *resource, quint64 timestamp);
    ~HawkArtifacts();

    QByteArray *m_app;
    QByteArray *m_dlg;
    QByteArray *m_ext;
    QByteArray *m_hash;
    QByteArray *m_host;
    QByteArray *m_method;
    QByteArray *m_nonce;
    QByteArray *m_port;
    QByteArray *m_resource;
    QByteArray *m_timestamp;
};

class HawkHeader
{
public:
    HawkHeader(const char *url, const char *method, const char *id, const char *key, size_t keyLen, HawkOptions *option);
    ~HawkHeader();

    QByteArray *m_header;
    HawkArtifacts *m_artifacts;

private:
    QByteArray *hawkParseContentType(QByteArray *contentType);
    QByteArray *hawkComputePayloadHash(QByteArray *payload, QByteArray *contentType);
    QByteArray *hawkAppendToHeader(QByteArray *header, QByteArray *name, QByteArray *value);
    QByteArray *hawkNormalizeString(QByteArray *type, HawkArtifacts *artifact);
    QByteArray *hawkComputeMac(QByteArray *type, QByteArray *key, size_t keyLen, HawkArtifacts *artifact);
};


