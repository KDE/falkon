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

// 3rdparty
#include "lineedit.h"
#include "squeezelabelv1.h"
#include "squeezelabelv2.h"
#include "qtsingleapplication/qtsingleapplication.h"

// app
#include "mainapplication.h"

// notifications
#include "desktopnotificationsfactory.h"

// plugins
#include "pluginproxy.h"
#include "plugininterface.h"

// popupwindow
#include "popupwebview.h"

// tools
#include "desktopfile.h"

// webengine
#include "webpage.h"
#include "webview.h"
#include "loadrequest.h"
#include "javascript/externaljsobject.h"

// webtab
#include "webtab.h"
#include "tabbedwebview.h"
