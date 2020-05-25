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
#include "networkmanager.h"
#include "autofill.h"
#include "qztools.h"
#include "settings.h"
#include "cookiejar.h"
#include "acceptlanguage.h"
#include "mainapplication.h"
#include "passwordmanager.h"
#include "sslerrordialog.h"
#include "networkurlinterceptor.h"
#include "schemehandlers/falkonschemehandler.h"
#include "schemehandlers/extensionschemehandler.h"
#include "webpage.h"

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QFormLayout>
#include <QAuthenticator>
#include <QDialogButtonBox>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QWebEngineProfile>
#include <QWebEngineCertificateError>
#include <QtWebEngineWidgetsVersion>

#if QTWEBENGINEWIDGETS_VERSION >= QT_VERSION_CHECK(5, 12, 0)
#include <QWebEngineUrlScheme>
#endif

NetworkManager::NetworkManager(QObject *parent)
    : QNetworkAccessManager(parent)
{
    // Create scheme handlers
    m_extensionScheme = new ExtensionSchemeManager();

    mApp->webProfile()->installUrlSchemeHandler(QByteArrayLiteral("falkon"), new FalkonSchemeHandler());
    mApp->webProfile()->installUrlSchemeHandler(QByteArrayLiteral("extension"), m_extensionScheme);
    WebPage::addSupportedScheme(QSL("falkon"));
    WebPage::addSupportedScheme(QSL("extension"));

    // Create url interceptor
    m_urlInterceptor = new NetworkUrlInterceptor(this);
#if QTWEBENGINEWIDGETS_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    mApp->webProfile()->setUrlRequestInterceptor(m_urlInterceptor);
#else
    mApp->webProfile()->setRequestInterceptor(m_urlInterceptor);
#endif

    // Create cookie jar
    mApp->cookieJar();

    connect(this, &QNetworkAccessManager::authenticationRequired, this, [this](QNetworkReply *reply, QAuthenticator *auth) {
        authentication(reply->url(), auth);
    });

    connect(this, &QNetworkAccessManager::proxyAuthenticationRequired, this, [this](const QNetworkProxy &proxy, QAuthenticator *auth) {
        proxyAuthentication(proxy.hostName(), auth);
    });
}

bool NetworkManager::certificateError(const QWebEngineCertificateError &error, QWidget *parent)
{
    const QString &host = error.url().host();

    if (m_rejectedSslErrors.contains(host) && m_rejectedSslErrors.value(host) == error.error()) {
        return false;
    }

    if (m_ignoredSslErrors.contains(host) && m_ignoredSslErrors.value(host) == error.error())
        return true;

    QString title = tr("SSL Certificate Error!");
    QString text1 = tr("The page you are trying to access has the following errors in the SSL certificate:");
    QString text2 = tr("Would you like to make an exception for this certificate?");

    QString message = QSL("<b>%1</b><p>%2</p><ul><li>%3</li></ul><p>%4</p>").arg(title, text1, error.errorDescription(), text2);

    SslErrorDialog dialog(parent);
    dialog.setText(message);
    dialog.exec();

    switch (dialog.result()) {
    case SslErrorDialog::Yes:
        // TODO: Permanent exceptions
    case SslErrorDialog::OnlyForThisSession:
        m_ignoredSslErrors[host] = error.error();
        return true;

    case SslErrorDialog::NoForThisSession:
        m_rejectedSslErrors[host] = error.error();
        return false;

    default:
        return false;
    }
}

