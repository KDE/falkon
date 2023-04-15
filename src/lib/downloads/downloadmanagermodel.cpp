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
#include "downloadmanagermodel.h"

DownloadManagerModel::DownloadManagerModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

int DownloadManagerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return count();
}

QVariant DownloadManagerModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        const DownloadItem *item = m_downloads.at(index.row());
        return item;
    }
    return QVariant();
}

void DownloadManagerModel::addDownload(DownloadItem *item)
{
    m_downloads.append(item);
    connect(item, &DownloadItem::deleteItem, this, &DownloadManagerModel::removeDownload);
    emit downloadAdded(item);
}

void DownloadManagerModel::removeDownload(DownloadItem *item)
{
    if (item && !item->isDownloading()) {
        delete item;
        emit downloadRemoved(item);
    }
}

int DownloadManagerModel::count() const
{
    return m_downloads.count();
}

DownloadItem *DownloadManagerModel::at(int index)
{
    return m_downloads.at(index);
}

