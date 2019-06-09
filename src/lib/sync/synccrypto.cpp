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

#include "synccrypto.h"

#include <openssl/evp.h>
#include <openssl/kdf.h>

#include <QByteArray>

HKDF::HKDF(const QByteArray key, const QByteArray salt, const QByteArray info)
{
    pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF,NULL);

    init(key, salt,info);
}

HKDF::~HKDF() {
}

void HKDF::init(const QByteArray key, const QByteArray salt, const QByteArray info)
{
    size_t keylen = key.size();
    size_t saltlen = salt.size();
    size_t infolen = info.size();

    if (EVP_PKEY_derive_init(pctx) <= 0) {
        qWarning("Unable to initialize public key algorithm context for HKDF.");
        return;
    }
    if (EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha256()) <= 0) {
        qWarning("Unable to set HKDF message digest.");
        return;
    }
    if (EVP_PKEY_CTX_set1_hkdf_salt(pctx, (uchar *)salt.data(), saltlen) <= 0) {
        qWarning("Unable to set salt.");
        return;
    }
    if (EVP_PKEY_CTX_set1_hkdf_key(pctx, (uchar *)key.data(), keylen) <= 0) {
        qWarning("Unable to set key.");
        return;
    }
    if (EVP_PKEY_CTX_add1_hkdf_info(pctx, (uchar *)info.data(), infolen) <= 0) {
        qWarning("Uable to set info for HKDF.");
        return;
    }
}

QByteArray HKDF::getKey(size_t outlen) {
    QByteArray out;
    if (EVP_PKEY_derive(pctx, (uchar *)out.data(), &outlen) <= 0) {
        qWarning("Unable to derive HKDF key.");
    }
    return out;
}

