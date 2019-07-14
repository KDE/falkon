/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 Prasenjit Kumar Shaw <shawprasenjit07@gmail.com>
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

#include "syncrequest.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQueue>
#include <QObject>

SyncRequestManager::SyncRequestManager(QObject* parent)
    : QObject(parent)
{
    m_requestManager = new QNetworkAccessManager(this);
    m_requestQueue = new QQueue<RequestPair>();
    startRequests = false;
}

void SyncRequestManager::addRequest(QNetworkRequest *request, bool post)
{
    RequestPair temp = {*request, post};
    m_requestQueue->enqueue(temp);
}

void SyncRequestManager::startSync(bool start)
{
    startRequests = start;
    if (startRequests) {
        if (!m_requestQueue->isEmpty()) {
            RequestPair req = m_requestQueue->dequeue();
            QNetworkRequest request = req.request;
            QNetworkReply *reply = nullptr;
            if (req.post) {
                reply = m_requestManager->get(request);
            } else {
                reply = m_requestManager->get(request);
            }
        }
    }
}
