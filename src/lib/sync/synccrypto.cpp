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
#include "syncutils.h"

#include <nettle/sha2.h>
#include <nettle/hkdf.h>
#include <nettle/hmac.h>
#include <nettle/rsa.h>
#include <nettle/cbc.h>

#include <QByteArray>
#include <QString>
#include <QDebug>
#include <QDateTime>

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

QByteArray *createBowserIdAssertion(QByteArray *certificate, QByteArray *audience, qint64 seconds, RSAKeyPair *keyPair)
{
    QByteArray *assertion = new QByteArray();
    mpz_t signature;
    
    QString tempHeader("{\"alg\": \"RS256\"}");
    QByteArray header = QByteArray(tempHeader.toUtf8());
    header.toBase64();
    
    qint64 expiry = QDateTime::currentMSecsSinceEpoch() + seconds * 1000;
    QString tempBody = QString("{\"exp\": %1, \"aud\": \"%2\"}").arg(expiry).arg(audience->data());
    QByteArray body = QByteArray(tempBody.toUtf8());
    body.toBase64();
    
    QByteArray toSign = QByteArray((QString("%1.%2").arg(header.data()).arg(header.data())).toUtf8());
    u_char *digest = new u_char[SHA256_DIGEST_SIZE];
    sha256_ctx *ctx = new sha256_ctx;
    sha256_init(ctx);
    sha256_update(ctx, toSign.size(), (u_char *)(toSign.data()));
    sha256_digest(ctx, SHA256_DIGEST_SIZE, digest);
    delete ctx;
    
    mpz_init(signature);
    int success = nettle_rsa_sha256_sign_digest_tr(&(keyPair->m_publicKey), &(keyPair->m_privateKey), nullptr, generateRandomBytes, digest, signature);
    Q_ASSERT(success);
    
    size_t expectedSize = (mpz_sizeinbase(signature, 2) + 7) / 8;
    size_t count;
    u_char *sig = new u_char[expectedSize];
    mpz_export(sig, &count, 1, sizeof(quint8), 0, 0, signature);
    Q_ASSERT(count == expectedSize);
    
    QByteArray sigBase64 = QByteArray::fromRawData((const char *)sig, count);
    sigBase64.toBase64();
    
    assertion->append((QString("%1~%2.%3.%4").arg(certificate->data()).arg(header.data()).arg(body.data()).arg(sigBase64.data())).toUtf8());

    mpz_clear(signature);
    delete digest;
    delete sig;
    return assertion;
}


QByteArray *xorQByteArray(QByteArray *a, QByteArray *b, size_t len)
{
    QByteArray *xored = new QByteArray();
    
    char *aData = a->data();
    char *bData = b->data();
    char *xorData = new char[len];
    
    for(int i = 0; i < len; ++i) {
        xorData[i] = aData[i] ^ bData[i];
    }
    xored->append(xorData, len);
    
    return xored;
}

bool deriveMasterKey(QByteArray *bundleHex, QByteArray *respHMACKey, QByteArray *respXORKey, QByteArray *unwrapKb, QByteArray *ka, QByteArray *kb)
{
    bool returnVal = true;
    QByteArray *bundle = new QByteArray(QByteArray::fromHex(bundleHex->data()));
    size_t masterKeyLen = 32;
    QByteArray *cipherText = new QByteArray(bundle->data());
    cipherText->chop(masterKeyLen);
    QByteArray *respHMAC = new QByteArray(bundle->right(masterKeyLen).data());
    
    hmac_sha256_ctx *ctx = new hmac_sha256_ctx;
    hmac_sha256_set_key(ctx, respHMACKey->size(), (u_char *)respHMACKey->data());
    hmac_sha256_update(ctx, cipherText->length(), (u_char *)cipherText->data());
    u_char *out = new u_char[SHA256_DIGEST_SIZE];
    hmac_sha256_digest(ctx, SHA256_DIGEST_SIZE, out);
    QByteArray *respHMAC2 = new QByteArray(QByteArray::fromRawData((const char *)out, SHA256_DIGEST_SIZE));
    
    
    QByteArray mac(respHMAC->data());
    QByteArray mac2(respHMAC2->data());
    qDebug() << "mac: " << mac.toHex().data() << "\ndigest:" << mac2.toHex().data();
    
    if (*respHMAC == *respHMAC2) {
        QByteArray *xored = xorQByteArray(cipherText, respXORKey, 2 * masterKeyLen);
        ka->append(xored->data());
        ka->chop(masterKeyLen);
        QByteArray *wrapKb = new QByteArray(xored->right(masterKeyLen));
        QByteArray *xored2 = xorQByteArray(unwrapKb, wrapKb, masterKeyLen);
        kb->append(xored2->data());
        
        delete xored;
        delete wrapKb;
        delete xored2;
    } else {
        returnVal = false;
    }
    
    delete bundle;
    delete cipherText;
    delete respHMAC;
    delete ctx;
    
    return returnVal;
}


