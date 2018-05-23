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
#include "qmlhistoryapitest.h"
#include "autotests.h"
#include "qmltesthelper.h"
#include "mainapplication.h"
#include "history.h"
#include "qml/api/history/qmlhistoryitem.h"
#include "qml/api/history/qmlhistory.h"

void QmlHistoryApiTest::initTestCase()
{
}

void QmlHistoryApiTest::cleanupTestCase()
{
}

void QmlHistoryApiTest::testAddition()
{
    QmlTestHelper qmlTest;
    qRegisterMetaType<HistoryEntry>();
    QSignalSpy historySpy(mApp->history(), &History::historyEntryAdded);
    qmlTest.evaluate("Falkon.History.addUrl({"
                     "    url: 'https://example.com',"
                     "    title: 'Example Domain'"
                     "})");
    QTRY_COMPARE(historySpy.count(), 1);
    HistoryEntry entry = qvariant_cast<HistoryEntry>(historySpy.at(0).at(0));
    QCOMPARE(entry.title, "Example Domain");

    auto object = qmlTest.evaluateQObject("Falkon.History");
    QSignalSpy qmlHistorySpy(object, SIGNAL(visited(QmlHistoryItem*)));
    mApp->history()->addHistoryEntry(QUrl("https://sample.com"), "Sample Domain");
    QTRY_COMPARE(qmlHistorySpy.count(), 1);
    mApp->history()->clearHistory();
}

void QmlHistoryApiTest::testSearch()
{
    QmlTestHelper qmlTest;
    QSignalSpy historySpy(mApp->history(), &History::historyEntryAdded);
    mApp->history()->addHistoryEntry(QUrl("https://example.com"), "Example Domain");
    mApp->history()->addHistoryEntry(QUrl("https://another-example.com"), "Another Example Domain");
    mApp->history()->addHistoryEntry(QUrl("https://sample.com"), "Sample Domain");
    QTRY_COMPARE(historySpy.count(), 3);
    auto list = qmlTest.evaluate("Falkon.History.search({text: 'example'})").toVariant().toList();
    QCOMPARE(list.length(), 2);
}

void QmlHistoryApiTest::testVisits()
{
    QmlTestHelper qmlTest;
    int visits = qmlTest.evaluate("Falkon.History.getVisits({url: 'https://sample.com'})").toInt();
    QCOMPARE(visits, 1);
    QSignalSpy historySpy(mApp->history(), &History::historyEntryEdited);
    mApp->history()->addHistoryEntry(QUrl("https://sample.com"), "Sample Domain");
    QTRY_COMPARE(historySpy.count(), 1);
    visits = qmlTest.evaluate("Falkon.History.getVisits({url: 'https://sample.com'})").toInt();
    QCOMPARE(visits, 2);
}

void QmlHistoryApiTest::testRemoval()
{
    QmlTestHelper qmlTest;
    QSignalSpy historySpy(mApp->history(), &History::historyEntryDeleted);
    qmlTest.evaluate("Falkon.History.deleteUrl({url: 'https://sample.com'})");
    QTRY_COMPARE(historySpy.count(), 1);

    auto object = qmlTest.evaluateQObject("Falkon.History");
    QSignalSpy qmlHistorySpy(object, SIGNAL(visitRemoved(QmlHistoryItem*)));
    mApp->history()->deleteHistoryEntry("https://example.com", "Example Domain");
    QTRY_COMPARE(qmlHistorySpy.count(), 1);
}

FALKONTEST_MAIN(QmlHistoryApiTest)
