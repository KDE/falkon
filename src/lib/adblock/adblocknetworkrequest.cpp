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

#include "adblocknetworkrequest.h"

#include <QWebEngineNewWindowRequest>

AdBlockNeworkRequest::AdBlockNeworkRequest(const QWebEngineUrlRequestInfo& request)
{
    m_firstPartyUrl = request.firstPartyUrl();
    m_requestUrl = request.requestUrl();
    m_resourceType = request.resourceType();
    m_requestType = NetworkRequest;
}

AdBlockNeworkRequest::AdBlockNeworkRequest(const QUrl& url, const QWebEngineNewWindowRequest& request)
{
    m_firstPartyUrl = url;
    m_requestUrl = request.requestedUrl();
    m_requestType = NewWindowRequest;
}

AdBlockNeworkRequest::~AdBlockNeworkRequest() = default;

QUrl AdBlockNeworkRequest::requestUrl() const
{
    return m_requestUrl;
}

QUrl AdBlockNeworkRequest::firstPartyUrl() const
{
    return m_firstPartyUrl;
}

QWebEngineUrlRequestInfo::ResourceType AdBlockNeworkRequest::resourceType() const
{
    return m_resourceType;
}

AdBlockNeworkRequest::RequestType AdBlockNeworkRequest::requestType() const
{
    return m_requestType;
}
