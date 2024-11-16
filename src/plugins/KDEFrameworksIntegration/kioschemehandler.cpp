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

#include <KIO/StoredTransferJob>


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

    KIO::StoredTransferJob *kioJob = KIO::storedGet(job->requestUrl(), KIO::NoReload,
                                                    KIO::HideProgressInfo);
    kioJob->setRedirectionHandlingEnabled(false);
    connect(kioJob, &KIO::StoredTransferJob::result, this, [=]() {
        kioJob->deleteLater();
        if (!jobPtr) {
            return;
        }

        if (kioJob->error() == KJob::NoError) {
            if (kioJob->redirectUrl().isValid()) {
                job->redirect(kioJob->redirectUrl());
            } else {
                QBuffer *buffer = new QBuffer;
                buffer->open(QBuffer::ReadWrite);
                buffer->write(kioJob->data());
                buffer->seek(0);
                connect(buffer, &QIODevice::aboutToClose, buffer, &QObject::deleteLater);
                job->reply(kioJob->mimetype().toUtf8(), buffer);
            }
        }
        else {
            job->fail(QWebEngineUrlRequestJob::RequestFailed);
        }
    });
}
