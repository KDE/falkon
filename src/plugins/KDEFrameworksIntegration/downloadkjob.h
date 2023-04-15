/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 Javier Llorente <javier@opensuse.org>
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
#ifndef DOWNLOADKJOB_H
#define DOWNLOADKJOB_H

#include "qzcommon.h"
#include <KJob>
#include <QUrl>

class FALKON_EXPORT DownloadKJob : public KJob
{
    Q_OBJECT

public:
    DownloadKJob(const QUrl &url, const QString &path, const QString &fileName, QObject *parent = nullptr);
    void start();
    void updateDescription();

public slots:
    void progress(double currSpeed, qint64 received, qint64 total);
    
private:
    QUrl m_url;
    QString m_path;
    QString m_fileName;

};

#endif // DOWNLOADKJOB_H
