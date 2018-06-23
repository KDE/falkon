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
public:
    explicit QmlWebHitTestResult(const WebHitTestResult &webHitTestResult, QObject *parent = nullptr);
    Q_INVOKABLE bool isImage() const;
    Q_INVOKABLE bool isContentEditable() const;
    Q_INVOKABLE bool isContentSelected() const;
    Q_INVOKABLE bool isNull() const;
    Q_INVOKABLE bool isLink() const;
    Q_INVOKABLE bool isMedia() const;
    Q_INVOKABLE bool mediaPaused() const;
    Q_INVOKABLE bool mediaMuted() const;
    Q_INVOKABLE QString tagName() const;
    Q_INVOKABLE QString baseUrl() const;
    Q_INVOKABLE QString linkTitle() const;
    Q_INVOKABLE QString linkUrl() const;
    Q_INVOKABLE QString imageUrl() const;
    Q_INVOKABLE QString mediaUrl() const;
    Q_INVOKABLE QPoint pos() const;
    Q_INVOKABLE QPointF viewportPos() const;

private:
    WebHitTestResult m_webHitTestResult;
};
