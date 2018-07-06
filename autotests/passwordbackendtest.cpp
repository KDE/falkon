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
#include "passwordbackendtest.h"

#include <QtTest/QtTest>
#include <QDebug>

#ifdef Q_OS_WIN
#include "qt_windows.h"
#else
#include "unistd.h"
#endif

static bool compareEntries(const PasswordEntry &value, const PasswordEntry &ref)
{
    if (ref.host != value.host) {
        qDebug() << "Host mismatch. Value =" << value.host << "Reference =" << ref.host;
        return false;
    }

    if (ref.username != value.username) {
        qDebug() << "Username mismatch. Value =" << value.username << "Reference =" << ref.username;
        return false;
    }

    if (ref.password != value.password) {
        qDebug() << "Password mismatch. Value =" << value.password << "Reference =" << ref.password;
        return false;
    }

    if (ref.data != value.data) {
        qDebug() << "Data mismatch. Value =" << value.data << "Reference =" << ref.data;
        return false;
    }

    return true;
}

PasswordBackendTest::PasswordBackendTest()
    : QObject()
    , m_backend(nullptr)
{
}

void PasswordBackendTest::initTestCase()
{
    init();

    // Backup entries
    reloadBackend();
    m_entries = m_backend->getAllEntries();
    m_backend->removeAll();
}

void PasswordBackendTest::cleanupTestCase()
{
    cleanup();

    reloadBackend();
    foreach (const PasswordEntry &entry, m_entries) {
        m_backend->addEntry(entry);
    }
}

void PasswordBackendTest::storeTest()
{
    reloadBackend();

    /* Basic password entry */
    PasswordEntry entry;
    entry.host = QSL("org.falkon.google.com");
    entry.username = QSL("user1");
    entry.password = QSL("pass1");
    entry.data = "entry1-data=23&username=user1&password=pass1";

    m_backend->addEntry(entry);

    // Check entry that may be stored in cache
    PasswordEntry stored = m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).constFirst();
    QVERIFY(compareEntries(stored, entry) == true);

    reloadBackend();

    // Check entry retrieved from backend engine
    QVERIFY(!m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).isEmpty());
    stored = m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).constFirst();
    QVERIFY(compareEntries(stored, entry) == true);


    /* UTF-8 password entry */
    PasswordEntry entry2;
    entry2.host = QSL("org.falkon.falkon.com");
    entry2.username = QString::fromUtf8("+ě ++ éí§`]|~đ11 +!:");
    entry2.password = QString::fromUtf8("+ěš asn~đ°#&# |€");
    entry2.data = "use%C2%B6+_nam%C4%8D=%2B%C4%9B+%2B%2B+%C3%A9%C3%AD%C2%A7%60%5D%7C%7E%C4%9111+%2B%21%3A"
            "&pA+%5DsQ+%2Bword=%2B%C4%9B%C5%A1+asn%7E%C4%91%C2%B0%23%26%23+%7C%E2%82%AC";

    m_backend->addEntry(entry2);

    // Check entry that may be stored in cache
    PasswordEntry stored2 = m_backend->getEntries(QUrl(QSL("org.falkon.falkon.com"))).constFirst();
    QVERIFY(compareEntries(stored2, entry2) == true);

    reloadBackend();

    // Check entry retrieved from backend engine
    stored2 = m_backend->getEntries(QUrl(QSL("org.falkon.falkon.com"))).constFirst();
    QVERIFY(compareEntries(stored2, entry2) == true);

    /* Cleanup */
    // Local cleanup
    m_backend->removeEntry(stored);
    QCOMPARE(m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).count(), 0);

    m_backend->removeEntry(stored2);
    QCOMPARE(m_backend->getEntries(QUrl(QSL("org.falkon.falkon.com"))).count(), 0);

    reloadBackend();

    // Backend engine cleanup
    QCOMPARE(m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).count(), 0);
    QCOMPARE(m_backend->getEntries(QUrl(QSL("org.falkon.falkon.com"))).count(), 0);
}

void PasswordBackendTest::removeAllTest()
{
    reloadBackend();

    PasswordEntry entry;
    entry.host = QSL("org.falkon.google.com");
    entry.username = QSL("user1");
    entry.password = QSL("pass1");
    entry.data = "entry1-data=23&username=user1&password=pass1";
    m_backend->addEntry(entry);

    entry.username.append(QSL("s"));
    m_backend->addEntry(entry);

    entry.username.append(QSL("s"));
    m_backend->addEntry(entry);

    entry.username.append(QSL("s"));
    m_backend->addEntry(entry);

    entry.username.append(QSL("s"));
    m_backend->addEntry(entry);

    entry.username.append(QSL("s"));
    m_backend->addEntry(entry);

    entry.username.append(QSL("s"));
    m_backend->addEntry(entry);

    QCOMPARE(m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).count(), 7);
    reloadBackend();
    QCOMPARE(m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).count(), 7);

    m_backend->removeAll();

    QCOMPARE(m_backend->getAllEntries().count(), 0);
    reloadBackend();
    QCOMPARE(m_backend->getAllEntries().count(), 0);
}

void PasswordBackendTest::updateLastUsedTest()
{
    reloadBackend();

    PasswordEntry entry;
    entry.host = QSL("org.falkon.google.com");
    entry.username = QSL("user1");
    entry.password = QSL("pass1");
    entry.data = "entry1-data=23&username=user1&password=pass1";
    m_backend->addEntry(entry);

#ifdef Q_OS_WIN
    Sleep(1000);
#else
    sleep(1);
#endif

    entry.username.append(QSL("s"));
    m_backend->addEntry(entry);

    QVERIFY(!m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).isEmpty());
    QVERIFY(compareEntries(entry, m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).constFirst()));
    reloadBackend();
    QVERIFY(!m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).isEmpty());
    QVERIFY(compareEntries(entry, m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).constFirst()));

    m_backend->removeEntry(m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).constFirst());
    m_backend->removeEntry(m_backend->getEntries(QUrl(QSL("org.falkon.google.com"))).constFirst());

    QCOMPARE(m_backend->getAllEntries().count(), 0);
    reloadBackend();
    QCOMPARE(m_backend->getAllEntries().count(), 0);
}
