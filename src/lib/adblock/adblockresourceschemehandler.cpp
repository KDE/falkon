/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2025 Juraj Oravec <jurajoravec@mailo.com>
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

#include "adblockresourceschemehandler.h"
#include "qzcommon.h"

#include <QBuffer>
#include <QFile>
#include <QWebEngineUrlRequestJob>

AdblockResourceSchemeHandler::AdblockResourceSchemeHandler(QObject* parent)
    : QWebEngineUrlSchemeHandler(parent)
{
}

void AdblockResourceSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    const QString path = job->requestUrl().path();
    QByteArray contentType;
    QString content = QSL(":/adblock/data/blank.txt");

    if (path == QSL("blank-js")) {
        contentType = QByteArrayLiteral("text/javascript");
    }
    else if (path == QSL("blank-css")) {
        contentType = QByteArrayLiteral("text/css");
    }
    else if (path == QSL("blank-html")) {
        contentType = QByteArrayLiteral("text/html");
        content = QSL(":/adblock/data/blank.html");
    }
    else if (path == QSL("blank-text")) {
        contentType = QByteArrayLiteral("text/plain");
    }
    else if (path == QSL("1x1-transparent-gif")) {
        contentType = QByteArrayLiteral("image/gif");
        content = QSL(":/adblock/data/1x1-transparent.gif");
    }
    else if (path == QSL("2x2-transparent-png")) {
        contentType = QByteArrayLiteral("image/png");
        content = QSL(":/adblock/data/2x2-transparent.png");
    }
    else if (path == QSL("3x2-transparent-png")) {
        contentType = QByteArrayLiteral("image/png");
        content = QSL(":/adblock/data/3x2-transparent.png");
    }
    else if (path == QSL("32x32-transparent-png")) {
        contentType = QByteArrayLiteral("image/png");
        content = QSL(":/adblock/data/32x32-transparent.png");
    }
    else if (path == QSL("blank-mp3")) {
        contentType = QByteArrayLiteral("audio/mpeg");
        content = QSL(":/adblock/data/blank.mp3");
    }
    else if (path == QSL("blank-mp4")) {
        contentType = QByteArrayLiteral("video/mp4");
        content = QSL(":/adblock/data/blank.mp4");
    }
    else {
        contentType = QByteArrayLiteral("text/plain");
    }

    QFile *file = new QFile(content);
    if (file->open(QIODevice::ReadOnly)) {
        connect(file, &QIODevice::aboutToClose, file, &QObject::deleteLater);
        job->reply(contentType, file);
    }
    else {
        qWarning() << "AdblockResourceSchemeHandler: Unable to open resource file:" << content;

        delete file;
        job->fail(QWebEngineUrlRequestJob::RequestFailed);
    }
}
