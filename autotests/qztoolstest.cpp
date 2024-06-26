/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2014  David Rosca <nowrep@gmail.com>
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
#include "qztoolstest.h"
#include "qztools.h"

#include <QDir>
#include <QtTest/QtTest>

void QzToolsTest::initTestCase()
{
    m_tmpPath = QDir::tempPath() + QL1S("/falkon-test/qztoolstest");
    QDir().mkpath(m_tmpPath);

    QVERIFY(QDir(m_tmpPath).exists());
}

void QzToolsTest::cleanupTestCase()
{
    QDir().rmpath(m_tmpPath);

    QVERIFY(!QDir(m_tmpPath).exists());
}

void QzToolsTest::samePartOfStrings_data()
{
    QTest::addColumn<QString>("string1");
    QTest::addColumn<QString>("string2");
    QTest::addColumn<QString>("result");

    // Lorem ipsum dolor sit amet, consectetur adipiscing elit.
    QTest::newRow("General") << "Lorem ipsum dolor" << "Lorem ipsum dolor Test_1" << "Lorem ipsum dolor";
    QTest::newRow("OneChar") << "L" << "LTest_1" << "L";
    QTest::newRow("EmptyReturn") << "Lorem ipsum dolor" << "orem ipsum dolor Test_1" << "";
    QTest::newRow("EmptyString1") << "" << "orem ipsum dolor Test_1" << "";
    QTest::newRow("EmptyString2") << "Lorem ipsum dolor" << "" << "";
    QTest::newRow("EmptyBoth") << "" << "" << "";
}

void QzToolsTest::samePartOfStrings()
{
    QFETCH(QString, string1);
    QFETCH(QString, string2);
    QFETCH(QString, result);

    QCOMPARE(QzTools::samePartOfStrings(string1, string2), result);
}

void QzToolsTest::getFileNameFromUrl_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("result");

    QTest::newRow("Basic") << QUrl(QSL("http://www.google.com/filename.html")) << QSL("filename.html");
    QTest::newRow("OnlyHost") << QUrl(QSL("http://www.google.com/")) << QSL("www.google.com");
    QTest::newRow("OnlyHostWithoutSlash") << QUrl(QSL("http://www.google.com")) << QSL("www.google.com");
    QTest::newRow("EndingDirectory") << QUrl(QSL("http://www.google.com/filename/")) << QSL("filename");
    QTest::newRow("EmptyUrl") << QUrl(QSL("")) << QSL("");
    QTest::newRow("OnlyScheme") << QUrl(QSL("http:")) << QSL("");
    QTest::newRow("FileSchemeUrl") << QUrl(QSL("file:///usr/share/test/file.tx")) << QSL("file.tx");
    QTest::newRow("FileSchemeUrlDirectory") << QUrl(QSL("file:///usr/share/test/")) << QSL("test");
    QTest::newRow("FileSchemeUrlRoot") << QUrl(QSL("file:///")) << QSL("");
}

void QzToolsTest::getFileNameFromUrl()
{
    QFETCH(QUrl, url);
    QFETCH(QString, result);

    QCOMPARE(QzTools::getFileNameFromUrl(url), result);
}

