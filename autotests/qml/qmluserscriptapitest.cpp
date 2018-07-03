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
#include "qmluserscriptapitest.h"
#include "autotests.h"
#include "qmltesthelper.h"
#include "mainapplication.h"
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include "qml/api/userscript/qmluserscript.h"
#include "qml/api/userscript/qmluserscripts.h"

void QmlUserScriptApiTest::initTestCase()
{
}

void QmlUserScriptApiTest::cleanupTestCase()
{
}

void QmlUserScriptApiTest::testCount()
{
    QmlTestHelper qmlTest;
    int count = qmlTest.evaluate("Falkon.UserScripts.count").toInt();
    QCOMPARE(count, mApp->webProfile()->scripts()->count());
}

void QmlUserScriptApiTest::testSize()
{
    QmlTestHelper qmlTest;
    int size = qmlTest.evaluate("Falkon.UserScripts.size").toInt();
    QCOMPARE(size, mApp->webProfile()->scripts()->size());
}

void QmlUserScriptApiTest::testEmpty()
{
    QmlTestHelper qmlTest;
    bool empty = qmlTest.evaluate("Falkon.UserScripts.empty").toBool();
    QCOMPARE(empty, mApp->webProfile()->scripts()->isEmpty());
}

void QmlUserScriptApiTest::testContains()
{
    QmlTestHelper qmlTest;
    QWebEngineScript script = mApp->webProfile()->scripts()->toList().at(0);
    QObject *object = qmlTest.evaluateQObject("Falkon.UserScripts");
    QmlUserScripts *userScripts = dynamic_cast<QmlUserScripts*>(object);
    QVERIFY(userScripts);
    QmlUserScript *userScript = new QmlUserScript();
    userScript->setWebEngineScript(script);
    bool contains = userScripts->contains(userScript);
    QCOMPARE(contains, true);
}

void QmlUserScriptApiTest::testFind()
{
    QmlTestHelper qmlTest;
    QWebEngineScript script = mApp->webProfile()->scripts()->toList().at(0);
    QObject *object = qmlTest.evaluateQObject("Falkon.UserScripts");
    QmlUserScripts *userScripts = dynamic_cast<QmlUserScripts*>(object);
    QVERIFY(userScripts);
    QObject *scriptFound = userScripts->findScript(script.name());
    QVERIFY(scriptFound);
    QCOMPARE(scriptFound->property("name").toString(), script.name());
}

void QmlUserScriptApiTest::testInsertRemove()
{
    QmlTestHelper qmlTest;
    int initialCount = qmlTest.evaluate("Falkon.UserScripts.count").toInt();
    QObject *object = qmlTest.evaluateQObject("Falkon.UserScripts");
    QmlUserScripts *userScripts = dynamic_cast<QmlUserScripts*>(object);
    QVERIFY(userScripts);
    QmlUserScript *userScript = new QmlUserScript();
    userScript->setProperty("name", "Hello World");
    userScript->setProperty("sourceCode", "(function() {"
                              "    alert('Hello World')"
                              "})()");
    userScripts->insert(userScript);
    int finalCount = qmlTest.evaluate("Falkon.UserScripts.count").toInt();
    QCOMPARE(finalCount, initialCount + 1);

    userScripts->remove(userScript);

    int ultimateCount = qmlTest.evaluate("Falkon.UserScripts.count").toInt();
    QCOMPARE(ultimateCount, initialCount);
}

FALKONTEST_MAIN(QmlUserScriptApiTest)
