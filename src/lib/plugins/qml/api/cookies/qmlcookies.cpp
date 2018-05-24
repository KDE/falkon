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

QmlCookies::QmlCookies(QObject *parent) :
    QObject(parent)
{
    connect(mApp->cookieJar(), &CookieJar::cookieAdded, this, [=](QNetworkCookie network_cookie){
        QmlCookie *cookie = new QmlCookie(&network_cookie);
        emit changed(cookie, false);
    });

    connect(mApp->cookieJar(), &CookieJar::cookieRemoved, this, [=](QNetworkCookie network_cookie){
        QmlCookie *cookie = new QmlCookie(&network_cookie);
        emit changed(cookie, true);
    });
}

QNetworkCookie *QmlCookies::getNetworkCookie(const QVariantMap &map)
{
    if (!map["name"].isValid() || !map["url"].isValid()) {
        qWarning() << "Error:" << "Wrong arguments passed to" << __FUNCTION__;
        return nullptr;
    }
    QString name = map["name"].toString();
    QString url = map["url"].toString();
    QVector<QNetworkCookie> cookies = mApp->cookieJar()->getAllCookies();
    for (QNetworkCookie cookie : cookies) {
        if (cookie.name() == name && cookie.domain() == url) {
            QNetworkCookie *netCookie = new QNetworkCookie(cookie);
            return netCookie;
        }
    }
    return nullptr;
}

QmlCookie *QmlCookies::get(const QVariantMap &map)
{
    QNetworkCookie *netCookie = getNetworkCookie(map);
    if (!netCookie) {
        return nullptr;
    }
    return new QmlCookie(netCookie);
}

QList<QObject*> QmlCookies::getAll(const QVariantMap &map)
{
    QList<QObject*> qmlCookies;
    QString name = map["name"].toString();
    QString url = map["url"].toString();
    QString path = map["path"].toString();
    bool secure = map["secure"].toBool();
    bool session = map["session"].toBool();
    QVector<QNetworkCookie> cookies = mApp->cookieJar()->getAllCookies();
    for (QNetworkCookie cookie : cookies) {
        if ((!map["name"].isValid() || cookie.name() == name)
                && (!map["url"].isValid() || cookie.domain() == url)
                && (!map["path"].isValid() || cookie.path() == path)
                && (!map["secure"].isValid() || cookie.isSecure() == secure)
                && (!map["session"].isValid() || cookie.isSessionCookie() == session)) {
            QNetworkCookie *netCookie = new QNetworkCookie(cookie);
            QmlCookie *qmlCookie = new QmlCookie(netCookie);
            qmlCookies.append(qmlCookie);
        }
    }
    return qmlCookies;
}

void QmlCookies::set(const QVariantMap &map)
{
    QString name = map["name"].toString();
    QString url = map["url"].toString();
    QString path = map["path"].toString();
    bool secure = map["secure"].toBool();
    QDateTime expirationDate = QDateTime::fromMSecsSinceEpoch(map["expirationDate"].toDouble());
    bool httpOnly = map["httpOnly"].toBool();
    QString value = map["value"].toString();
    QNetworkCookie cookie;
    cookie.setName(name.toLocal8Bit());
    cookie.setDomain(url);
    cookie.setPath(path);
    cookie.setSecure(secure);
    cookie.setExpirationDate(expirationDate);
    cookie.setHttpOnly(httpOnly);
    cookie.setValue(value.toLocal8Bit());
    mApp->webProfile()->cookieStore()->setCookie(cookie);
}

void QmlCookies::remove(const QVariantMap &map)
{
    QNetworkCookie *netCookie = getNetworkCookie(map);
    if (!netCookie) {
        qWarning() << "Error:" << "Cannot find cookie";
        return;
    }
    mApp->webProfile()->cookieStore()->deleteCookie(*netCookie);
}
