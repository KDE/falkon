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
#include <QQmlEngine>

QmlWebHitTestResult::QmlWebHitTestResult(const WebHitTestResult &webHitTestResult, QObject *parent)
    : QObject(parent)
    , m_webHitTestResult(webHitTestResult)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);
}

bool QmlWebHitTestResult::isImage() const
{
    return !m_webHitTestResult.imageUrl().isEmpty();
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
    return !m_webHitTestResult.linkUrl().isEmpty();
}

bool QmlWebHitTestResult::isMedia() const
{
    return !m_webHitTestResult.mediaUrl().isEmpty();
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

QString QmlWebHitTestResult::baseUrl() const
{
    const QUrl base = m_webHitTestResult.baseUrl();
    return QString::fromUtf8(base.toEncoded());
}

QString QmlWebHitTestResult::linkTitle() const
{
    return m_webHitTestResult.linkTitle();
}

QString QmlWebHitTestResult::linkUrl() const
{
    const QUrl link = m_webHitTestResult.linkUrl();
    return QString::fromUtf8(link.toEncoded());
}

QString QmlWebHitTestResult::imageUrl() const
{
    const QUrl image = m_webHitTestResult.imageUrl();
    return QString::fromUtf8(image.toEncoded());
}

QString QmlWebHitTestResult::mediaUrl() const
{
    const QUrl media = m_webHitTestResult.mediaUrl();
    return QString::fromUtf8(media.toEncoded());
}

QPoint QmlWebHitTestResult::pos() const
{
    return m_webHitTestResult.pos();
}

QPointF QmlWebHitTestResult::viewportPos() const
{
    return m_webHitTestResult.viewportPos();
}
