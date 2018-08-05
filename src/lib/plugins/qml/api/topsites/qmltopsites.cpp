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
#include "qmltopsites.h"
#include "speeddial.h"
#include "mainapplication.h"
#include "pluginproxy.h"

Q_GLOBAL_STATIC(QmlMostVisitedUrlData, mostVisitedUrlData)

QmlTopSites::QmlTopSites(QObject *parent)
    : QObject(parent)
{
}

QList<QObject*> QmlTopSites::get() const
{
    QList<SpeedDial::Page> pages = mApp->plugins()->speedDial()->pages();
    QList<QObject*> list;
    foreach(SpeedDial::Page page, pages) {
        auto mostVisitedUrl = mostVisitedUrlData->get(page.title, page.url);
        list.append(mostVisitedUrl);
    }
    return list;
}
