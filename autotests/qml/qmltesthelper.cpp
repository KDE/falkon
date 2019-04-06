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
#include "qmltesthelper.h"
#include "qml/qmlplugins.h"
#include <QQmlComponent>
#include <QDebug>

QmlTestHelper::QmlTestHelper()
{
    QmlPlugins::registerQmlTypes();
    qmlRegisterType<QmlTestItem>("org.kde.falkon.test", 1, 0, "TestItem");
    QQmlComponent component(&engine);
    component.setData("import org.kde.falkon 1.0 as Falkon\n"
                      "import org.kde.falkon.test 1.0 as FalkonTest\n"
                      "import QtQuick 2.7\n"
                      "FalkonTest.TestItem {"
                      "    evalFunc: function(source) {"
                      "        return eval(source);"
                      "    }"
                      "}"
                      , QUrl());
    testItem = qobject_cast<QmlTestItem*>(component.create());
    Q_ASSERT(testItem);
}

QJSValue QmlTestHelper::evaluate(const QString &source)
{
    auto out = testItem->evaluate(source);
    if (out.isError()) {
        qWarning() << "Error:" << out.toString();
    }
    return out;
}

QObject *QmlTestHelper::evaluateQObject(const QString &source)
{
    auto out = evaluate(source);
    if (out.isQObject()) {
        return out.toQObject();
    }
    return out.toVariant().value<QObject*>();
}
