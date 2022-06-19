/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 David Rosca <nowrep@gmail.com>
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
#include "protocolhandlermanager.h"
#include "settings.h"

#include <QWebEnginePage>
#include <QtWebEngineWidgetsVersion>

#include <QWebEngineRegisterProtocolHandlerRequest>

ProtocolHandlerManager::ProtocolHandlerManager(QObject *parent)
    : QObject(parent)
{
    init();
}

QHash<QString, QUrl> ProtocolHandlerManager::protocolHandlers() const
{
    return m_protocolHandlers;
}

void ProtocolHandlerManager::addProtocolHandler(const QString &scheme, const QUrl &url)
{
    if (scheme.isEmpty() || url.isEmpty()) {
        return;
    }
    m_protocolHandlers[scheme] = url;
    registerHandler(scheme, url);
    save();
}

void ProtocolHandlerManager::removeProtocolHandler(const QString &scheme)
{
    m_protocolHandlers.remove(scheme);
    save();
}

void ProtocolHandlerManager::init()
{
    Settings settings;
    settings.beginGroup(QSL("ProtocolHandlers"));
    const QStringList keys = settings.childKeys();
    for (const QString &scheme : keys) {
        const QUrl url = settings.value(scheme).toUrl();
        m_protocolHandlers[scheme] = url;
        registerHandler(scheme, url);
    }
    settings.endGroup();
}

void ProtocolHandlerManager::save()
{
    Settings settings;
    settings.remove(QSL("ProtocolHandlers"));
    settings.beginGroup(QSL("ProtocolHandlers"));
    for (auto it = m_protocolHandlers.cbegin(); it != m_protocolHandlers.cend(); ++it) {
        settings.setValue(it.key(), it.value());
    }
    settings.endGroup();
}

void ProtocolHandlerManager::registerHandler(const QString &scheme, const QUrl &url)
{
    QString urlString = url.toString();
    urlString.replace(QL1S("%25s"), QL1S("%s"));

    auto *page = new QWebEnginePage(this);
    connect(page, &QWebEnginePage::loadFinished, page, &QObject::deleteLater);
    connect(page, &QWebEnginePage::registerProtocolHandlerRequested, this, [](QWebEngineRegisterProtocolHandlerRequest request) {
        request.accept();
    });
    page->setHtml(QSL("<script>navigator.registerProtocolHandler('%1', '%2', '')</script>").arg(scheme, urlString), url);
}