void QzToolsTest::splitCommandArguments_data()
{
    QTest::addColumn<QString>("command");
    QTest::addColumn<QStringList>("result");

    QTest::newRow("Basic") << "/usr/bin/foo -o foo.out"
                           << (QStringList() << QSL("/usr/bin/foo") << QSL("-o") << QSL("foo.out"));
    QTest::newRow("Empty") << QString()
                           << QStringList();
    QTest::newRow("OnlySpaces") << QSL("                   ")
                           << QStringList();
    QTest::newRow("OnlyQuotes") << QSL(R"("" "")")
                           << QStringList();
    QTest::newRow("EmptyQuotesAndSpace") << QSL(R"("" "" " ")")
                           << QStringList(QSL(" "));
    QTest::newRow("MultipleSpaces") << "    /usr/foo   -o    foo.out    "
                           << (QStringList() << QSL("/usr/foo") << QSL("-o") << QSL("foo.out"));
    QTest::newRow("Quotes") << R"("/usr/foo" "-o" "foo.out")"
                           << (QStringList() << QSL("/usr/foo") << QSL("-o") << QSL("foo.out"));
    QTest::newRow("SingleQuotes") << "'/usr/foo' '-o' 'foo.out'"
                           << (QStringList() << QSL("/usr/foo") << QSL("-o") << QSL("foo.out"));
    QTest::newRow("SingleAndDoubleQuotes") << " '/usr/foo' \"-o\" 'foo.out' "
                           << (QStringList() << QSL("/usr/foo") << QSL("-o") << QSL("foo.out"));
    QTest::newRow("SingleInDoubleQuotes") << "/usr/foo \"-o 'ds' \" 'foo.out' "
                           << (QStringList() << QSL("/usr/foo") << QSL("-o 'ds' ") << QSL("foo.out"));
    QTest::newRow("DoubleInSingleQuotes") << "/usr/foo -o 'foo\" d \".out' "
                           << (QStringList() << QSL("/usr/foo") << QSL("-o") << QSL("foo\" d \".out"));
    QTest::newRow("SpacesWithQuotes") << QSL(R"(  "   "     "   "     )")
                           << (QStringList() << QSL("   ") << QSL("   "));
    QTest::newRow("QuotesAndSpaces") << "/usr/foo -o \"foo - out\""
                           << (QStringList() << QSL("/usr/foo") << QSL("-o") << QSL("foo - out"));
    QTest::newRow("EqualAndQuotes") << "/usr/foo -o=\"foo - out\""
                           << (QStringList() << QSL("/usr/foo") << QSL("-o=foo - out"));
    QTest::newRow("EqualWithSpaces") << "/usr/foo -o = \"foo - out\""
                           << (QStringList() << QSL("/usr/foo") << QSL("-o") << QSL("=") << QSL("foo - out"));
    QTest::newRow("MultipleSpacesAndQuotes") << "    /usr/foo   -o=\"    foo.out   \" "
                           << (QStringList() << QSL("/usr/foo") << QSL("-o=    foo.out   "));
    // Unmatched quotes should be treated as an error
    QTest::newRow("UnmatchedQuote") << "/usr/bin/foo -o \"bar"
                           << QStringList();
}

void QzToolsTest::splitCommandArguments()
{
    QFETCH(QString, command);
    QFETCH(QStringList, result);

    QCOMPARE(QzTools::splitCommandArguments(command), result);
}

void QzToolsTest::escapeSqlGlobString_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("result");

    QTest::newRow("NothingToEscape") << "http://test" << "http://test";
    QTest::newRow("Escape *") << "http://test*/heh" << "http://test[*]/heh";
    QTest::newRow("Escape **") << "http://test**/he*h" << "http://test[*][*]/he[*]h";
    QTest::newRow("Escape ?") << "http://test?/heh" << "http://test[?]/heh";
    QTest::newRow("Escape ??") << "http://t??est?/heh" << "http://t[?][?]est[?]/heh";
    QTest::newRow("Escape [") << "http://[test/heh" << "http://[[]test/heh";
    QTest::newRow("Escape [[") << "http://[[te[st/heh" << "http://[[][[]te[[]st/heh";
    QTest::newRow("Escape ]") << "http://]test/heh" << "http://[]]test/heh";
    QTest::newRow("Escape ]]") << "http://]]te]st/heh" << "http://[]][]]te[]]st/heh";
    QTest::newRow("Escape []") << "http://[]test/heh" << "http://[[][]]test/heh";
    QTest::newRow("Escape [][[]][]") << "http://t[][[]][]est/heh" << "http://t[[][]][[][[][]][]][[][]]est/heh";
    QTest::newRow("Escape [?]][[*]") << "http://t[?]][[*]est/heh" << "http://t[[][?][]][]][[][[][*][]]est/heh";
}

void QzToolsTest::escapeSqlGlobString()
{
    QFETCH(QString, input);
    QFETCH(QString, result);

    QCOMPARE(QzTools::escapeSqlGlobString(input), result);
}

