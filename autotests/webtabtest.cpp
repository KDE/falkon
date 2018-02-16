/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 David Rosca <nowrep@gmail.com>
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
#include "webtabtest.h"
#include "autotests.h"
#include "webtab.h"
#include "tabwidget.h"
#include "tabbedwebview.h"
#include "mainapplication.h"
#include "browserwindow.h"

#include <QWebEngineHistory>

void WebTabTest::initTestCase()
{
}

void WebTabTest::cleanupTestCase()
{
    WebTab::setAddChildBehavior(WebTab::AppendChild);
}

void WebTabTest::init()
{
    WebTab::setAddChildBehavior(WebTab::AppendChild);
}

void WebTabTest::parentChildTabsTest()
{
    WebTab tab1;
    WebTab tab2;
    WebTab tab3;
    WebTab tab4;
    WebTab tab5;
    WebTab tab6;

    tab1.addChildTab(&tab2);
    QCOMPARE(tab1.childTabs(), QVector<WebTab*>{&tab2});
    QCOMPARE(tab2.parentTab(), &tab1);
    QCOMPARE(tab2.childTabs(), QVector<WebTab*>{});

    tab1.addChildTab(&tab3);
    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab2, &tab3}));
    QCOMPARE(tab3.parentTab(), &tab1);
    QCOMPARE(tab3.childTabs(), QVector<WebTab*>{});

    tab1.addChildTab(&tab4, 1);
    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab2, &tab4, &tab3}));
    QCOMPARE(tab4.parentTab(), &tab1);
    QCOMPARE(tab4.childTabs(), QVector<WebTab*>{});

    tab4.addChildTab(&tab5);
    tab4.addChildTab(&tab6);

    tab4.attach(mApp->getWindow());
    tab4.detach();

    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab2, &tab5, &tab6, &tab3}));
    QCOMPARE(tab4.parentTab(), nullptr);
    QCOMPARE(tab4.childTabs(), QVector<WebTab*>{});

    tab3.addChildTab(&tab4);
    tab3.setParentTab(nullptr);
    tab1.addChildTab(&tab3, 0);

    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab3, &tab2, &tab5, &tab6}));
    QCOMPARE(tab3.parentTab(), &tab1);
    QCOMPARE(tab3.childTabs(), QVector<WebTab*>{&tab4});
    QCOMPARE(tab4.parentTab(), &tab3);

    tab3.addChildTab(&tab2);
    QCOMPARE(tab3.childTabs(), (QVector<WebTab*>{&tab4, &tab2}));
    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab3, &tab5, &tab6}));
}

void WebTabTest::prependChildTabsTest()
{
    WebTab::setAddChildBehavior(WebTab::PrependChild);

    WebTab tab1;
    WebTab tab2;
    WebTab tab3;
    WebTab tab4;
    WebTab tab5;
    WebTab tab6;

    tab1.addChildTab(&tab2);
    QCOMPARE(tab1.childTabs(), QVector<WebTab*>{&tab2});
    QCOMPARE(tab2.parentTab(), &tab1);
    QCOMPARE(tab2.childTabs(), QVector<WebTab*>{});

    tab1.addChildTab(&tab3);
    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab3, &tab2}));
    QCOMPARE(tab3.parentTab(), &tab1);
    QCOMPARE(tab3.childTabs(), QVector<WebTab*>{});

    tab1.addChildTab(&tab4, 1);
    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab3, &tab4, &tab2}));
    QCOMPARE(tab4.parentTab(), &tab1);
    QCOMPARE(tab4.childTabs(), QVector<WebTab*>{});

    tab4.addChildTab(&tab5);
    tab4.addChildTab(&tab6);

    QCOMPARE(tab4.childTabs(), (QVector<WebTab*>{&tab6, &tab5}));

    tab4.attach(mApp->getWindow());
    tab4.detach();

    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab3, &tab6, &tab5, &tab2}));
    QCOMPARE(tab4.parentTab(), nullptr);
    QCOMPARE(tab4.childTabs(), QVector<WebTab*>{});

    tab3.addChildTab(&tab4);
    tab3.setParentTab(nullptr);
    tab1.addChildTab(&tab3, 0);

    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab3, &tab6, &tab5, &tab2}));
    QCOMPARE(tab3.parentTab(), &tab1);
    QCOMPARE(tab3.childTabs(), QVector<WebTab*>{&tab4});
    QCOMPARE(tab4.parentTab(), &tab3);

    tab3.addChildTab(&tab2);
    QCOMPARE(tab3.childTabs(), (QVector<WebTab*>{&tab2, &tab4}));
    QCOMPARE(tab1.childTabs(), (QVector<WebTab*>{&tab3, &tab6, &tab5}));
}

