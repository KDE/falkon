/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 Prasenjit Kumar Shaw <shawprasenjit07@gmail.com>
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

#include "syncutils.h"

#include <QString>
#include <QUrl>
#include <qzcommon.h>

QString getAudience(QUrl url)
{
    QString host = url.host();
    QString scheme = url.scheme();
    
    qint64 port = url.port(0);
    
    QString audience;
    if(port == 0) {
        audience = QSL("%s://%s").arg(scheme).arg(host);
    }
    else {
        audience = QSL("%s://%s:%s").arg(scheme).arg(host).arg(port);
    }
    return audience;
}
