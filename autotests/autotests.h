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
#include "mainapplication.h"
#include "qztools.h"

#include <QtTest/QtTest>

#define FALKONTEST_MAIN(Test) \
    int main(int argc, char **argv) \
    { \
        QzTools::removeRecursively(QDir::tempPath() + QSL("/Falkon-test")); \
        MainApplication::setTestModeEnabled(true); \
        MainApplication app(argc, argv); \
        QTEST_DISABLE_KEYPAD_NAVIGATION; \
        Test test; \
        return QTest::qExec(&test, argc, argv); \
    }

bool waitForLoadfinished(QObject *object)
{
    if (qstrcmp(object->metaObject()->className(), "WebTab") == 0) {
        QSignalSpy spy(object, SIGNAL(loadingChanged(bool)));
        while (spy.wait()) {
            if (spy.count() > 0 && spy.at(0).at(0).toBool()) {
                return true;
            }
            spy.clear();
        }
        return false;
    }
    qDebug() << "Unsupported object" << object;
    return false;
}
