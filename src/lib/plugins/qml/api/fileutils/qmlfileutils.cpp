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
#include <QQmlEngine>

QmlFileUtils::QmlFileUtils(const QString &filePath, QObject *parent)
    : QObject(parent)
{
    m_path = filePath;
}

QString QmlFileUtils::resolve(const QString &filePath)
{
    QString resolvedPath = m_path + QL1C('/') + filePath;
    resolvedPath = QDir::cleanPath(resolvedPath);
    if (resolvedPath.contains(m_path)) {
        return resolvedPath;
    }
    return {};
}

QByteArray QmlFileUtils::readAllFileContents(const QString &fileName)
{
    const QString path = resolve(fileName);
    return QzTools::readAllFileByteContents(path);
}

bool QmlFileUtils::exists(const QString &filePath)
{
    const QString path = resolve(filePath);
    return QFile(path).exists();
}
