/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2018 David Rosca <nowrep@gmail.com>
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
#include "adblocktest.h"
#include "adblockrule.h"
#include "adblocksubscription.h"

#include <QtTest/QtTest>

class AdBlockRule_Test : public AdBlockRule
{
public:
    QStringList parseRegExpFilter(const QString &parsedFilter)
    {
        return AdBlockRule::parseRegExpFilter(parsedFilter);
    }

    bool isMatchingDomain(const QString &domain, const QString &filter) const
    {
        return AdBlockRule::isMatchingDomain(domain, filter);
    }
};

void AdBlockTest::isMatchingCookieTest_data()
{
    // Test copied from CookiesTest
    QTest::addColumn<QString>("filterDomain");
    QTest::addColumn<QString>("siteDomain");
    QTest::addColumn<bool>("result");

    QTest::newRow("test1") << "example.com" << "www.example.com" << true;
    QTest::newRow("test2") << "example.com" << "example.com" << true;
    QTest::newRow("test3") << "example.com" << "anotherexample.com" << false;
    QTest::newRow("test4") << "test.example.com" << "example.com" << false;
    QTest::newRow("test5") << "www.example.com" << "example.com" << false;
    QTest::newRow("test_empty") << "www.example.com" << "" << false;
    QTest::newRow("test_empty2") << "" << "example.com" << false;
}

void AdBlockTest::isMatchingCookieTest()
{
    AdBlockRule_Test rule_test;

    QFETCH(QString, filterDomain);
    QFETCH(QString, siteDomain);
    QFETCH(bool, result);

    QCOMPARE(rule_test.isMatchingDomain(siteDomain, filterDomain), result);
}

void AdBlockTest::parseRegExpFilterTest_data()
{
    QTest::addColumn<QString>("parsedFilter");
    QTest::addColumn<QStringList>("result");

    QTest::newRow("test1") << QSL("||doubleclick.net/pfadx/tmg.telegraph.")
                           << (QStringList() << QSL("doubleclick.net/pfadx/tmg.telegraph."));
    QTest::newRow("test2") << QSL("||doubleclick.net/pfadx/*.mtvi")
                           << (QStringList() << QSL("doubleclick.net/pfadx/") << QSL(".mtvi"));
    QTest::newRow("test3") << QSL("&prvtof=*&poru=")
                           << (QStringList() << QSL("&prvtof=") << QSL("&poru="));
    QTest::newRow("test4") << QSL("/addyn|*;adtech;")
                           << (QStringList() << QSL("/addyn") << QSL(";adtech;"));
    QTest::newRow("test5") << QSL("/eas_fif.html^")
                           << (QStringList() << QSL("/eas_fif.html"));
    QTest::newRow("test6") << QSL("://findnsave.^.*/api/groupon.json?")
                           << (QStringList() << QSL("://findnsave.") << QSL("/api/groupon.json?"));
    QTest::newRow("test7") << QSL("^fp=*&prvtof=")
                           << (QStringList() << QSL("fp=") << QSL("&prvtof="));
    QTest::newRow("test8") << QSL("|http://ax-d.*/jstag^")
                           << (QStringList() << QSL("http://ax-d.") << QSL("/jstag"));
    QTest::newRow("test9") << QSL("||reuters.com^*/rcom-wt-mlt.js")
                           << (QStringList() << QSL("reuters.com") <<QSL("/rcom-wt-mlt.js"));
    QTest::newRow("test10") << QSL("||chip.de^*/tracking.js")
                           << (QStringList() << QSL("chip.de") << QSL("/tracking.js"));
    QTest::newRow("ignore1char") << QSL("/search.php?uid=*.*&src=")
                           << (QStringList() << QSL("/search.php?uid=") << QSL("&src="));
    QTest::newRow("ignoreDuplicates") << QSL("/search.*.dup.*.dup.*&src=")
                           << (QStringList() << QSL("/search.") << QSL(".dup.") << QSL("&src="));
    QTest::newRow("empty") << QString()
                           << (QStringList());
    QTest::newRow("justspaces") << QSL("       ")
                           << (QStringList() << QSL("       "));
    QTest::newRow("spacesWithMetachars") << QSL("   *    ?")
                           << (QStringList() << QSL("   ") << QSL("    ?"));
}

void AdBlockTest::parseRegExpFilterTest()
{
    AdBlockRule_Test rule_test;

    QFETCH(QString, parsedFilter);
    QFETCH(QStringList, result);

    QCOMPARE(rule_test.parseRegExpFilter(parsedFilter), result);
}

void AdBlockTest::ignoreEmptyLinesInSubscriptionTest()
{
    AdBlockSubscription subscription(QSL("test-subscription"));
    subscription.setFilePath(QSL(":autotests/data/adblock_empty_lines.txt"));
    subscription.loadSubscription({});

    QCOMPARE(subscription.allRules().count(), 3);
    QCOMPARE(subscription.allRules().at(0)->filter(), QSL("filter.com"));
    QCOMPARE(subscription.allRules().at(1)->filter(), QSL("test"));
    QCOMPARE(subscription.allRules().at(2)->isComment(), true);
}

QTEST_GUILESS_MAIN(AdBlockTest)
