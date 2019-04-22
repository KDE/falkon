/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
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
#include "htmlimporter.h"
#include "bookmarkitem.h"

#include <QUrl>
#include <QFileDialog>
#include <QRegularExpression>

#include <KLocalizedString>

HtmlImporter::HtmlImporter(QObject* parent)
    : BookmarksImporter(parent)
{
}

QString HtmlImporter::description() const
{
    return i18n("You can import bookmarks from any browser that supports HTML exporting. "
                                 "This file has usually these suffixes");
}

QString HtmlImporter::standardPath() const
{
    return QStringLiteral(".htm, .html");
}

QString HtmlImporter::getPath(QWidget* parent)
{
    const QString filter = i18n("HTML Bookmarks") + QLatin1String(" (*.htm *.html)");
    m_path = QFileDialog::getOpenFileName(parent, i18n("Choose file..."), QDir::homePath(), filter);
    return m_path;
}

bool HtmlImporter::prepareImport()
{
    m_file.setFileName(m_path);

    if (!m_file.open(QFile::ReadOnly)) {
        setError(i18n("Unable to open file."));
        return false;
    }

    return true;
}

static int qzMin(int a, int b)
{
    if (a > -1 && b > -1) {
        return qMin(a, b);
    }

    if (a > -1) {
        return a;
    }
    else {
        return b;
    }
}

BookmarkItem* HtmlImporter::importBookmarks()
{
    QString bookmarks = QString::fromUtf8(m_file.readAll());
    m_file.close();

    // Converting tags to lower case -,-
    // For some reason Qt::CaseInsensitive is not every time insensitive :-D

    bookmarks.replace(QLatin1String("<DL"), QLatin1String("<dl"));
    bookmarks.replace(QLatin1String("</DL"), QLatin1String("</dl"));
    bookmarks.replace(QLatin1String("<DT"), QLatin1String("<dt"));
    bookmarks.replace(QLatin1String("</DT"), QLatin1String("</dt"));
    bookmarks.replace(QLatin1String("<P"), QLatin1String("<p"));
    bookmarks.replace(QLatin1String("</P"), QLatin1String("</p"));
    bookmarks.replace(QLatin1String("<A"), QLatin1String("<a"));
    bookmarks.replace(QLatin1String("</A"), QLatin1String("</a"));
    bookmarks.replace(QLatin1String("HREF="), QLatin1String("href="));
    bookmarks.replace(QLatin1String("<H3"), QLatin1String("<h3"));
    bookmarks.replace(QLatin1String("</H3"), QLatin1String("</h3"));

    bookmarks = bookmarks.left(bookmarks.lastIndexOf(QLatin1String("</dl><p>")));
    int start = bookmarks.indexOf(QLatin1String("<dl><p>"));

    BookmarkItem* root = new BookmarkItem(BookmarkItem::Folder);
    root->setTitle(QStringLiteral("HTML Import"));

    QList<BookmarkItem*> folders;
    folders.append(root);

    while (start > 0) {
        QString string = bookmarks.mid(start);

        int posOfFolder = string.indexOf(QLatin1String("<dt><h3"));
        int posOfEndFolder = string.indexOf(QLatin1String("</dl><p>"));
        int posOfLink = string.indexOf(QLatin1String("<dt><a"));

        int nearest = qzMin(posOfLink, qzMin(posOfFolder, posOfEndFolder));
        if (nearest == -1) {
            break;
        }

        if (nearest == posOfFolder) {
            // Next is folder
            QRegularExpression rx(QSL("<dt><h3(.*)>(.*)</h3>"), QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption);
            QRegularExpressionMatch match = rx.match(string);
            QString folderName = match.captured(2).trimmed();

            BookmarkItem* folder = new BookmarkItem(BookmarkItem::Folder, folders.isEmpty() ? root : folders.last());
            folder->setTitle(folderName);
            folders.append(folder);

            start += posOfFolder + match.captured(0).size();
        }
        else if (nearest == posOfEndFolder) {
            // Next is end of folder
            if (!folders.isEmpty()) {
                folders.removeLast();
            }

            start += posOfEndFolder + 8;
        }
        else {
            // Next is link
            QRegularExpression rx(QSL("<dt><a(.*)>(.*)</a>"), QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption);
            QRegularExpressionMatch match = rx.match(string);

            QString arguments = match.captured(1);
            QString linkName = match.captured(2).trimmed();

            QRegularExpression rx2(QSL("href=\"(.*)\""), QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption);
            QRegularExpressionMatch match2 = rx2.match(arguments);

            QUrl url = QUrl::fromEncoded(match2.captured(1).trimmed().toUtf8());

            start += posOfLink + match.captured(0).size();

            if (url.isEmpty() || url.scheme() == QL1S("place") || url.scheme() == QL1S("about"))
                continue;

            BookmarkItem* b = new BookmarkItem(BookmarkItem::Url, folders.isEmpty() ? root : folders.last());
            b->setTitle(linkName.isEmpty() ? url.toString() : linkName);
            b->setUrl(url);
        }
    }

    return root;
}
