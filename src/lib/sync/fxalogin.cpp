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
    page = new QWebEnginePage();
    channel = new QWebChannel(page);
    page->setWebChannel(channel);
    
    page->load(FxALoginUrl);
    this->setPage(page);
    connect(page, SIGNAL(loadFinished(bool)), this, SLOT(pageLoadFinished(bool)));
}

FxALoginPage::~FxALoginPage()
{
    delete communicator;
    delete channel;
    delete page;
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
        page->runJavaScript(apiScript);

        communicator = new MessageReceiver(this);
        connect(communicator, SIGNAL(signalMessageReceived()),
                this, SLOT(slotMessageReceived()));

        channel->registerObject(QString("communicator"), communicator);

        page->runJavaScript("new QWebChannel(qt.webChannelTransport, function(channel) {"
                            "    communicator = channel.objects.communicator;"
                            "    window.comm = communicator;"
                            "});"
                            "window.addEventListener('WebChannelMessageToChrome', function(event) {"
                            "    let e = {type: event.type, detail: event.detail};"
                            "    window.comm.receiveJSON(e);"
                            "});"
                            "function sendMessage(response) {  let e = new window.CustomEvent('WebChannelMessageToContent', {detail: response}); window.dispatchEvent(e)};"
                            );
    }
}

void FxALoginPage::slotMessageReceived()
{
    qDebug() << "===Msg Rcvd by FxALoginPage===";
    QJsonObject *message = communicator->getMessage();
    qDebug() << "Message Recieved: ";
    QJsonObject *response = parseMessage(message);
    sendMessage(response);
}

QJsonObject * FxALoginPage::parseMessage(QJsonObject *msg)
{
    /* TODO:
     *  Parse the recived message and respond with proper resonse
     */ 
    QJsonValue type = msg->value("type");
    QJsonObject detail = msg->value("detail").toObject();

    QJsonValue channelId = detail.value("id");
    QJsonObject message = detail.value("message").toObject();

    QJsonValue command = message.value("command");
    QJsonObject data = message.value("data").toObject();
    QJsonValue messageId = message.value("messageId");

    QJsonObject resp_data;
    resp_data.insert("ok", QJsonValue(true));

    QJsonObject resp_message;
    resp_message.insert("command", command);
    resp_message.insert("data", QJsonValue(resp_data));
    resp_message.insert("messageId", messageId);

    QJsonObject *response = new QJsonObject();
    response->insert("id", channelId);
    response->insert("message", QJsonValue(resp_message));

    return response;
}

void FxALoginPage::sendMessage(QJsonObject* msg)
{
    QJsonDocument doc(*msg);
    QString stringMsg(doc.toJson(QJsonDocument::Compact));
    qDebug() << ">>> Sending to server: " << stringMsg;

    QString srcCode = "sendMessage(" + stringMsg + ");";
    page->runJavaScript(srcCode);
}


MessageReceiver::MessageReceiver(QObject *parent)
    : QObject(parent)
{
}

MessageReceiver::~MessageReceiver()
{
    delete message;
}

void MessageReceiver::receiveJSON(const QVariantMap &data)
{
    QJsonObject obj = QJsonObject::fromVariantMap(data);
    qDebug() << "Received JSON:\n  " << obj;
    message = new QJsonObject(obj);
    emit signalMessageReceived();
}

QJsonObject * MessageReceiver::getMessage()
{
    return message;
}
