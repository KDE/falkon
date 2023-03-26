/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2015-2018 David Rosca <nowrep@gmail.com>
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

#include "adblockurlinterceptor.h"
#include "adblockrule.h"
#include "qztools.h"

#include <QUrlQuery>

AdBlockUrlInterceptor::AdBlockUrlInterceptor(AdBlockManager *manager)
    : UrlInterceptor(manager)
    , m_manager(manager)
{
}

void AdBlockUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &request)
{
    QString ruleFilter;
    QString ruleSubscription;
    if (!m_manager->block(request, ruleFilter, ruleSubscription)) {
        return;
    }

    if (request.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeMainFrame) {
        QString url = QSL("qrc:adblock/data/adblock.html?direction=%DIRECTION%&title=%1&rule=%3").arg(
            tr("Blocked content"),
            tr("Blocked by <i>%1 (%2)</i>").arg(ruleFilter, ruleSubscription)
        );
        url = QzTools::applyDirectionToPage(url);

        request.redirect(QUrl(url));
    } else {
        request.block(true);
    }

    AdBlockedRequest r;
    r.requestUrl = request.requestUrl();
    r.firstPartyUrl = request.firstPartyUrl();
    r.requestMethod = request.requestMethod();
    r.resourceType = request.resourceType();
    r.navigationType = request.navigationType();
    r.rule = ruleFilter;
    Q_EMIT requestBlocked(r);
}
