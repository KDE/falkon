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

/**
 * @brief Checks if the context menu is requested on image.
 * @return true if image, else false
 */
bool QmlWebHitTestResult::isImage() const
{
    return !m_webHitTestResult.imageUrl().isEmpty();
}

/**
 * @brief Checks if the context menu is requested on editable content.
 * @return true if the content is editable, else false
 */
bool QmlWebHitTestResult::isContentEditable() const
{
    return m_webHitTestResult.isContentEditable();
}

/**
 * @brief Checks if the context menu is requested on the selected content.
 * @return true if content is selected, else false.
 */
bool QmlWebHitTestResult::isContentSelected() const
{
    return m_webHitTestResult.isContentSelected();
}

/**
 * @brief Checks if the context menu is requested on null element.
 * @return true if the element is null, else false
 */
bool QmlWebHitTestResult::isNull() const
{
    return m_webHitTestResult.isNull();
}

/**
 * @brief Checks if the context menu is requested on a link.
 * @return true if the element is link, else false
 */
bool QmlWebHitTestResult::isLink() const
{
    return !m_webHitTestResult.linkUrl().isEmpty();
}

/**
 * @brief Checks if the context menu is requested on a media element.
 * @return true if the element is media, else false
 */
bool QmlWebHitTestResult::isMedia() const
{
    return !m_webHitTestResult.mediaUrl().isEmpty();
}

/**
 * @brief Checks if the context menu requested on media element is paused.
 * @return true if media is paused, else false
 */
bool QmlWebHitTestResult::mediaPaused() const
{
    return m_webHitTestResult.mediaPaused();
}

/**
 * @brief Checks if the context menu requested on media element is muted.
 * @return true if media is muted, else false
 */
bool QmlWebHitTestResult::mediaMuted() const
{
    return m_webHitTestResult.mediaMuted();
}

/**
 * @brief Gets the tagName of the element on which the context menu is requested.
 * @return String representing the tag name of the element
 */
QString QmlWebHitTestResult::tagName() const
{
    return m_webHitTestResult.tagName();
}

/**
 * @brief Gets the base url on which the context menu is requested.
 * @return String representing the base url
 */
QString QmlWebHitTestResult::baseUrl() const
{
    const QUrl base = m_webHitTestResult.baseUrl();
    return QString::fromUtf8(base.toEncoded());
}

/**
 * @brief Gets the link title on which the context menu is requested.
 * @return String representing the link title
 */
QString QmlWebHitTestResult::linkTitle() const
{
    return m_webHitTestResult.linkTitle();
}

/**
 * @brief Gets the link url on which the context menu is requested.
 * @return String representing the link url
 */
QString QmlWebHitTestResult::linkUrl() const
{
    const QUrl link = m_webHitTestResult.linkUrl();
    return QString::fromUtf8(link.toEncoded());
}

/**
 * @brief Gets the url of image on which the context menu is requested.
 * @return String representing the image url
 */
QString QmlWebHitTestResult::imageUrl() const
{
    const QUrl image = m_webHitTestResult.imageUrl();
    return QString::fromUtf8(image.toEncoded());
}

/**
 * @brief Gets the url of media on which the context menu is requested.
 * @return String representing the media url
 */
QString QmlWebHitTestResult::mediaUrl() const
{
    const QUrl media = m_webHitTestResult.mediaUrl();
    return QString::fromUtf8(media.toEncoded());
}

/**
 * @brief Gets the position at which the context menu is requested.
 * @return QPoint representing the position
 */
QPoint QmlWebHitTestResult::pos() const
{
    return m_webHitTestResult.pos();
}

/**
 * @brief Gets the viewport position at which the context menu is requested.
 * @return QPoint representing the viewport position
 */
QPointF QmlWebHitTestResult::viewportPos() const
{
    return m_webHitTestResult.viewportPos();
}
