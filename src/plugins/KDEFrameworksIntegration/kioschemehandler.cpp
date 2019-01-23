/* ============================================================
* KDEFrameworksIntegration - KDE support plugin for Falkon
* Copyright (C) 2018 David Rosca <nowrep@gmail.com>
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
#include "kioschemehandler.h"

#include <QBuffer>
#include <QPointer>
#include <QNetworkReply>
#include <QWebEngineUrlRequestJob>

#include <KIO/AccessManager>

Q_GLOBAL_STATIC_WITH_ARGS(KIO::Integration::AccessManager, s_knam, (nullptr))

KIOSchemeHandler::KIOSchemeHandler(const QString &protocol, QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
    , m_protocol(protocol)
{
}

QString KIOSchemeHandler::protocol() const
{
    return m_protocol;
}

void KIOSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    if (job->requestMethod() != QByteArray("GET")) {
        qWarning() << "Unsupported method" << job->requestMethod();
        job->fail(QWebEngineUrlRequestJob::RequestFailed);
        return;
    }

    QPointer<QWebEngineUrlRequestJob> jobPtr = job;
    QNetworkReply *reply = s_knam()->get(QNetworkRequest(job->requestUrl()));
    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (!jobPtr) {
            reply->deleteLater();
            return;
        }
        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            qWarning() << "Error:" << reply->errorString();
            job->fail(QWebEngineUrlRequestJob::RequestFailed);
        } else {
            reply->setParent(job);
            job->reply(reply->header(QNetworkRequest::ContentTypeHeader).toByteArray(), reply);
        }
    });
}
