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
#include "qmltabsapitest.h"
#include "autotests.h"
#include "mainapplication.h"
#include "tabwidget.h"

void QmlTabsApiTest::initTestCase()
{
}

void QmlTabsApiTest::cleanupTestCase()
{
}

void QmlTabsApiTest::testInitWindowCount()
{
    QCOMPARE(mApp->windowCount(), 1);
    QTRY_COMPARE(mApp->getWindow()->tabCount(), 1);
}

void QmlTabsApiTest::testTabsAPI()
{
    /* Wait until the initial tab (at index 0) in the window is created */
    QTRY_COMPARE(mApp->getWindow()->tabCount(), 1);

    // Tab Insertion
    QObject *qmlTabsObject = m_testHelper.evaluateQObject(QSL("Falkon.Tabs"));
    QVERIFY(qmlTabsObject);
    QSignalSpy qmlTabsInsertedSpy(qmlTabsObject, SIGNAL(tabInserted(QVariantMap)));
    m_testHelper.evaluate(QL1S("Falkon.Tabs.addTab({"
                     "    url: 'https://example.com/'"
                     "})"));
    QCOMPARE(qmlTabsInsertedSpy.count(), 1);
    QVariantMap retMap1 = QVariant(qmlTabsInsertedSpy.at(0).at(0)).toMap();
    int index1 = retMap1.value(QSL("index"), -1).toInt();
    int windowId1 = retMap1.value(QSL("windowId"), -1).toInt();
    QCOMPARE(index1, 1);
    QCOMPARE(windowId1, 0);

    QObject *qmlTabObject1 = m_testHelper.evaluateQObject(QSL("Falkon.Tabs.get({index: 1})"));
    QVERIFY(qmlTabObject1);
    QCOMPARE(qmlTabObject1->property("index").toInt(), 1);
    QCOMPARE(qmlTabObject1->property("pinned").toBool(), false);
    QTRY_COMPARE(qmlTabObject1->property("url").toString(), QSL("https://example.com/"));

    m_testHelper.evaluate(QL1S("Falkon.Tabs.addTab({"
                     "    url: 'https://another-example.com/'"
                     "})"));
    QCOMPARE(qmlTabsInsertedSpy.count(), 2);
    QVariantMap retMap2 = QVariant(qmlTabsInsertedSpy.at(1).at(0)).toMap();
    int index2 = retMap2.value(QSL("index"), -1).toInt();
    int windowId2 = retMap2.value(QSL("windowId"), -1).toInt();
    QCOMPARE(index2, 2);
    QCOMPARE(windowId2, 0);

    bool pinnedTab = m_testHelper.evaluate(QSL("Falkon.Tabs.pinTab({index: 2})")).toBool();
    QVERIFY(pinnedTab);
    QObject *qmlTabObject2 = m_testHelper.evaluateQObject(QSL("Falkon.Tabs.get({index: 0})"));
    QVERIFY(qmlTabObject2);
    QCOMPARE(qmlTabObject2->property("index").toInt(), 0);
    QCOMPARE(qmlTabObject2->property("pinned").toBool(), true);
    QTRY_COMPARE(qmlTabObject2->property("url").toString(), QSL("https://another-example.com/"));

    bool unpinnedTab = m_testHelper.evaluate(QSL("Falkon.Tabs.unpinTab({index: 0})")).toBool();
    QVERIFY(unpinnedTab);
    QObject *qmlTabObject3 = m_testHelper.evaluateQObject(QSL("Falkon.Tabs.get({index: 0})"));
    QVERIFY(qmlTabObject3);
    QCOMPARE(qmlTabObject3->property("url").toString(), QSL("https://another-example.com/"));
    QCOMPARE(qmlTabObject3->property("index").toInt(), 0);
    QCOMPARE(qmlTabObject3->property("pinned").toBool(), false);

    // Next-Previous-Current
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 0);
    m_testHelper.evaluate(QSL("Falkon.Tabs.nextTab()"));
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 1);
    m_testHelper.evaluate(QSL("Falkon.Tabs.nextTab()"));
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 2);
    m_testHelper.evaluate(QSL("Falkon.Tabs.nextTab()"));
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 0);
    m_testHelper.evaluate(QSL("Falkon.Tabs.previousTab()"));
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 2);
    m_testHelper.evaluate(QSL("Falkon.Tabs.previousTab()"));
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 1);
    m_testHelper.evaluate(QSL("Falkon.Tabs.previousTab()"));
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 0);
    m_testHelper.evaluate(QSL("Falkon.Tabs.setCurrentIndex({index: 2})"));
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 2);
    m_testHelper.evaluate(QSL("Falkon.Tabs.setCurrentIndex({index: 1})"));
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 1);
    m_testHelper.evaluate(QSL("Falkon.Tabs.setCurrentIndex({index: 0})"));
    QCOMPARE(mApp->getWindow()->tabWidget()->currentIndex(), 0);

    // Move Tab
    QSignalSpy qmlTabsMovedSpy(qmlTabsObject, SIGNAL(tabMoved(QVariantMap)));
    m_testHelper.evaluate(QSL("Falkon.Tabs.moveTab({from: 0, to:1, windowId: 0})"));
    QCOMPARE(qmlTabsMovedSpy.count(), 1);

    // Tab Removal
    QCOMPARE(mApp->getWindow()->tabCount(), 3);
    QSignalSpy qmlTabsRemovedSpy(qmlTabsObject, SIGNAL(tabRemoved(QVariantMap)));
    m_testHelper.evaluate(QSL("Falkon.Tabs.closeTab({index: 0})"));
    QCOMPARE(qmlTabsRemovedSpy.count(), 1);
    QCOMPARE(mApp->getWindow()->tabCount(), 2);
}

FALKONTEST_MAIN(QmlTabsApiTest)
