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
#include "communicator.h"

#include <QJsonObject>

Communicator::Communicator(QObject *parent)
    : QObject(parent)
{
}

Communicator::~Communicator()
{
    delete m_message;
}

void Communicator::receiveJSON(const QVariantMap &data)
{
    QJsonObject obj = QJsonObject::fromVariantMap(data);
    m_message = new QJsonObject(obj);
    emit signalMessageReceived();
}

QJsonObject * Communicator::getMessage()
{
    return m_message;
}
