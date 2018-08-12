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
#pragma once

#include <QObject>
#include <QDateTime>
#include <QNetworkCookie>

/**
 * @brief The class exposing QNetworkCookie to QML
 */
class QmlCookie : public QObject
{
    Q_OBJECT

    /**
     * @brief domain of the cookie
     */
    Q_PROPERTY(QString domain READ domain CONSTANT)

    /**
     * @brief expiration date of the cookie
     */
    Q_PROPERTY(QDateTime expirationDate READ expirationDate CONSTANT)

    /**
     * @brief name of the cookie
     */
    Q_PROPERTY(QString name READ name CONSTANT)

    /**
     * @brief path of the cookie
     */
    Q_PROPERTY(QString path READ path CONSTANT)

    /**
     * @brief checks if cookie is secure
     */
    Q_PROPERTY(bool secure READ secure CONSTANT)

    /**
     * @brief checks if cookie is a session cookie
     */
    Q_PROPERTY(bool session READ session CONSTANT)

    /**
     * @brief value of the cookie
     */
    Q_PROPERTY(QString value READ value CONSTANT)
public:
    explicit QmlCookie(QNetworkCookie *cookie, QObject *parent = nullptr);

private:
    QNetworkCookie *m_cookie = nullptr;

    QString domain() const;
    QDateTime expirationDate() const;
    QString name() const;
    QString path() const;
    bool secure() const;
    bool session() const;
    QString value() const;
};

Q_DECLARE_METATYPE(QmlCookie*)
