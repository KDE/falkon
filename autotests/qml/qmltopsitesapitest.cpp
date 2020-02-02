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
#include "qmltopsitesapitest.h"
#include "autotests.h"
#include "qmltesthelper.h"
#include "mainapplication.h"
#include "pluginproxy.h"
#include "speeddial.h"

void QmlTopSitesApiTest::initTestCase()
{
}

void QmlTopSitesApiTest::cleanupTestCase()
{
}

void QmlTopSitesApiTest::testTopSites()
{
    mApp->plugins()->speedDial()->addPage(QUrl("https://example.com"), "Example Domain");
    auto list = m_testHelper.evaluate("Falkon.TopSites.get()").toVariant().toList();
    QCOMPARE(list.length(), 1);
    QObject* object = qvariant_cast<QObject*>(list.at(0));
    QVERIFY(object);
    QCOMPARE(object->property("title").toString(), QSL("Example Domain"));
    QCOMPARE(object->property("url").toString(), QSL("https://example.com"));
}

FALKONTEST_MAIN(QmlTopSitesApiTest)
