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

#include <nettle/sha2.h>
#include <nettle/hkdf.h>
#include <nettle/hmac.h>
#include <nettle/rsa.h>
#include <nettle/cbc.h>

#include <QByteArray>
#include <QString>

#include <random>

/* This function is required for Nettle's RSA support.
 * We are not using random_ctx, instead we are using C++11's <random>'s
 * Merseinne Twister.
 */
void generateRandomBytes(void* randomCtx, size_t numBytes, u_char* out)
{
    Q_UNUSED(randomCtx);

    std::random_device device;
    std::mt19937 generator(device());

    // Generate a uniform int distribution in range of 0 to 255, that is 8-bit range
    std::uniform_int_distribution<std::mt19937::result_type> distribution(0, 255);

    for (size_t i = 0; i < numBytes; ++i) {
        out[i] = (u_char)distribution(generator);
    }
}


u_char *syncCryptoHkdf(QByteArray *in, QByteArray *info, size_t out_len)
{
    hmac_sha256_ctx *ctx = new hmac_sha256_ctx;

    size_t in_len = in->size();
    size_t info_len = info->size();
    u_char *salt = new u_char[SHA256_DIGEST_SIZE] {0};
    u_char *prk = new u_char[SHA256_DIGEST_SIZE];
    u_char *output = new u_char[out_len];

    nettle_hmac_sha256_set_key(ctx, SHA256_DIGEST_SIZE, salt);
    nettle_hkdf_extract(ctx,
                        (nettle_hash_update_func *)hmac_sha256_update,
                        (nettle_hash_digest_func *)hmac_sha256_digest,
                        SHA256_DIGEST_SIZE,
                        in_len, (u_char *)in->data(), prk);
    nettle_hmac_sha256_set_key(ctx, SHA256_DIGEST_SIZE, prk);
    nettle_hkdf_expand(ctx,
                       (nettle_hash_update_func *)hmac_sha256_update,
                       (nettle_hash_digest_func *)hmac_sha256_digest,
                       SHA256_DIGEST_SIZE,
                       info_len, (u_char *)info->data(), out_len, output);

    delete[] prk;
    return output;
}

void syncCryptoKW(QByteArray *kw, QString name)
{
    // Concatenate "name" to Mozilla prefix to get the required KW.
    // See https://raw.githubusercontent.com/wiki/mozilla/fxa-auth-server/images/onepw-create.png for details.
    QString info = QString("identity.mozilla.com/picl/v1/").append(name);
    kw->append(info.toUtf8());
}

void deriveSessionToken(QByteArray *sessionToken, QByteArray *tokenId, QByteArray *reqHMACKey, QByteArray *reqKey)
{
    QByteArray *kw = new QByteArray();
    syncCryptoKW(kw, QString("sessionToken"));
    size_t len = 32;

    u_char *out = syncCryptoHkdf(sessionToken, kw, 3 * len);
    QByteArray *temp = new QByteArray(QByteArray::fromRawData((const char *)out, 3 * len));
    QByteArray tId(*temp);
    tId.remove(len, 2 * len);
    tokenId->append(tId);
    reqKey->append(temp->right(len));
    temp->remove(0, len);
    temp->remove(len, len);
    reqHMACKey->append(*temp);
}

void deriveKeyFetchToken(QByteArray *keyFetchToken, QByteArray *tokenId, QByteArray *reqHMACKey, QByteArray *respHMACKey, QByteArray *respXORKey)
{
    QByteArray *infoKft = new QByteArray();
    syncCryptoKW(infoKft, "keyFetchToken");
    QByteArray *infoKeys = new QByteArray();
    syncCryptoKW(infoKeys, "account/keys");
    size_t len = 32;

    u_char *out1 = syncCryptoHkdf(keyFetchToken, infoKft, 3 * len);
    QByteArray *temp = new QByteArray(QByteArray::fromRawData((const char *)out1, 3 * len));
    QByteArray tId(*temp);
    tId.remove(len, 2 * len);
    tokenId->append(tId);
    QByteArray *reqKey = new QByteArray();
    reqKey->append(temp->right(len));
    temp->remove(0, len);
    temp->remove(len, len);
    reqHMACKey->append(*temp);

    u_char *out2 = syncCryptoHkdf(reqKey, infoKeys, 3 * len);
    QByteArray *temp2 = new QByteArray(QByteArray::fromRawData((const char *)out2, 3 * len));
    QByteArray respHKey(*temp2);
    respHKey.remove(len, 2 * len);
    respHMACKey->append(respHKey);
    temp2->remove(0, len);
    respXORKey->append(*temp2);
}

RSAKeyPair  *generateRSAKeyPair()
{
    RSAKeyPair *keyPair = new RSAKeyPair();
    rsa_public_key publicKey;
    rsa_private_key privateKey;

    rsa_public_key_init(&publicKey);
    rsa_private_key_init(&privateKey);

    // Set public key exponent to a small Fermat prime number
    mpz_set_ui(publicKey.e, 65537);

    // Using key size 2048 for security
    int success = rsa_generate_keypair(&publicKey, &privateKey,
                                       NULL, generateRandomBytes,
                                       NULL, NULL, 2048, 0);

    Q_ASSERT(success);

    keyPair->m_publicKey = publicKey;
    keyPair->m_privateKey = privateKey;

    return keyPair;
}
