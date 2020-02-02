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
#include "qmlcookiesapitest.h"
#include "autotests.h"
#include "qmltesthelper.h"
#include "mainapplication.h"
#include "cookiejar.h"
#include "qml/api/cookies/qmlcookie.h"
#include <QWebEngineProfile>

void QmlCookiesApiTest::initTestCase()
{
}

void QmlCookiesApiTest::cleanupTestCase()
{
}

void QmlCookiesApiTest::testCookieAdditionRemoval()
{
    QSignalSpy cookieAddSpy(mApp->cookieJar(), &CookieJar::cookieAdded);
    m_testHelper.evaluate("Falkon.Cookies.set({"
                     "    name: 'Example',"
                     "    url: '.example.com',"
                     "    expirationDate: Date.now() + 60*1000"
                     "})");
    QTRY_COMPARE(cookieAddSpy.count(), 1);
    QNetworkCookie netCookie = qvariant_cast<QNetworkCookie>(cookieAddSpy.at(0).at(0));
    QCOMPARE(netCookie.name(), QByteArrayLiteral("Example"));
    QObject *object = m_testHelper.evaluateQObject("Falkon.Cookies");
    QVERIFY(object);
    QSignalSpy qmlCookieSpy(object, SIGNAL(changed(QVariantMap)));
    QNetworkCookie anotherNetCookie;
    anotherNetCookie.setName(QString("Hello").toLocal8Bit());
    anotherNetCookie.setDomain(".mydomain.com");
    anotherNetCookie.setExpirationDate(QDateTime::currentDateTime().addSecs(60));
    mApp->webProfile()->cookieStore()->setCookie(anotherNetCookie);
    QTRY_COMPARE(qmlCookieSpy.count(), 1);
    QVariantMap addedQmlCookieMap = QVariant(qmlCookieSpy.at(0).at(0)).toMap();
    QObject *addedQmlCookie = qvariant_cast<QObject*>(addedQmlCookieMap.value("cookie"));
    bool removed = addedQmlCookieMap.value("removed").toBool();
    QCOMPARE(addedQmlCookie->property("name").toString(), QSL("Hello"));
    QCOMPARE(removed, false);

    mApp->webProfile()->cookieStore()->deleteCookie(netCookie);
    QTRY_COMPARE(qmlCookieSpy.count(), 2);
    QVariantMap removedQmlCookieMap = QVariant(qmlCookieSpy.at(1).at(0)).toMap();
    QObject *removedQmlCookie = qvariant_cast<QObject*>(removedQmlCookieMap.value("cookie"));
    removed = removedQmlCookieMap.value("removed").toBool();
    QCOMPARE(removedQmlCookie->property("name").toString(), QSL("Example"));
    QCOMPARE(removed, true);

    QSignalSpy cookieRemoveSpy(mApp->cookieJar(), &CookieJar::cookieRemoved);
    m_testHelper.evaluate("Falkon.Cookies.remove({"
                     "    name: 'Hello',"
                     "    url: '.mydomain.com',"
                     "})");
    QTRY_COMPARE(cookieRemoveSpy.count(), 1);
    netCookie = qvariant_cast<QNetworkCookie>(cookieRemoveSpy.at(0).at(0));
    QCOMPARE(netCookie.name(), QByteArrayLiteral("Hello"));
}

void QmlCookiesApiTest::testCookieGet()
{
    QDateTime current = QDateTime::currentDateTime();
    QSignalSpy cookieAddSpy(mApp->cookieJar(), &CookieJar::cookieAdded);

    QNetworkCookie netCookie_1;
    netCookie_1.setName(QString("Apple").toLocal8Bit());
    netCookie_1.setDomain(".apple-domain.com");
    netCookie_1.setExpirationDate(current.addSecs(60));
    mApp->webProfile()->cookieStore()->setCookie(netCookie_1);

    QNetworkCookie netCookie_2;
    netCookie_2.setName(QString("Mango").toLocal8Bit());
    netCookie_2.setDomain(".mango-domain.com");
    netCookie_2.setExpirationDate(current.addSecs(120));
    mApp->webProfile()->cookieStore()->setCookie(netCookie_2);

    QNetworkCookie netCookie_3;
    netCookie_3.setName(QString("Mango").toLocal8Bit());
    netCookie_3.setDomain(".yet-another-mango-domain.com");
    netCookie_3.setExpirationDate(current.addSecs(180));
    mApp->webProfile()->cookieStore()->setCookie(netCookie_3);

    QTRY_COMPARE(cookieAddSpy.count(), 3);

    QNetworkCookie actualMangoCookie;
    for (const QNetworkCookie &cookie : mApp->cookieJar()->getAllCookies()) {
        if (cookie.name() == QSL("Mango") && cookie.domain() == QSL(".mango-domain.com")) {
            actualMangoCookie = cookie;
        }
    }

    QObject *mangoCookie = m_testHelper.evaluateQObject("Falkon.Cookies.get({"
                     "    name: 'Mango',"
                     "    url: '.mango-domain.com'"
                     "})");
    QVERIFY(mangoCookie);
    QCOMPARE(mangoCookie->property("name").toString(), QSL("Mango"));
    QCOMPARE(mangoCookie->property("expirationDate").toDateTime(), actualMangoCookie.expirationDate());

    QList<QVariant> mangoCookies = m_testHelper.evaluate("Falkon.Cookies.getAll({name: 'Mango'})").toVariant().toList();
    QCOMPARE(mangoCookies.length(), 2);
}

FALKONTEST_MAIN(QmlCookiesApiTest)
