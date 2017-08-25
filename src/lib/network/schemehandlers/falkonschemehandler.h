/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#ifndef FALKONSCHEMEHANDLER_H
#define FALKONSCHEMEHANDLER_H

#include <QBuffer>
#include <QIODevice>
#include <QWebEngineUrlSchemeHandler>

#include "qzcommon.h"

class FALKON_EXPORT FalkonSchemeHandler : public QWebEngineUrlSchemeHandler
{
public:
    explicit FalkonSchemeHandler(QObject *parent = Q_NULLPTR);

    void requestStarted(QWebEngineUrlRequestJob *job) Q_DECL_OVERRIDE;
};

class FALKON_EXPORT FalkonSchemeReply : public QIODevice
{
    Q_OBJECT

public:
    explicit FalkonSchemeReply(QWebEngineUrlRequestJob *job, QObject *parent = Q_NULLPTR);

    qint64 bytesAvailable() const Q_DECL_OVERRIDE;
    qint64 readData(char *data, qint64 maxSize) Q_DECL_OVERRIDE;
    qint64 writeData(const char *data, qint64 len) Q_DECL_OVERRIDE;

private slots:
    void loadPage();

private:
    QString aboutPage();
    QString reportbugPage();
    QString startPage();
    QString speeddialPage();
    QString restorePage();
    QString configPage();
    QString adblockPage();

    bool m_loaded;
    QBuffer m_buffer;
    QString m_pageName;
    QWebEngineUrlRequestJob *m_job;
};

#endif // FALKONSCHEMEHANDLER_H
