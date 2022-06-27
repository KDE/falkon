/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QWebEngineCertificateError>

#include "qzcommon.h"

class UrlInterceptor;
class NetworkUrlInterceptor;
class ExtensionSchemeManager;
class ExtensionSchemeHandler;

class FALKON_EXPORT NetworkManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);

    bool certificateError(const QWebEngineCertificateError &error, QWidget *parent = nullptr);
    void authentication(const QUrl &url, QAuthenticator *auth, QWidget *parent = nullptr);
    void proxyAuthentication(const QString &proxyHost, QAuthenticator *auth, QWidget *parent = nullptr);

    void installUrlInterceptor(UrlInterceptor *interceptor);
    void removeUrlInterceptor(UrlInterceptor *interceptor);

    void registerExtensionSchemeHandler(const QString &name, ExtensionSchemeHandler *handler);
    void unregisterExtensionSchemeHandler(ExtensionSchemeHandler *handler);

    void loadSettings();
    void shutdown();

    static void registerSchemes();

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData) override;

private:
    NetworkUrlInterceptor *m_urlInterceptor;
    ExtensionSchemeManager *m_extensionScheme;
    QHash<QString, QWebEngineCertificateError::Error> m_ignoredSslErrors;
    QHash<QString, QWebEngineCertificateError::Error> m_rejectedSslErrors;
};

#endif // NETWORKMANAGER_H
