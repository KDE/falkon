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
#include <QString>
#include <nettle/rsa.h>

#define SHA256_DIGEST_SIZE 32

struct RSAKeyPair {
    rsa_public_key m_publicKey;
    rsa_private_key m_privateKey;
};

void generateRandomBytes(void *randomCtx, size_t numBytes, u_char *out);
u_char *syncCryptoHkdf(QByteArray *in, QByteArray *info, size_t out_len);
void syncCryptoKW(QByteArray *kw, QString name);
void deriveSessionToken(QByteArray *sessionToken, QByteArray *tokenId, QByteArray *reqHMACKey, QByteArray *reqKey);
void deriveKeyFetchToken(QByteArray *keyFetchToken, QByteArray *tokenId, QByteArray *reqHMACKey, QByteArray *respHMACKey, QByteArray *respXORKey);
RSAKeyPair  *generateRSAKeyPair();
