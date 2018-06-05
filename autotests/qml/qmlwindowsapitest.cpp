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
#include "qmlwindowsapitest.h"
#include "autotests.h"
#include "qmltesthelper.h"
#include "mainapplication.h"
#include "qml/api/windows/qmlwindow.h"
#include "pluginproxy.h"
#include "browserwindow.h"

void QmlWindowsApiTest::initTestCase()
{
}

void QmlWindowsApiTest::cleanupTestCase()
{
}

void QmlWindowsApiTest::testWindowsAPI()
{
    QmlTestHelper qmlTest;
    QObject *currentWindowObject = qmlTest.evaluateQObject("Falkon.Windows.getCurrent()");
    QVERIFY(currentWindowObject);
    QCOMPARE(currentWindowObject->property("id").toInt(), mApp->windowIdHash().value(mApp->getWindow()));
    QCOMPARE(currentWindowObject->property("title").toString(), mApp->getWindow()->windowTitle());
    QCOMPARE(currentWindowObject->property("type").toInt(), (int)mApp->getWindow()->windowType());
    QCOMPARE(currentWindowObject->property("tabs").toList().length(), mApp->getWindow()->tabCount());

    QObject *windowObject = qmlTest.evaluateQObject("Falkon.Windows");
    QVERIFY(windowObject);
    QSignalSpy qmlWindowCreatedSignal(windowObject, SIGNAL(created(QmlWindow*)));
    qRegisterMetaType<BrowserWindow*>();
    QSignalSpy windowCreatedSingal(mApp->plugins(), SIGNAL(mainWindowCreated(BrowserWindow*)));
    QObject *newQmlWindow = qmlTest.evaluateQObject("Falkon.Windows.create({})");
    QVERIFY(newQmlWindow);
    QCOMPARE(mApp->windowCount(), 2);
    // FIXME: signal is emitted 2 times here -
    // 1st for the initial window, 2nd for the created window
    QTRY_COMPARE(qmlWindowCreatedSignal.count(), 2);
    QTRY_COMPARE(windowCreatedSingal.count(), 2);
    QObject *newQmlSignalWindow = qvariant_cast<QObject*>(qmlWindowCreatedSignal.at(1).at(0));
    QVERIFY(newQmlSignalWindow);
    QCOMPARE(newQmlWindow->property("id").toInt(), newQmlSignalWindow->property("id").toInt());

    int qmlWindowCount = qmlTest.evaluate("Falkon.Windows.getAll().length").toInt();
    QCOMPARE(qmlWindowCount, mApp->windowCount());

    QSignalSpy qmlWindowRemovedSignal(windowObject, SIGNAL(removed(QmlWindow*)));
    int newQmlWindowId = newQmlSignalWindow->property("id").toInt();
    qmlTest.evaluate(QString("Falkon.Windows.remove(%1)").arg(newQmlWindowId));
    QTRY_COMPARE(qmlWindowRemovedSignal.count(), 1);
}

FALKONTEST_MAIN(QmlWindowsApiTest)
