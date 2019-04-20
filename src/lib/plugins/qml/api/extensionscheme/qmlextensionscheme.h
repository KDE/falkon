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

#include "schemehandlers/extensionschemehandler.h"
#include "qmlwebengineurlrequestjob.h"
#include <QQmlParserStatus>
#include <QJSValue>

class QmlExtensionSchemeHandler;

/**
 * @brief The QmlExtensionScheme class, exposed to QML as ExtensionScheme
 */
class QmlExtensionScheme : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    /**
     * @brief extension scheme handle name
     */
    Q_PROPERTY(QString name READ name WRITE setName)
public:
    explicit QmlExtensionScheme(QObject *parent = nullptr);
    ~QmlExtensionScheme() override;
    void classBegin() override {}
    void componentComplete() override;
Q_SIGNALS:
    /**
     * @brief The signal emitted when the request to the scheme handle is started
     * @param request of the type [QmlWebEngineUrlRequestJob](@ref QmlWebEngineUrlRequestJob)
     */
    void requestStarted(const QJSValue &request);
private:
    QString m_name;
    QmlExtensionSchemeHandler *m_schemeHandler = nullptr;

    QString name() const;
    void setName(const QString &name);
};

class QmlExtensionSchemeHandler : public ExtensionSchemeHandler
{
    Q_OBJECT
public:
    void requestStarted(QWebEngineUrlRequestJob *job) override;
Q_SIGNALS:
    void _requestStarted(QWebEngineUrlRequestJob *job);
};