void NetworkManager::authentication(const QUrl &url, QAuthenticator *auth, QWidget *parent)
{
    QDialog* dialog = new QDialog(parent);
    dialog->setWindowTitle(tr("Authorization required"));

    QFormLayout* formLa = new QFormLayout(dialog);

    QLabel* label = new QLabel(dialog);
    QLabel* userLab = new QLabel(dialog);
    QLabel* passLab = new QLabel(dialog);
    userLab->setText(tr("Username: "));
    passLab->setText(tr("Password: "));

    QLineEdit* user = new QLineEdit(dialog);
    QLineEdit* pass = new QLineEdit(dialog);
    pass->setEchoMode(QLineEdit::Password);
    QCheckBox* save = new QCheckBox(dialog);
    save->setText(tr("Save username and password for this site"));

    QDialogButtonBox* box = new QDialogButtonBox(dialog);
    box->addButton(QDialogButtonBox::Ok);
    box->addButton(QDialogButtonBox::Cancel);
    connect(box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    connect(box, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    label->setText(tr("A username and password are being requested by %1. "
                      "The site says: \"%2\"").arg(url.host(), auth->realm().toHtmlEscaped()));

    formLa->addRow(label);
    formLa->addRow(userLab, user);
    formLa->addRow(passLab, pass);
    formLa->addRow(save);
    formLa->addWidget(box);

    AutoFill* fill = mApp->autoFill();
    QString storedUser;
    QString storedPassword;
    bool shouldUpdateEntry = false;

    if (fill->isStored(url)) {
        const QVector<PasswordEntry> &data = fill->getFormData(url);
        if (!data.isEmpty()) {
            save->setChecked(true);
            shouldUpdateEntry = true;
            storedUser = data.at(0).username;
            storedPassword = data.at(0).password;
            user->setText(storedUser);
            pass->setText(storedPassword);
        }
    }

    // Do not save when private browsing is enabled
    if (mApp->isPrivate()) {
        save->setVisible(false);
    }

    if (dialog->exec() != QDialog::Accepted) {
        *auth = QAuthenticator();
        delete dialog;
        return;
    }

    auth->setUser(user->text());
    auth->setPassword(pass->text());

    if (save->isChecked()) {
        if (shouldUpdateEntry) {
            if (storedUser != user->text() || storedPassword != pass->text()) {
                fill->updateEntry(url, user->text(), pass->text());
            }
        }
        else {
            fill->addEntry(url, user->text(), pass->text());
        }
    }

    delete dialog;
}

void NetworkManager::proxyAuthentication(const QString &proxyHost, QAuthenticator *auth, QWidget *parent)
{
    const QNetworkProxy proxy = QNetworkProxy::applicationProxy();
    if (!proxy.user().isEmpty() && !proxy.password().isEmpty()) {
        auth->setUser(proxy.user());
        auth->setPassword(proxy.password());
        return;
    }

    QDialog* dialog = new QDialog(parent);
    dialog->setWindowTitle(tr("Proxy authorization required"));

    QFormLayout* formLa = new QFormLayout(dialog);

    QLabel* label = new QLabel(dialog);
    QLabel* userLab = new QLabel(dialog);
    QLabel* passLab = new QLabel(dialog);
    userLab->setText(tr("Username: "));
    passLab->setText(tr("Password: "));

    QLineEdit* user = new QLineEdit(dialog);
    QLineEdit* pass = new QLineEdit(dialog);
    pass->setEchoMode(QLineEdit::Password);

    QDialogButtonBox* box = new QDialogButtonBox(dialog);
    box->addButton(QDialogButtonBox::Ok);
    box->addButton(QDialogButtonBox::Cancel);
    connect(box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    connect(box, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    label->setText(tr("A username and password are being requested by proxy %1. ").arg(proxyHost));
    formLa->addRow(label);
    formLa->addRow(userLab, user);
    formLa->addRow(passLab, pass);
    formLa->addWidget(box);

    if (dialog->exec() != QDialog::Accepted) {
        *auth = QAuthenticator();
        delete dialog;
        return;
    }

    auth->setUser(user->text());
    auth->setPassword(pass->text());

    delete dialog;
}

void NetworkManager::installUrlInterceptor(UrlInterceptor *interceptor)
{
    m_urlInterceptor->installUrlInterceptor(interceptor);
}

void NetworkManager::removeUrlInterceptor(UrlInterceptor *interceptor)
{
    m_urlInterceptor->removeUrlInterceptor(interceptor);
}

void NetworkManager::registerExtensionSchemeHandler(const QString &name, ExtensionSchemeHandler *handler)
{
    m_extensionScheme->registerHandler(name, handler);
}

void NetworkManager::unregisterExtensionSchemeHandler(ExtensionSchemeHandler *handler)
{
    m_extensionScheme->unregisterHandler(handler);
}

void NetworkManager::loadSettings()
{
    Settings settings;
    settings.beginGroup("Language");
    QStringList langs = settings.value("acceptLanguage", AcceptLanguage::defaultLanguage()).toStringList();
    settings.endGroup();
    mApp->webProfile()->setHttpAcceptLanguage(AcceptLanguage::generateHeader(langs));

    QNetworkProxy proxy;
    settings.beginGroup("Web-Proxy");
    const int proxyType = settings.value("ProxyType", 2).toInt();
    proxy.setHostName(settings.value("HostName", QString()).toString());
    proxy.setPort(settings.value("Port", 8080).toInt());
    proxy.setUser(settings.value("Username", QString()).toString());
    proxy.setPassword(settings.value("Password", QString()).toString());
    settings.endGroup();

    if (proxyType == 0) {
        proxy.setType(QNetworkProxy::NoProxy);
    } else if (proxyType == 3) {
        proxy.setType(QNetworkProxy::HttpProxy);
    } else if (proxyType == 4) {
        proxy.setType(QNetworkProxy::Socks5Proxy);
    }

    if (proxyType == 2) {
        QNetworkProxy::setApplicationProxy(QNetworkProxy());
        QNetworkProxyFactory::setUseSystemConfiguration(true);
    } else {
        QNetworkProxy::setApplicationProxy(proxy);
        QNetworkProxyFactory::setUseSystemConfiguration(false);
    }

    m_urlInterceptor->loadSettings();
}

void NetworkManager::shutdown()
{
#if QTWEBENGINEWIDGETS_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    mApp->webProfile()->setUrlRequestInterceptor(nullptr);
#else
    mApp->webProfile()->setRequestInterceptor(nullptr);
#endif
}

// static
void NetworkManager::registerSchemes()
{
#if QTWEBENGINEWIDGETS_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    QWebEngineUrlScheme falkonScheme("falkon");
    falkonScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored);
    falkonScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    QWebEngineUrlScheme::registerScheme(falkonScheme);
    QWebEngineUrlScheme extensionScheme("extension");
    extensionScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::ContentSecurityPolicyIgnored);
    extensionScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    QWebEngineUrlScheme::registerScheme(extensionScheme);
#endif
}

QNetworkReply *NetworkManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    QNetworkRequest req = request;
    req.setAttribute(QNetworkRequest::SpdyAllowedAttribute, true);
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    return QNetworkAccessManager::createRequest(op, req, outgoingData);
}
