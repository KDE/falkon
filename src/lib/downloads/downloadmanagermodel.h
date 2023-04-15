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
#ifndef DOWNLOADMANAGERMODEL_H
#define DOWNLOADMANAGERMODEL_H

#include <QAbstractListModel>
#include "downloaditem.h"

class DownloadManagerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    DownloadManagerModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void addDownload(DownloadItem *item);
    void removeDownload(DownloadItem *item);
    int count() const;
    DownloadItem *at(int index);

private:
    QList<DownloadItem *> m_downloads;

signals:
    void downloadAdded(DownloadItem *item);
    void downloadRemoved(DownloadItem *item);
};

#endif // DOWNLOADMANAGERMODEL_H
