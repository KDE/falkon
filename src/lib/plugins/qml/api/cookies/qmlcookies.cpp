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
#include "qml/qmlstaticdata.h"
#include <QQmlEngine>

QmlCookies::QmlCookies(QObject *parent)
    : QObject(parent)
{
    connect(mApp->cookieJar(), &CookieJar::cookieAdded, this, [this](const QNetworkCookie &network_cookie){
        QmlCookie *cookie = QmlStaticData::instance().getCookie(network_cookie);
        QVariantMap map;
        map.insert(QSL("cookie"), QVariant::fromValue(cookie));
        map.insert(QSL("removed"), false);
        Q_EMIT changed(map);
    });

    connect(mApp->cookieJar(), &CookieJar::cookieRemoved, this, [this](const QNetworkCookie &network_cookie){
        QmlCookie *cookie = QmlStaticData::instance().getCookie(network_cookie);
        QVariantMap map;
        map.insert(QSL("cookie"), QVariant::fromValue(cookie));
        map.insert(QSL("removed"), true);
        Q_EMIT changed(map);
    });
}

QNetworkCookie QmlCookies::getNetworkCookie(const QVariantMap &map)
{
    if (!map.contains(QSL("name")) || !map.contains(QSL("url"))) {
        qWarning() << "Error:" << "Wrong arguments passed to" << __FUNCTION__;
        return QNetworkCookie();
    }
    const QString name = map.value(QSL("name")).toString();
    const QString url = map.value(QSL("url")).toString();
    QVector<QNetworkCookie> cookies = mApp->cookieJar()->getAllCookies();
    for (const QNetworkCookie &cookie : qAsConst(cookies)) {
        if (cookie.name() == name && cookie.domain() == url) {
            return cookie;
        }
    }
    return QNetworkCookie();
}

QmlCookie *QmlCookies::get(const QVariantMap &map)
{
    QNetworkCookie netCookie = getNetworkCookie(map);
    return QmlStaticData::instance().getCookie(netCookie);
}

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
            QmlCookie *qmlCookie = QmlStaticData::instance().getCookie(cookie);
            qmlCookies.append(qmlCookie);
        }
    }
    return qmlCookies;
}

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

void QmlCookies::remove(const QVariantMap &map)
{
    QNetworkCookie netCookie = getNetworkCookie(map);
    mApp->webProfile()->cookieStore()->deleteCookie(netCookie);
}
