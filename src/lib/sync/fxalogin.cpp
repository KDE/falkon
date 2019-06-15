/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 Prasenjit Kumar Shaw <shawprasenjit07@gmail.com>
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
#include "fxalogin.h"

#include <QWebEnginePage>
#include <QWebEngineScript>
#include <QWebChannel>
#include <QJsonObject>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>

FxALoginPage::FxALoginPage(QWidget* parent)
    : QWebEngineView(parent)
{
    m_page = new QWebEnginePage();
    m_channel = new QWebChannel(m_page);
    m_page->setWebChannel(m_channel);
    m_page->load(FxALoginUrl);
    this->setPage(m_page);
    connect(m_page, SIGNAL(loadFinished(bool)), this, SLOT(pageLoadFinished(bool)));
}

FxALoginPage::~FxALoginPage()
{
    delete m_communicator;
    delete m_channel;
    delete m_page;
}

void  FxALoginPage::pageLoadFinished(bool pageLoaded)
{
    if(pageLoaded) {
        QFile apiFile(":/qtwebchannel/qwebchannel.js");
        if(!apiFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Couldn't load Qt's Webchannel API!";
        }
        QString apiScript = QString::fromLatin1(apiFile.readAll());
        apiFile.close();
        m_page->runJavaScript(apiScript);

        m_communicator = new MessageReceiver(this);
        connect(m_communicator, SIGNAL(signalMessageReceived()),
                this, SLOT(slotMessageReceived()));
        m_channel->registerObject(QString("communicator"), m_communicator);

        QFile scriptFile(":/data/inject.js");
        if(!scriptFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Couldn't load JavaScript file to inject.";
        }
        QString injectScript = QString::fromLatin1(scriptFile.readAll());
        scriptFile.close();
        m_page->runJavaScript(injectScript);
    }
}

void FxALoginPage::slotMessageReceived()
{
    QJsonObject *message = m_communicator->getMessage();
    qDebug() << ">>> Received from server:\n  " << (*message);
    parseMessage(message);

}

void FxALoginPage::parseMessage(QJsonObject *msg)
{
    QJsonValue command = (*msg).value("detail").toObject().value("message").toObject().value("command");
    if(command.toString() == QString("fxaccounts:can_link_account")) {
        QJsonObject responseData;
        responseData.insert("ok", true);
        QJsonObject message;
        message.insert("command", command);
        message.insert("data", responseData);
        message.insert("messageId", (*msg).value("detail").toObject().value("message").toObject().value("messageId"));
        QJsonObject response;
        response.insert("id", (*msg).value("detail").toObject().value("id"));
        response.insert("message", message);
        sendMessage(response);
    }
    else if(command.toString() == QString("fxaccounts:login")) {
        QJsonObject data = (*msg).value("detail").toObject().value("message").toObject().value("data").toObject();
        QString email = data.value("email").toString();
        QString uid = data.value("uid").toString();
        QString session_token = data.value("sessionToken").toString();
        QString key_fetch_token = data.value("keyFetchToken").toString();
        QString unwrap_kb = data.value("unwrapBKey").toString();

        qDebug() << "===Recieved login credentials:===\n"
                 << "  Email: " << email << '\n'
                 << "  UID: " << uid << '\n'
                 << "  Session Token: " << session_token << '\n'
                 << "  Key Fetch Token: " << key_fetch_token << '\n'
                 << "  unwrapBKey: " << unwrap_kb << '\n'
                 << "=================================\n";
    }
}

void FxALoginPage::sendMessage(QJsonObject msg)
{
    QJsonDocument doc(msg);
    QString stringMsg(doc.toJson(QJsonDocument::Compact));
    qDebug() << "<<< Sending to server:\n  " << stringMsg;

    QString srcCode = "sendMessage(" + stringMsg + ");";
    m_page->runJavaScript(srcCode);
}


MessageReceiver::MessageReceiver(QObject *parent)
    : QObject(parent)
{
}

MessageReceiver::~MessageReceiver()
{
    delete m_message;
}

void MessageReceiver::receiveJSON(const QVariantMap &data)
{
    QJsonObject obj = QJsonObject::fromVariantMap(data);
    m_message = new QJsonObject(obj);
    emit signalMessageReceived();
}

QJsonObject * MessageReceiver::getMessage()
{
    return m_message;
}
