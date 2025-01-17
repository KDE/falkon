/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2025 Juraj Oravec <jurajoravec@mailo.com>
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
#ifndef ADBLOCKNETWORKREQUEST_H
#define ADBLOCKNETWORKREQUEST_H

#include <QUrl>
#include <QWebEngineUrlRequestInfo>

#include "qzcommon.h"

class QWebEngineNewWindowRequest;

class FALKON_EXPORT AdBlockNeworkRequest
{
public:
    explicit AdBlockNeworkRequest(const QWebEngineUrlRequestInfo &request);
    explicit AdBlockNeworkRequest(const QUrl &url, const QWebEngineNewWindowRequest &request);
    ~AdBlockNeworkRequest();

    enum RequestType {
        NetworkRequest = 0,
        NewWindowRequest = 1,
    };

    QUrl requestUrl() const;
    QUrl firstPartyUrl() const;
    QWebEngineUrlRequestInfo::ResourceType resourceType() const;
    AdBlockNeworkRequest::RequestType requestType() const;

private:
    RequestType m_requestType;
    QWebEngineUrlRequestInfo::ResourceType m_resourceType;
    QUrl m_requestUrl;
    QUrl m_firstPartyUrl;
};

#endif /* ADBLOCKNETWORKREQUEST_H */
