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
#include "sqldatabasetest.h"
#include "sqldatabase.h"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QSqlDatabase>
#include <QTemporaryFile>

void SqlDatabaseTest::initTestCase()
{
}

void SqlDatabaseTest::cleanupTestCase()
{
}

static bool waitForFinished(SqlQueryJob *job)
{
    QSignalSpy spy(job, &SqlQueryJob::finished);
    return spy.wait();
}

void SqlDatabaseTest::sqlQueryJobTest()
{
    QTemporaryFile file;
    file.open();

    QSqlDatabase db = QSqlDatabase::addDatabase(QSL("QSQLITE"));
    db.setDatabaseName(file.fileName());
    db.open();

    SqlDatabase::instance()->setDatabase(db);

    QCOMPARE(db.tables().count(), 0);

    auto *job = new SqlQueryJob();
    job->setQuery(QSL("CREATE TABLE test1 (data TEXT, id INTEGER PRIMARY KEY)"));
    job->start();
    QVERIFY(waitForFinished(job));
    QVERIFY(!job->error().isValid());

    QCOMPARE(db.tables(), QStringList{QSL("test1")});

    job = new SqlQueryJob();
    job->setQuery(QSL("INSERT INTO test1 (data) VALUES (?)"));
    job->addBindValue(QSL("test-value"));
    job->start();
    QVERIFY(waitForFinished(job));
    QVERIFY(!job->error().isValid());

    QCOMPARE(job->lastInsertId().toInt(), 1);
    QSqlQuery query(QSL("SELECT data FROM test1"), db);
    query.next();
    QCOMPARE(query.value(0).toString(), QSL("test-value"));
    QVERIFY(!query.next());

    job = new SqlQueryJob();
    job->setQuery(QSL("SELECT data FROM test1"));
    job->start();
    QVERIFY(waitForFinished(job));
    QVERIFY(!job->error().isValid());

    QCOMPARE(job->records().size(), 1);
    QCOMPARE(job->records().at(0).value(0).toString(), QSL("test-value"));

    job = new SqlQueryJob();
    job->setQuery(QSL("SELECT invalid sql syntax; 1321sdsa from"));
    job->start();
    QVERIFY(waitForFinished(job));
    QVERIFY(job->error().isValid());
}

QTEST_GUILESS_MAIN(SqlDatabaseTest)
