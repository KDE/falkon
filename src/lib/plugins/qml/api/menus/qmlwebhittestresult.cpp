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
#include "qmlwebhittestresult.h"

#define NOT !

QmlWebHitTestResult::QmlWebHitTestResult(const WebHitTestResult &webHitTestResult, QObject *parent)
    : QObject(parent)
    , m_webHitTestResult(webHitTestResult)
{
}

bool QmlWebHitTestResult::isImage() const
{
    return NOT m_webHitTestResult.imageUrl().isEmpty();
}

bool QmlWebHitTestResult::isContentEditable() const
{
    return m_webHitTestResult.isContentEditable();
}

bool QmlWebHitTestResult::isContentSelected() const
{
    return m_webHitTestResult.isContentSelected();
}

bool QmlWebHitTestResult::isNull() const
{
    return m_webHitTestResult.isNull();
}

bool QmlWebHitTestResult::isLink() const
{
    return NOT m_webHitTestResult.linkUrl().isEmpty();
}

bool QmlWebHitTestResult::isMedia() const
{
    return NOT m_webHitTestResult.mediaUrl().isEmpty();
}

bool QmlWebHitTestResult::mediaPaused() const
{
    return m_webHitTestResult.mediaPaused();
}

bool QmlWebHitTestResult::mediaMuted() const
{
    return m_webHitTestResult.mediaMuted();
}

QString QmlWebHitTestResult::tagName() const
{
    return m_webHitTestResult.tagName();
}
