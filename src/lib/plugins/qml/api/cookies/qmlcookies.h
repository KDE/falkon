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
#include "qmlcookie.h"

/**
 * @brief The class exposing Cookies API to QML
 */
class QmlCookies : public QObject
{
    Q_OBJECT
public:
    explicit QmlCookies(QObject *parent = nullptr);
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
    Q_INVOKABLE QmlCookie *get(const QVariantMap &map);
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
    Q_INVOKABLE QList<QObject*> getAll(const QVariantMap &map);
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
    Q_INVOKABLE void set(const QVariantMap &map);
    /**
     * @brief Remove a cookie
     * @param A JavaScript object containing
     *        - name:
     *          String representing the name of the cookie
     *        - url:
     *          String representing the url of the cookie
     */
    Q_INVOKABLE void remove(const QVariantMap &map);
Q_SIGNALS:
    /**
     * @brief The signal emitted when a cookie is added or removed
     * @param A JavaScript object containing
     *        - cookie:
     *          Object of type [QmlCookie](@ref QmlCookie), which is added or removed
     *        - removed:
     *          Bool representing if the cookie is removed
     */
    void changed(const QVariantMap &map);
private:
    QNetworkCookie getNetworkCookie(const QVariantMap &map);
};
