/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmlfileutils.h"
#include "datapaths.h"
#include "qztools.h"
#include <QFile>
#include <QDebug>

QmlFileUtils::QmlFileUtils(QString filePath, QObject *parent)
    : QObject(parent)
{
    // Get the plugin root directory - of the form
    // some-path-in-plugin-paths/qml/plugin-directory
    const QStringList dirs = DataPaths::allPaths(DataPaths::Plugins);
    for (QString path : dirs) {
        path.append(QSL("/qml/"));
        if (filePath.contains(path)) {
            m_path = path;
            QString pluginDirName = filePath.remove(path).split(QSL("/"))[0];
            m_path.append(pluginDirName);
            m_path.append(QSL("/"));
            break;
        }
    }
}

QString QmlFileUtils::resolve(const QString &filePath)
{
    QUrl resolvedUrl = QUrl::fromLocalFile(m_path).resolved(QUrl::fromEncoded(filePath.toUtf8()));
    QString resolvedPath = resolvedUrl.toLocalFile();
    if (resolvedPath.contains(m_path)) {
        return resolvedPath;
    }
    return QString();
}

QString QmlFileUtils::readAllFileContents(const QString &fileName)
{
    QString path = resolve(fileName);
    return QzTools::readAllFileContents(path);
}

bool QmlFileUtils::exists(const QString &filePath)
{
    QString path = resolve(filePath);
    return QFile(path).exists();
}
