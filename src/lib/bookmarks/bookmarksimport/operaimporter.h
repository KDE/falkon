/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#ifndef OPERAIMPORTER_H
#define OPERAIMPORTER_H

#include <QFile>
#include <QTextStream>

#include "bookmarksimporter.h"

class FALKON_EXPORT OperaImporter : public BookmarksImporter
{
public:
    explicit OperaImporter(QObject* parent = nullptr);

    QString description() const override;
    QString standardPath() const override;

    QString getPath(QWidget* parent) override;
    bool prepareImport() override;

    BookmarkItem* importBookmarks() override;

private:
    enum Token { EmptyLine,
                 StartFolder,
                 EndFolder,
                 StartUrl,
                 StartSeparator,
                 StartDeleted,
                 KeyValuePair,
                 Invalid
               };

    Token parseLine(const QString &line);
    QString m_key;
    QString m_value;

    QString m_path;
    QFile m_file;
    QTextStream m_stream;
};

#endif // OPERAIMPORTER_H
