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
#pragma once

#include <QSettings>

#include "qzcommon.h"

class FALKON_EXPORT DesktopFile
{
public:
    explicit DesktopFile(const QString &filePath);

    QString name(const QString &locale = QString()) const;
    QString comment(const QString &locale = QString()) const;
    QString type() const;
    QString icon() const;
    QVariant value(const QString &key, const QString &locale = QString()) const;

private:
    QSettings m_settings;
};