void testDeriveMasterKey()
{
    bool returnVal = true;
    QByteArray bundle(QByteArray::fromHex("ee5c58845c7c9412b11bbd20920c2fddd83c33c9cd2c2de2d66b222613364636fc7e59d854d599f10e212801de3a47c34333f3b838ee3471e0f285649c332bbb4c17f42a0b319bbba327d2b326ad23e937219b4de32e3ec7b3e3f740522ad6ef"));
    size_t masterKeyLen = 32;
    QByteArray cipherText(bundle.data());
    cipherText.chop(masterKeyLen);
    QByteArray respHMAC(bundle.right(masterKeyLen).data());
    
    QByteArray respHMACKey(QByteArray::fromHex("f824d2953aab9faf51a1cb65ba9e7f9e5bf91c8d8fd1ac1c8c2d31853a8a1210"));
    QByteArray respXORKey(QByteArray::fromHex("ce7d7aa77859b2359932970bbe2101f2e80d01faf9191bd5ee52181d2f0b78098281ba8cff3925433a89f7c3095e0c89900a469d60790c833281c4df1a11c763"));
    
    QByteArray unwrapKb(QByteArray::fromHex("de6a2648b78284fcb9ffa81ba95803309cfba7af583c01a8a1a63e567234dd28"));
    
    hmac_sha256_ctx *ctx = new hmac_sha256_ctx;
    hmac_sha256_set_key(ctx, respHMACKey.size(), (u_char *)respHMACKey.data());
    hmac_sha256_update(ctx, cipherText.length(), (u_char *)cipherText.data());
    u_char *out = new u_char[SHA256_DIGEST_SIZE];
    hmac_sha256_digest(ctx, SHA256_DIGEST_SIZE, out);
    QByteArray respHMAC2(QByteArray::fromRawData((const char *)out, SHA256_DIGEST_SIZE));
    
    
    QByteArray mac(respHMAC.data());
    QByteArray mac2(respHMAC2.data());
    qDebug() << "mac: " << mac.toHex().data() << "\ndigest:" << mac2.toHex().data() << "\nequal? " << (respHMAC == respHMAC2);
    
    if (respHMAC == respHMAC2) {
        QByteArray *xored = xorQByteArray(&cipherText, &respXORKey, 2 * masterKeyLen);
        QByteArray ka(xored->data());
        ka.chop(masterKeyLen);
        QByteArray *wrapKb = new QByteArray(xored->right(masterKeyLen));
        QByteArray *xored2 = xorQByteArray(&unwrapKb, wrapKb, masterKeyLen);
        QByteArray kb(xored2->data());
        
        QByteArray expectedKa(QByteArray::fromHex("202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f"));
        QByteArray expectedKb(("a095c51c1c6e384e8d5777d97e3c487a4fc2128a00ab395a73d57fedf41631f0"));
        
        qDebug() << "ka? " << (ka == expectedKa) << "\nkb? " << (kb == expectedKb);
        
        delete xored;
        delete wrapKb;
        delete xored2;
    } else {
        returnVal = false;
    }
    
    delete ctx;
    delete out;
}