void WebTabTest::moveTabTest()
{
    BrowserWindow *w = mApp->createWindow(Qz::BW_NewWindow);

    QSignalSpy movedSpy(w->tabWidget(), &TabWidget::tabMoved);

    w->tabWidget()->addView(QUrl());
    w->tabWidget()->addView(QUrl());
    w->tabWidget()->addView(QUrl());
    w->tabWidget()->addView(QUrl());
    QTRY_COMPARE(w->tabWidget()->count(), 5);

    WebTab *tab0 = w->tabWidget()->webTab(0);
    WebTab *tab1 = w->tabWidget()->webTab(1);
    WebTab *tab2 = w->tabWidget()->webTab(2);
    WebTab *tab3 = w->tabWidget()->webTab(3);
    WebTab *tab4 = w->tabWidget()->webTab(4);

    movedSpy.clear();
    tab0->moveTab(2); // 1, 2, 0, 3, 4
    QCOMPARE(movedSpy.count(), 1);
    QCOMPARE(movedSpy.at(0).at(0).toInt(), 0);
    QCOMPARE(movedSpy.at(0).at(1).toInt(), 2);

    QCOMPARE(w->tabWidget()->webTab(0), tab1);
    QCOMPARE(w->tabWidget()->webTab(1), tab2);
    QCOMPARE(w->tabWidget()->webTab(2), tab0);
    QCOMPARE(w->tabWidget()->webTab(3), tab3);
    QCOMPARE(w->tabWidget()->webTab(4), tab4);

    movedSpy.clear();
    tab4->moveTab(0); // 4, 1, 2, 0, 3
    QCOMPARE(movedSpy.count(), 1);
    QCOMPARE(movedSpy.at(0).at(0).toInt(), 4);
    QCOMPARE(movedSpy.at(0).at(1).toInt(), 0);

    tab4->togglePinned(); // [4], 1, 2, 0, 3

    movedSpy.clear();
    tab2->moveTab(0); // [2, 4], 1, 0, 3
    QCOMPARE(tab2->isPinned(), true);
    QCOMPARE(movedSpy.count(), 1);
    QCOMPARE(movedSpy.at(0).at(0).toInt(), 2);
    QCOMPARE(movedSpy.at(0).at(1).toInt(), 0);

    movedSpy.clear();
    tab0->moveTab(1); // [2, 0, 4], 1, 3
    QCOMPARE(tab0->isPinned(), true);
    QCOMPARE(movedSpy.count(), 1);
    QCOMPARE(movedSpy.at(0).at(0).toInt(), 3);
    QCOMPARE(movedSpy.at(0).at(1).toInt(), 1);

    QCOMPARE(w->tabWidget()->webTab(0), tab2);
    QCOMPARE(w->tabWidget()->webTab(1), tab0);
    QCOMPARE(w->tabWidget()->webTab(2), tab4);
    QCOMPARE(w->tabWidget()->webTab(3), tab1);
    QCOMPARE(w->tabWidget()->webTab(4), tab3);

    movedSpy.clear();
    tab0->moveTab(0); // [0, 2, 4], 1, 3
    QCOMPARE(movedSpy.count(), 1);
    QCOMPARE(movedSpy.at(0).at(0).toInt(), 1);
    QCOMPARE(movedSpy.at(0).at(1).toInt(), 0);

    QCOMPARE(w->tabWidget()->webTab(0), tab0);
    QCOMPARE(w->tabWidget()->webTab(1), tab2);
    QCOMPARE(w->tabWidget()->webTab(2), tab4);
    QCOMPARE(w->tabWidget()->webTab(3), tab1);
    QCOMPARE(w->tabWidget()->webTab(4), tab3);

    movedSpy.clear();
    tab0->moveTab(3); // [2, 4], 1, 0, 3
    QCOMPARE(tab0->isPinned(), false);
    QCOMPARE(movedSpy.count(), 1);
    QCOMPARE(movedSpy.at(0).at(0).toInt(), 0);
    QCOMPARE(movedSpy.at(0).at(1).toInt(), 3);

    QCOMPARE(w->tabWidget()->webTab(0), tab2);
    QCOMPARE(w->tabWidget()->webTab(1), tab4);
    QCOMPARE(w->tabWidget()->webTab(2), tab1);
    QCOMPARE(w->tabWidget()->webTab(3), tab0);
    QCOMPARE(w->tabWidget()->webTab(4), tab3);

    movedSpy.clear();
    tab2->moveTab(4); // [4], 1, 0, 3, 2
    QCOMPARE(tab0->isPinned(), false);
    QCOMPARE(movedSpy.count(), 1);
    QCOMPARE(movedSpy.at(0).at(0).toInt(), 0);
    QCOMPARE(movedSpy.at(0).at(1).toInt(), 4);

    movedSpy.clear();
    tab4->moveTab(2); // 1, 0, 4, 3, 2
    QCOMPARE(tab0->isPinned(), false);
    QCOMPARE(movedSpy.count(), 1);
    QCOMPARE(movedSpy.at(0).at(0).toInt(), 0);
    QCOMPARE(movedSpy.at(0).at(1).toInt(), 2);

    QCOMPARE(w->tabWidget()->webTab(0), tab1);
    QCOMPARE(w->tabWidget()->webTab(1), tab0);
    QCOMPARE(w->tabWidget()->webTab(2), tab4);
    QCOMPARE(w->tabWidget()->webTab(3), tab3);
    QCOMPARE(w->tabWidget()->webTab(4), tab2);

    // Invalid moves
    movedSpy.clear();
    tab4->moveTab(tab4->tabIndex());
    tab4->moveTab(-1);
    tab4->moveTab(5);
    tab4->moveTab(6);
    QCOMPARE(movedSpy.count(), 0);

    delete w;
}

