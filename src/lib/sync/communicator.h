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
#pragma once

#include <QObject>
#include <QJsonObject>

class Communicator : public QObject
{
    Q_OBJECT

public:
    Communicator(QObject *parent = nullptr);
    ~Communicator();
    QJsonObject *getMessage();

public Q_SLOTS:
    void receiveJSON(const QVariantMap &data);

Q_SIGNALS:
    void signalMessageReceived();

private:
    QJsonObject *m_message;
};
