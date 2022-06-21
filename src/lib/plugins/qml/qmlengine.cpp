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
#include "qmlengine.h"

QmlEngine::QmlEngine(QObject *parent)
    : QQmlEngine(parent)
{
}

QString QmlEngine::extensionName() const
{
    return m_extensionName;
}

void QmlEngine::setExtensionName(const QString &name)
{
    m_extensionName = name;
}

QString QmlEngine::extensionPath() const
{
    return m_extensionPath;
}

void QmlEngine::setExtensionPath(const QString &path)
{
    m_extensionPath = path;
}
