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
#include "downloadkjob.h"

DownloadKJob::DownloadKJob(const QUrl &url, const QString &path, const QString &fileName, QObject *parent)
    : KJob(parent),
      m_url(url),
      m_path(path),
      m_fileName(fileName)
{
}

void DownloadKJob::start()
{
}

void DownloadKJob::updateDescription()
{
    emit description(this, tr("Downloading"),
                     qMakePair<QString, QString>(tr("Source"), m_url.toDisplayString()),
                     qMakePair<QString, QString>(tr("Destination"), QString("%1/%2").arg(m_path, m_fileName)));
}

void DownloadKJob::progress(double currSpeed, qint64 received, qint64 total)
{
    setProcessedAmount(Bytes, received);
    setTotalAmount(Bytes, total);
    emit speed(this, currSpeed);
}
