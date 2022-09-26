/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
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
#include "cookiejar.h"
#include "mainapplication.h"
#include "datapaths.h"
#include "autosaver.h"
#include "settings.h"
#include "qztools.h"
#include "sitesettingsmanager.h"

#include <QNetworkCookie>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QDateTime>

//#define COOKIE_DEBUG

CookieJar::CookieJar(QObject* parent)
    : QObject(parent)
    , m_client(mApp->webProfile()->cookieStore())
{
    loadSettings();
    m_client->loadAllCookies();

    m_client->setCookieFilter(std::bind(&CookieJar::cookieFilter, this, std::placeholders::_1));

    connect(m_client, &QWebEngineCookieStore::cookieAdded, this, &CookieJar::slotCookieAdded);
    connect(m_client, &QWebEngineCookieStore::cookieRemoved, this, &CookieJar::slotCookieRemoved);
}

CookieJar::~CookieJar()
{
    m_client->setCookieFilter(nullptr);
}

void CookieJar::loadSettings()
{
    Settings settings;
    settings.beginGroup(QSL("Cookie-Settings"));
    m_allowCookies = settings.value(QSL("allowCookies"), true).toBool();
    m_filterThirdParty = settings.value(QSL("filterThirdPartyCookies"), false).toBool();
    m_filterTrackingCookie = settings.value(QSL("filterTrackingCookie"), false).toBool();
    m_whitelist = settings.value(QSL("whitelist"), QStringList()).toStringList();
    m_blacklist = settings.value(QSL("blacklist"), QStringList()).toStringList();
    settings.endGroup();
}

void CookieJar::setAllowCookies(bool allow)
{
    m_allowCookies = allow;
}

void CookieJar::deleteCookie(const QNetworkCookie &cookie)
{
    m_client->deleteCookie(cookie);
}

QVector<QNetworkCookie> CookieJar::getAllCookies() const
{
    return m_cookies;
}

void CookieJar::deleteAllCookies(bool deleteAll)
{
    if (deleteAll || m_whitelist.isEmpty()) {
        m_client->deleteAllCookies();
        return;
    }

    for (const QNetworkCookie &cookie : std::as_const(m_cookies)) {
        if (!listMatchesDomain(m_whitelist, cookie.domain())) {
            m_client->deleteCookie(cookie);
        }
    }
}

bool CookieJar::matchDomain(QString cookieDomain, QString siteDomain) const
{
    // According to RFC 6265

    // Remove leading dot
    if (cookieDomain.startsWith(QLatin1Char('.'))) {
        cookieDomain.remove(0, 1);
    }

    if (siteDomain.startsWith(QLatin1Char('.'))) {
        siteDomain.remove(0, 1);
    }

    return QzTools::matchDomain(cookieDomain, siteDomain);
}

bool CookieJar::listMatchesDomain(const QStringList &list, const QString &cookieDomain) const
{
    for (const QString &d : list) {
        if (matchDomain(d, cookieDomain)) {
            return true;
        }
    }

    return false;
}

void CookieJar::slotCookieAdded(const QNetworkCookie &cookie)
{
    if (rejectCookie(QString(), cookie, cookie.domain())) {
        m_client->deleteCookie(cookie);
        return;
    }

    m_cookies.append(cookie);
    Q_EMIT cookieAdded(cookie);
}

void CookieJar::slotCookieRemoved(const QNetworkCookie &cookie)
{
    if (m_cookies.removeOne(cookie))
        Q_EMIT cookieRemoved(cookie);
}

bool CookieJar::cookieFilter(const QWebEngineCookieStore::FilterRequest &request) const
{
    auto result = mApp->siteSettingsManager()->getPermission(SiteSettingsManager::poAllowCookies, request.origin);

    if (!m_allowCookies && (result != SiteSettingsManager::Allow)) {
#ifdef COOKIE_DEBUG
        qDebug() << "Cookies not allowed" << request.origin;
#endif
        return false;
    }

    if (m_allowCookies && (result == SiteSettingsManager::Deny)) {
#ifdef COOKIE_DEBUG
        qDebug() << "Cookies denied" << request.origin;
#endif
        return false;
    }

    if (m_filterThirdParty && request.thirdParty) {
#ifdef COOKIE_DEBUG
        qDebug() << "thirdParty" << request.firstPartyUrl << request.origin;
#endif
        return false;
    }

    return true;
}

bool CookieJar::rejectCookie(const QString &domain, const QNetworkCookie &cookie, const QString &cookieDomain) const
{
    Q_UNUSED(domain)

    auto result = mApp->siteSettingsManager()->getPermission(SiteSettingsManager::poAllowCookies, cookieDomain);

    if (!m_allowCookies && (result != SiteSettingsManager::Allow)) {
#ifdef COOKIE_DEBUG
        qDebug() << "Cookies not allowed" << cookie;
#endif
        return false;
    }

    if (m_allowCookies && (result == SiteSettingsManager::Deny)) {
#ifdef COOKIE_DEBUG
        qDebug() << "Cookies denied" << cookie;
#endif
        return false;
    }

#ifdef QTWEBENGINE_DISABLED
    if (m_filterThirdParty) {
        bool result = matchDomain(cookieDomain, domain);
        if (!result) {
#ifdef COOKIE_DEBUG
            qDebug() << "purged for domain mismatch" << cookie << cookieDomain << domain;
#endif
            return true;
        }
    }
#endif

    if (m_filterTrackingCookie && cookie.name().startsWith("__utm")) {
#ifdef COOKIE_DEBUG
        qDebug() << "purged as tracking " << cookie;
#endif
        return true;
    }

    return false;
}
