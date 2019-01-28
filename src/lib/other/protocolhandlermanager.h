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
#pragma once

#include <QUrl>
#include <QHash>
#include <QObject>

#include "qzcommon.h"

class FALKON_EXPORT ProtocolHandlerManager : public QObject
{
    Q_OBJECT

public:
    explicit ProtocolHandlerManager(QObject *parent = nullptr);

    QHash<QString, QUrl> protocolHandlers() const;

    void addProtocolHandler(const QString &scheme, const QUrl &url);
    void removeProtocolHandler(const QString &scheme);

private:
    void init();
    void save();
    void registerHandler(const QString &scheme, const QUrl &url);

    QHash<QString, QUrl> m_protocolHandlers;
};
