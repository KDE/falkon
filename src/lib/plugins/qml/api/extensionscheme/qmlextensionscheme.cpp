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
#include "qmlextensionscheme.h"
#include "mainapplication.h"
#include "networkmanager.h"
#include <QWebEngineUrlRequestJob>
#include <QQmlEngine>
#include <QMap>

QmlExtensionScheme::QmlExtensionScheme(QObject *parent)
    : QObject(parent)
{
    m_schemeHandler = new QmlExtensionSchemeHandler;
    connect(m_schemeHandler, &QmlExtensionSchemeHandler::_requestStarted, this, [this](QWebEngineUrlRequestJob *job) {
        auto *qmlRequest = new QmlWebEngineUrlRequestJob(job);
        const QJSValue request = qmlEngine(this)->newQObject(qmlRequest);
        Q_EMIT requestStarted(request);
    });
}

QmlExtensionScheme::~QmlExtensionScheme()
{
    mApp->networkManager()->unregisterExtensionSchemeHandler(m_schemeHandler);
    m_schemeHandler->deleteLater();
}

void QmlExtensionScheme::componentComplete()
{
    mApp->networkManager()->registerExtensionSchemeHandler(m_name, m_schemeHandler);
}

QString QmlExtensionScheme::name() const
{
    return m_name;
}

void QmlExtensionScheme::setName(const QString &name)
{
    m_name = name;
}

void QmlExtensionSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    Q_EMIT _requestStarted(job);
}
