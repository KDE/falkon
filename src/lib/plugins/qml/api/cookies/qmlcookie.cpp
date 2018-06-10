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
#include "qmlcookie.h"
#include <QDebug>
#include <QQmlEngine>

QmlCookie::QmlCookie(QNetworkCookie *cookie, QObject *parent)
    : QObject(parent)
    , m_cookie(cookie)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

/**
 * @brief Get the domain of the cookie
 * @return String representing the domain of the cookie
 */
QString QmlCookie::domain() const
{
    if (!m_cookie) {
        return QString();
    }
    return m_cookie->domain();
}

/**
 * @brief Get expiration date of the cookie
 * @return expiration date of the cookie
 */
QDateTime QmlCookie::expirationDate() const
{
    if (!m_cookie) {
        return QDateTime();
    }
    return m_cookie->expirationDate();
}

/**
 * @brief Get the name of the cookie
 * @return String representing the name of the cookie
 */
QString QmlCookie::name() const
{
    if (!m_cookie) {
        return QString();
    }
    return QString(m_cookie->name());
}

/**
 * @brief Get the path of the cookie
 * @return String representing the path of the cookie
 */
QString QmlCookie::path() const
{
    if (!m_cookie) {
        return QString();
    }
    return m_cookie->path();
}

/**
 * @brief Checks if the cookie is secure
 * @return true if the cookie is secure, else false
 */
bool QmlCookie::secure() const
{
    if (!m_cookie) {
        return false;
    }
    return m_cookie->isSecure();
}

/**
 * @brief Checks if the cookie is a session cookie
 * @return true if the cookie is a session cookie, else false
 */
bool QmlCookie::session() const
{
    if (!m_cookie) {
        return false;
    }
    return m_cookie->isSessionCookie();
}

/**
 * @brief Get the value of the cookie
 * @return String representing the value of the cookie
 */
QString QmlCookie::value() const
{
    if (!m_cookie) {
        return QString();
    }
    return QString(m_cookie->value());
}

QmlCookieData::QmlCookieData()
{
}

QmlCookieData::~QmlCookieData()
{
    qDeleteAll(m_cookies);
}

QmlCookie *QmlCookieData::get(QNetworkCookie *cookie)
{
    QmlCookie *qmlCookie = m_cookies.value(cookie);
    if (!qmlCookie) {
        qmlCookie = new QmlCookie(cookie);
        m_cookies.insert(cookie, qmlCookie);
    }
    return qmlCookie;
}
