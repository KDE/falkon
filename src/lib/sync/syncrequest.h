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

#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQueue>
#include <QObject>

class SyncRequestManager : public QObject
{
    Q_OBJECT

public:
    explicit SyncRequestManager(QObject *parent = nullptr);
    void addRequest(QNetworkRequest *request, bool post);
    void startSync(bool start);

private:
    struct RequestPair {
        QNetworkRequest request;
        bool post;
    };

    QNetworkAccessManager *m_requestManager;
    QQueue<RequestPair> *m_requestQueue;
    bool startRequests;
};