void WebTabTest::loadNotRestoredTabTest()
{
    WebTab tab;

    tab.load(QUrl("qrc:autotests/data/basic_page.html"));
    QVERIFY(waitForLoadfinished(&tab));
    QTRY_COMPARE(tab.webView()->history()->count(), 1);

    tab.unload();
    QVERIFY(!tab.isRestored());

    tab.load(QUrl("qrc:autotests/data/basic_page2.html"));
    QVERIFY(waitForLoadfinished(&tab));
    QTRY_COMPARE(tab.webView()->history()->count(), 2);

    QCOMPARE(tab.url(), QUrl("qrc:autotests/data/basic_page2.html"));
    QCOMPARE(tab.webView()->history()->currentItem().url(), QUrl("qrc:autotests/data/basic_page2.html"));
    QCOMPARE(tab.webView()->history()->backItem().url(), QUrl("qrc:autotests/data/basic_page.html"));
}

void WebTabTest::saveNotRestoredTabTest()
{
    WebTab tab;

    tab.load(QUrl("qrc:autotests/data/basic_page.html"));
    QVERIFY(waitForLoadfinished(&tab));
    QTRY_COMPARE(tab.webView()->history()->count(), 1);

    tab.unload();
    QVERIFY(!tab.isRestored());

    WebTab::SavedTab saved(&tab);
    QVERIFY(saved.isValid());
    QCOMPARE(saved.url, QUrl("qrc:autotests/data/basic_page.html"));
}

FALKONTEST_MAIN(WebTabTest)
