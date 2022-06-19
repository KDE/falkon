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
#include <QQmlEngine>

QmlCookie::QmlCookie(QNetworkCookie *cookie, QObject *parent)
    : QObject(parent)
    , m_cookie(cookie)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

QString QmlCookie::domain() const
{
    if (!m_cookie) {
        return {};
    }
    return m_cookie->domain();
}

QDateTime QmlCookie::expirationDate() const
{
    if (!m_cookie) {
        return {};
    }
    return m_cookie->expirationDate();
}

QString QmlCookie::name() const
{
    if (!m_cookie) {
        return {};
    }
    return QString(m_cookie->name());
}

QString QmlCookie::path() const
{
    if (!m_cookie) {
        return {};
    }
    return m_cookie->path();
}

bool QmlCookie::secure() const
{
    if (!m_cookie) {
        return false;
    }
    return m_cookie->isSecure();
}

bool QmlCookie::session() const
{
    if (!m_cookie) {
        return false;
    }
    return m_cookie->isSessionCookie();
}

QString QmlCookie::value() const
{
    if (!m_cookie) {
        return {};
    }
    return QString(m_cookie->value());
}
