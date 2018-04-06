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
#include "qztools.h"
#include "mainapplication.h"

#include <iostream>
#include <QLibrary>
#include <QtTest/QTest>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "pyfalkontestrunner [test.py]" << std::endl;
        return 1;
    }

    QzTools::removeRecursively(QDir::tempPath() + QSL("/Falkon-test"));
    MainApplication::setTestModeEnabled(true);
    MainApplication app(argc, argv);

#if defined(Q_OS_MACOS)
    const QString suffix = QSL("dylib");
#elif defined(Q_OS_UNIX)
    const QString suffix = QSL("so");
#elif defined(Q_OS_WIN)
    const QString suffix = QSL("dll");
#endif

    QLibrary library(QFINDTESTDATA("plugins/PyFalkon." + suffix));
    library.setLoadHints(QLibrary::ExportExternalSymbolsHint);
    if (!library.load()) {
        std::cerr << qPrintable(library.errorString()) << std::endl;
        return 2;
    }

    auto run_script = (bool(*)(const QByteArray&)) library.resolve("pyfalkon_run_script");
    if (!run_script) {
        return 3;
    }

    QFile file(app.arguments().at(1));
    if (!file.open(QFile::ReadOnly)) {
        std::cerr << "Failed to open " << qPrintable(file.fileName()) << " for reading" << std::endl;
        return 4;
    }

    if (!run_script(file.readAll())) {
        return 5;
    }

    return 0;
}
