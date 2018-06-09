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
#pragma once

#include <QObject>
#include "qzcommon.h"

class QmlWindowType : public QObject
{
    Q_OBJECT
public:
    enum WindowType {
        FirstAppWindow = Qz::BW_FirstAppWindow,
        MacFirstWindow = Qz::BW_MacFirstWindow,
        NewWindow = Qz::BW_NewWindow,
        OtherRestoredWindow = Qz::BW_OtherRestoredWindow
    };
    Q_ENUMS(WindowType)

    QmlWindowType(QObject *parent = nullptr);
};
