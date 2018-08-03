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
    Q_PROPERTY(QString tagName READ tagName CONSTANT)
    Q_PROPERTY(QString baseUrl READ baseUrl CONSTANT)
    Q_PROPERTY(QString linkTitle READ linkTitle CONSTANT)
    Q_PROPERTY(QString linkUrl READ linkUrl CONSTANT)
    Q_PROPERTY(QString imageUrl READ imageUrl CONSTANT)
    Q_PROPERTY(QString mediaUrl READ mediaUrl CONSTANT)
    Q_PROPERTY(QPoint pos READ pos CONSTANT)
    Q_PROPERTY(QPointF viewportPos READ viewportPos CONSTANT)
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
