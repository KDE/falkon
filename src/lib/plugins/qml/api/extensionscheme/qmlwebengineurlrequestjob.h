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

#include <QWebEngineUrlRequestJob>

/**
 * @brief The QmlWebEngineUrlRequestJob class
 */
class QmlWebEngineUrlRequestJob : public QObject
{
    Q_OBJECT
    /**
     * @brief initiator of the QWebEngineUrlRequestJob
     */
    Q_PROPERTY(QString initiator READ initiator CONSTANT)
    /**
     * @brief request url of the QWebEngineUrlRequestJob
     */
    Q_PROPERTY(QString requestUrl READ requestUrl CONSTANT)
    /**
     * @brief request method of the QWebEngineUrlRequestJob
     */
    Q_PROPERTY(QString requestMethod READ requestMethod CONSTANT)
public:
    /**
     * @brief The Error enum, exposes QWebEngineUrlRequestJob::Error to QML
     */
    enum Error {
        NoError = QWebEngineUrlRequestJob::NoError,               //!< No error
        UrlNotFound = QWebEngineUrlRequestJob::UrlNotFound,       //!< Url not found error
        UrlInvaild = QWebEngineUrlRequestJob::UrlInvalid,         //!< Url invalid error
        RequestAborted = QWebEngineUrlRequestJob::RequestAborted, //!< Request aborted
        RequestDenied = QWebEngineUrlRequestJob::RequestDenied,   //!< Request denied
        RequestFailed = QWebEngineUrlRequestJob::RequestFailed    //!< Request failed
    };
    Q_ENUM(Error)
    explicit QmlWebEngineUrlRequestJob(QWebEngineUrlRequestJob *job = nullptr, QObject *parent = nullptr);
    /**
     * @brief Fails the request with the error
     * @param error
     */
    Q_INVOKABLE void fail(QmlWebEngineUrlRequestJob::Error error);
    /**
     * @brief Redirects the request to the url
     * @param urlString, represents the url to which the request is to be redirected
     */
    Q_INVOKABLE void redirect(const QString &urlString);
    /**
     * @brief Replies to the request
     * @param A JavaScript object containing
     *        - content: String representing the reply data
     *        - contentType: String representing the contentType of reply data
     */
    Q_INVOKABLE void reply(const QVariantMap &map);
private:
    QWebEngineUrlRequestJob *m_job = nullptr;
    QString initiator() const;
    QString requestUrl() const;
    QString requestMethod() const;
};
