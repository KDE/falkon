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
#pragma once

#include "webhittestresult.h"
#include <QObject>

/**
 * @brief The class exposing result of WebHitTest to QML
 */
class QmlWebHitTestResult : public QObject
{
    Q_OBJECT
    /**
     * @brief Gets the tagName of the element on which the context menu is requested.
     */
    Q_PROPERTY(QString tagName READ tagName CONSTANT)
    /**
     * @brief Gets the base url on which the context menu is requested.
     */
    Q_PROPERTY(QString baseUrl READ baseUrl CONSTANT)
    /**
     * @brief Gets the link title on which the context menu is requested.
     */
    Q_PROPERTY(QString linkTitle READ linkTitle CONSTANT)
    /**
     * @brief Gets the link url on which the context menu is requested.
     */
    Q_PROPERTY(QString linkUrl READ linkUrl CONSTANT)
    /**
     * @brief Gets the url of image on which the context menu is requested.
     */
    Q_PROPERTY(QString imageUrl READ imageUrl CONSTANT)
    /**
     * @brief Gets the url of media on which the context menu is requested.
     */
    Q_PROPERTY(QString mediaUrl READ mediaUrl CONSTANT)
    /**
     * @brief Gets the position at which the context menu is requested.
     */
    Q_PROPERTY(QPoint pos READ pos CONSTANT)
    /**
     * @brief Gets the viewport position at which the context menu is requested.
     */
    Q_PROPERTY(QPointF viewportPos READ viewportPos CONSTANT)
public:
    explicit QmlWebHitTestResult(const WebHitTestResult &webHitTestResult, QObject *parent = nullptr);
    /**
     * @brief Checks if the context menu is requested on image.
     * @return true if image, else false
     */
    Q_INVOKABLE bool isImage() const;
    /**
     * @brief Checks if the context menu is requested on editable content.
     * @return true if the content is editable, else false
     */
    Q_INVOKABLE bool isContentEditable() const;
    /**
     * @brief Checks if the context menu is requested on the selected content.
     * @return true if content is selected, else false.
     */
    Q_INVOKABLE bool isContentSelected() const;
    /**
     * @brief Checks if the context menu is requested on null element.
     * @return true if the element is null, else false
     */
    Q_INVOKABLE bool isNull() const;
    /**
     * @brief Checks if the context menu is requested on a link.
     * @return true if the element is link, else false
     */
    Q_INVOKABLE bool isLink() const;
    /**
     * @brief Checks if the context menu is requested on a media element.
     * @return true if the element is media, else false
     */
    Q_INVOKABLE bool isMedia() const;
    /**
     * @brief Checks if the context menu requested on media element is paused.
     * @return true if media is paused, else false
     */
    Q_INVOKABLE bool mediaPaused() const;
    /**
     * @brief Checks if the context menu requested on media element is muted.
     * @return true if media is muted, else false
     */
    Q_INVOKABLE bool mediaMuted() const;
    QString tagName() const;
    QString baseUrl() const;
    QString linkTitle() const;
    QString linkUrl() const;
    QString imageUrl() const;
    QString mediaUrl() const;
    QPoint pos() const;
    QPointF viewportPos() const;

private:
    WebHitTestResult m_webHitTestResult;
};
