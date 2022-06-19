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
#include "databaseencryptedpasswordbackendtest.h"
#include "aesinterface.h"

#include <QtTest/QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>

void DatabaseEncryptedPasswordBackendTest::reloadBackend()
{
    delete m_backend;
    auto* backend = new DatabaseEncryptedPasswordBackend;

    if (m_testMasterPassword.isEmpty()) {
        m_testMasterPassword = AesInterface::passwordToHash(QString::fromUtf8(AesInterface::createRandomData(8)));
        backend->updateSampleData(m_testMasterPassword);
    }

    // a trick for setting masterPassword without gui interactions
    backend->isPasswordVerified(m_testMasterPassword);
    backend->setAskMasterPasswordState(false);

    m_backend = backend;
}

void DatabaseEncryptedPasswordBackendTest::init()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase(QSL("QSQLITE"));
        db.setDatabaseName(QSL(":memory:"));
    }
    db.open();
}

void DatabaseEncryptedPasswordBackendTest::cleanup()
{
    QSqlDatabase::removeDatabase(QSqlDatabase::database().databaseName());
}

QTEST_GUILESS_MAIN(DatabaseEncryptedPasswordBackendTest)