class TempFile
{
    QString name;

public:
    explicit TempFile(const QString &name)
        : name(name)
    {
        QFile file(name);
        file.open(QFile::WriteOnly);
        file.write(QByteArrayLiteral("falkon-test"));
        file.close();
    }

    ~TempFile()
    {
        QFile::remove(name);
    }
};

void QzToolsTest::ensureUniqueFilename()
{
    QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out")), createPath("test.out"));
    QCOMPARE(QzTools::ensureUniqueFilename(createPath("test")), createPath("test"));

    // default appendFormat = (%1)
    {
        TempFile f1(createPath("test.out"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out")), createPath("test(1).out"));
        TempFile f2(createPath("test(1).out"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out")), createPath("test(2).out"));
        TempFile f3(createPath("test(2).out"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out")), createPath("test(3).out"));
    }
    {
        TempFile f1(createPath("test"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test")), createPath("test(1)"));
        TempFile f2(createPath("test(1)"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test")), createPath("test(2)"));
        TempFile f3(createPath("test(2)"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test")), createPath("test(3)"));
    }
    {
        TempFile f1(createPath("test(1)"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test")), createPath("test"));
        TempFile f2(createPath("test"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test")), createPath("test(2)"));
    }

    // appendFormat = %1
    {
        QString appendFormat = QSL("%1");

        TempFile f1(createPath("test.out"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out"), appendFormat), createPath("test1.out"));
        TempFile f2(createPath("test1.out"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out"), appendFormat), createPath("test2.out"));
        TempFile f3(createPath("test2.out"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out"), appendFormat), createPath("test3.out"));
    }
    {
        QString appendFormat = QSL("%1");

        TempFile f1(createPath("test"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test1"));
        TempFile f2(createPath("test1"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test2"));
        TempFile f3(createPath("test2"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test3"));
    }
    {
        QString appendFormat = QSL("%1");

        TempFile f1(createPath("test1"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test"));
        TempFile f2(createPath("test"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test2"));
    }

    // appendFormat = .%1
    {
        QString appendFormat = QSL(".%1");

        TempFile f1(createPath("test.out"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out"), appendFormat), createPath("test.1.out"));
        TempFile f2(createPath("test.1.out"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out"), appendFormat), createPath("test.2.out"));
        TempFile f3(createPath("test.2.out"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test.out"), appendFormat), createPath("test.3.out"));
    }
    {
        QString appendFormat = QSL(".%1");

        TempFile f1(createPath("test"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test.1"));
        TempFile f2(createPath("test.1"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test.2"));
        TempFile f3(createPath("test.2"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test.3"));
    }
    {
        QString appendFormat = QSL(".%1");

        TempFile f1(createPath("test.1"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test"));
        TempFile f2(createPath("test"));
        QCOMPARE(QzTools::ensureUniqueFilename(createPath("test"), appendFormat), createPath("test.2"));
    }
}

static void createTestDirectoryStructure(const QString &path)
{
    QDir().mkdir(path);
    QDir dir(path);
    dir.mkdir(QSL("dir1"));
    dir.mkdir(QSL("dir2"));
    dir.mkdir(QSL("dir3"));
    dir.cd(QSL("dir1"));
    dir.mkdir(QSL("dir1_1"));
    dir.mkdir(QSL("dir1_2"));
    dir.mkdir(QSL("dir1_3"));
    dir.cdUp();
    dir.cd(QSL("dir3"));
    dir.mkdir(QSL("dir3_1"));
    QFile file(path + QSL("/dir1/dir1_2/file1.txt"));
    file.open(QFile::WriteOnly);
    file.write("test");
    file.close();
}

void QzToolsTest::copyRecursivelyTest()
{
    const QString testDir = createPath("copyRecursivelyTest");
    createTestDirectoryStructure(testDir);

    QVERIFY(!QFileInfo::exists(testDir + QSL("-copy")));

    // Copy to non-existent target
    QCOMPARE(QzTools::copyRecursively(testDir, testDir + QSL("-copy")), true);

    QCOMPARE(QFileInfo(testDir + QSL("-copy")).isDir(), true);
    QCOMPARE(QFileInfo(testDir + QSL("-copy/dir1")).isDir(), true);
    QCOMPARE(QFileInfo(testDir + QSL("-copy/dir2")).isDir(), true);
    QCOMPARE(QFileInfo(testDir + QSL("-copy/dir3")).isDir(), true);
    QCOMPARE(QFileInfo(testDir + QSL("-copy/dir1/dir1_1")).isDir(), true);
    QCOMPARE(QFileInfo(testDir + QSL("-copy/dir1/dir1_2")).isDir(), true);
    QCOMPARE(QFileInfo(testDir + QSL("-copy/dir1/dir1_3")).isDir(), true);
    QCOMPARE(QFileInfo(testDir + QSL("-copy/dir3/dir3_1")).isDir(), true);
    QCOMPARE(QFileInfo(testDir + QSL("-copy/dir1/dir1_2/file1.txt")).isFile(), true);

    QFile file(testDir + QSL("-copy/dir1/dir1_2/file1.txt"));
    file.open(QFile::ReadOnly);
    QCOMPARE(file.readAll(), QByteArray("test"));
    file.close();

    // Copy to target that already exists
    QCOMPARE(QzTools::copyRecursively(testDir, testDir + QSL("-copy")), false);

    // Cleanup
    QCOMPARE(QzTools::removeRecursively(testDir), true);
    QCOMPARE(QzTools::removeRecursively(testDir + QSL("-copy")), true);
}

void QzToolsTest::removeRecursivelyTest()
{
    const QString testDir = createPath("removeRecursivelyTest");
    createTestDirectoryStructure(testDir);

    QCOMPARE(QzTools::copyRecursively(testDir, testDir + QSL("-copy")), true);
    QCOMPARE(QzTools::removeRecursively(testDir + QSL("-copy")), true);
    QCOMPARE(QFileInfo::exists(testDir + QSL("-copy")), false);

    // Remove non-existent path returns success
    QCOMPARE(QzTools::removeRecursively(testDir + QSL("-copy")), true);

    QCOMPARE(QzTools::copyRecursively(testDir, testDir + QSL("-copy2")), true);

    QFile dir(testDir + QSL("-copy2"));
    dir.setPermissions(dir.permissions() & ~(QFile::WriteOwner | QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther));

    QCOMPARE(QzTools::removeRecursively(testDir + QSL("-copy2")), false);

    dir.setPermissions(dir.permissions() | QFile::WriteOwner);

    QCOMPARE(QzTools::removeRecursively(testDir + QSL("-copy2")), true);

    // Cleanup
    QCOMPARE(QzTools::removeRecursively(testDir), true);
}

void QzToolsTest::dontFollowSymlinksTest()
{
    const QString testDir = createPath("removeRecursivelyTest");
    createTestDirectoryStructure(testDir);

    QDir().mkpath(testDir + QSL("/subdir"));
    QFile::link(testDir, testDir + QSL("/subdir/link"));

    QVERIFY(QzTools::removeRecursively(testDir + QSL("/subdir")));

    QVERIFY(!QFile::exists(testDir + QSL("/subdir")));
    QVERIFY(QFile::exists(testDir));

    QDir().mkpath(testDir + QSL("/subdir/normalfolder"));
    QFile::link(QSL(".."), testDir + QSL("/subdir/link"));

    QVERIFY(QzTools::copyRecursively(testDir + QSL("/subdir"), testDir + QSL("/subdir2")));

    QCOMPARE(QFile::exists(testDir + QSL("/subdir2/link")), true);
    QCOMPARE(QFile::exists(testDir + QSL("/subdir2/normalfolder")), true);

    // Cleanup
    QCOMPARE(QzTools::removeRecursively(testDir), true);
}

QString QzToolsTest::createPath(const char *file) const
{
    return m_tmpPath + QL1S("/") + QString::fromUtf8(file);
}

QTEST_GUILESS_MAIN(QzToolsTest)
