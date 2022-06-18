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
#ifndef CHROMEIMPORTER_H
#define CHROMEIMPORTER_H

#include <QFile>
#include <QVariantList>

#include "bookmarksimporter.h"

class FALKON_EXPORT ChromeImporter : public BookmarksImporter
{
public:
    explicit ChromeImporter(QObject* parent = nullptr);

    QString description() const override;
    QString standardPath() const override;

    QString getPath(QWidget* parent) override;
    bool prepareImport() override;

    BookmarkItem* importBookmarks() override;

private:
    void readBookmarks(const QVariantList &list, BookmarkItem* parent);

    QString m_path;
    QFile m_file;
};

#endif // CHROMEIMPORTER_H
