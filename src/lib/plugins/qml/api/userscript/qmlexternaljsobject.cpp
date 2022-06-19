/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmlexternaljsobject.h"
#include "javascript/externaljsobject.h"
#include <QQmlEngine>

QmlExternalJsObject::QmlExternalJsObject(QObject *parent)
    : QObject(parent)
{
}

QmlExternalJsObject::~QmlExternalJsObject()
{
    for (QObject *object : qAsConst(m_objects)) {
        ExternalJsObject::unregisterExtraObject(object);
    }
}

void QmlExternalJsObject::registerExtraObject(const QVariantMap &map)
{
    if (!map.contains(QSL("id")) || !map.contains(QSL("object"))) {
        qWarning() << "Unable to call" << __FUNCTION__ << ": insufficient parameters";
        return;
    }

    const QString id = map.value(QSL("id")).toString();
    auto *object = qvariant_cast<QObject*>(map.value(QSL("object")));
    if (!object) {
        qWarning() << "Unable to cast to QObject";
        return;
    }
    ExternalJsObject::registerExtraObject(id, object);
    m_objects.append(object);
}

void QmlExternalJsObject::unregisterExtraObject(QObject *object)
{
    ExternalJsObject::unregisterExtraObject(object);
}
