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
#include "databasepasswordbackendtest.h"

#include <QtTest/QTest>
#include <QSqlDatabase>
#include <QSqlQuery>

void DatabasePasswordBackendTest::reloadBackend()
{
    delete m_backend;
    m_backend = new DatabasePasswordBackend;
}

void DatabasePasswordBackendTest::init()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QSL("QSQLITE"));
    db.setDatabaseName(QSL(":memory:"));
    db.open();

    db.exec(QSL("CREATE TABLE autofill (data TEXT, id INTEGER PRIMARY KEY, password TEXT,"
            "server TEXT, username TEXT, last_used NUMERIC)"));
}

void DatabasePasswordBackendTest::cleanup()
{
    QSqlDatabase::removeDatabase(QSqlDatabase::database().databaseName());
}

QTEST_GUILESS_MAIN(DatabasePasswordBackendTest)
