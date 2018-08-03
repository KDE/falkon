/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmlcookies.h"
#include "mainapplication.h"
#include "cookiejar.h"
#include "qwebengineprofile.h"

Q_GLOBAL_STATIC(QmlCookieData, cookieData)

QmlCookies::QmlCookies(QObject *parent)
    : QObject(parent)
{
    connect(mApp->cookieJar(), &CookieJar::cookieAdded, this, [this](QNetworkCookie network_cookie){
        // FIXME: improve this
        QmlCookie *cookie = cookieData->get(&network_cookie);
        QVariantMap map;
        map.insert(QSL("cookie"), QVariant::fromValue(cookie));
        map.insert(QSL("removed"), false);
        emit changed(map);
    });

    connect(mApp->cookieJar(), &CookieJar::cookieRemoved, this, [this](QNetworkCookie network_cookie){
        // FIXME: improve this
        QmlCookie *cookie = cookieData->get(&network_cookie);
        QVariantMap map;
        map.insert(QSL("cookie"), QVariant::fromValue(cookie));
        map.insert(QSL("removed"), true);
        emit changed(map);
    });
}

QNetworkCookie *QmlCookies::getNetworkCookie(const QVariantMap &map)
{
    if (!map.contains(QSL("name")) || !map.contains(QSL("url"))) {
        qWarning() << "Error:" << "Wrong arguments passed to" << __FUNCTION__;
        return nullptr;
    }
    const QString name = map.value(QSL("name")).toString();
    const QString url = map.value(QSL("url")).toString();
    QVector<QNetworkCookie> cookies = mApp->cookieJar()->getAllCookies();
    for (const QNetworkCookie &cookie : qAsConst(cookies)) {
        if (cookie.name() == name && cookie.domain() == url) {
            return new QNetworkCookie(cookie);
        }
    }
    return nullptr;
}

/**
 * @brief Get a cookie
 * @param A JavaScript object containing
 *        - name:
 *          String representing the name of the cookie
 *        - url:
 *          String representing the url of the cookie
 * @return Cookie of type [QmlCookie](@ref QmlCookie)
 *         if such cookie exists, else null
 */
QmlCookie *QmlCookies::get(const QVariantMap &map)
{
    QNetworkCookie *netCookie = getNetworkCookie(map);
    if (!netCookie) {
        return nullptr;
    }
    return cookieData->get(netCookie);
}

/**
 * @brief Get all cookies matching a criteria
 * @param A JavaScript object containing
 *        - name:
 *          String representing the name of the cookie
 *        - url:
 *          String representing the url of the cookie
 *        - path:
 *          String representing the path of the cookie
 *        - secure:
 *          Bool representing if the cookie is secure
 *        - session:
 *          Bool representing if the cookie is a session cookie
 * @return List containing cookies, each of type [QmlCookie](@ref QmlCookie)
 */
QList<QObject*> QmlCookies::getAll(const QVariantMap &map)
{
    QList<QObject*> qmlCookies;
    const QString name = map.value(QSL("name")).toString();
    const QString url = map.value(QSL("url")).toString();
    const QString path = map.value(QSL("path")).toString();
    const bool secure = map.value(QSL("secure")).toBool();
    const bool session = map.value(QSL("session")).toBool();
    QVector<QNetworkCookie> cookies = mApp->cookieJar()->getAllCookies();
    for (QNetworkCookie cookie : qAsConst(cookies)) {
        if ((!map.contains(QSL("name")) || cookie.name() == name)
                && (!map.contains(QSL("url")) || cookie.domain() == url)
                && (!map.contains(QSL("path")) || cookie.path() == path)
                && (!map.contains(QSL("secure")) || cookie.isSecure() == secure)
                && (!map.contains(QSL("session")) || cookie.isSessionCookie() == session)) {
            QmlCookie *qmlCookie = cookieData->get(&cookie);
            qmlCookies.append(qmlCookie);
        }
    }
    return qmlCookies;
}

/**
 * @brief Set a cookie
 * @param A JavaScript object containing
 *        - name:
 *          String representing the name of the cookie
 *        - url:
 *          String representing the name of the cookie
 *        - path:
 *          String representing the path of the cookie
 *        - secure:
 *          Bool representing if the cookie is secure
 *        - expirationDate:
 *          A JavaScript Date object, representing the expiration date of the cookie
 *        - httpOnly:
 *          Bool representing if the cookie is httpOnly
 *        - value:
 *          String representing the value of the cookie
 */
void QmlCookies::set(const QVariantMap &map)
{
    const QString name = map.value(QSL("name")).toString();
    const QString url = map.value(QSL("url")).toString();
    const QString path = map.value(QSL("path")).toString();
    const bool secure = map.value(QSL("secure")).toBool();
    const QDateTime expirationDate = QDateTime::fromMSecsSinceEpoch(map.value(QSL("expirationDate")).toDouble());
    const bool httpOnly = map.value(QSL("httpOnly")).toBool();
    const QString value = map.value(QSL("value")).toString();
    QNetworkCookie cookie;
    cookie.setName(name.toUtf8());
    cookie.setDomain(url);
    cookie.setPath(path);
    cookie.setSecure(secure);
    cookie.setExpirationDate(expirationDate);
    cookie.setHttpOnly(httpOnly);
    cookie.setValue(value.toUtf8());
    mApp->webProfile()->cookieStore()->setCookie(cookie);
}

/**
 * @brief Remove a cookie
 * @param A JavaScript object containing
 *        - name:
 *          String representing the name of the cookie
 *        - url:
 *          String representing the url of the cookie
 */
void QmlCookies::remove(const QVariantMap &map)
{
    QNetworkCookie *netCookie = getNetworkCookie(map);
    if (!netCookie) {
        qWarning() << "Error:" << "Cannot find cookie";
        return;
    }
    mApp->webProfile()->cookieStore()->deleteCookie(*netCookie);
}
