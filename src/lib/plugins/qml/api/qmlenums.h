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

/**
 * @brief The class exposing Enums to QML
 */
class QmlEnums : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief The WindowState enum
     */
    enum WindowState {
        FullScreen, //!< Represents full screen window
        Maximized,  //!< Represents maximized window
        Minimized,  //!< Represents minimized window
        Normal,     //!< Represents normal window
        Invalid     //!< Represents a invalid window
    };
    Q_ENUM(WindowState)

    /**
     * @brief The WindowType enum
     */
    enum WindowType {
        FirstAppWindow = Qz::BW_FirstAppWindow,          //!< Represents first app window
        MacFirstWindow = Qz::BW_MacFirstWindow,          //!< Represents first mac window
        NewWindow = Qz::BW_NewWindow,                    //!< Represents new window
        OtherRestoredWindow = Qz::BW_OtherRestoredWindow //!< Represents other restored window
    };
    Q_ENUM(WindowType)

    QmlEnums(QObject *parent = nullptr);
};
