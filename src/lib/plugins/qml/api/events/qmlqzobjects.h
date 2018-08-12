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

#include "qzcommon.h"
#include <QObject>

/**
 * @brief The class exposing Qz ObjectName to QML
 */
class QmlQzObjects : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief The ObjectName enum
     */
    enum ObjectName {
        ON_WebView = Qz::ON_WebView,            //!< Represents object is webview
        ON_TabBar = Qz::ON_TabBar,              //!< Represents object is tabbar
        ON_TabWidget = Qz::ON_TabWidget,        //!< Represents object is tabwidget
        ON_BrowserWindow = Qz::ON_BrowserWindow //!< Represents object is browser window
    };
    Q_ENUM(ObjectName)

    explicit QmlQzObjects(QObject *parent = nullptr);
};
