/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2014  David Rosca <nowrep@gmail.com>
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
#include "delayedfilewatcher.h"

#include <QTimer>

DelayedFileWatcher::DelayedFileWatcher(QObject* parent)
    : QFileSystemWatcher(parent)
{
    init();
}

DelayedFileWatcher::DelayedFileWatcher(const QStringList &paths, QObject* parent)
    : QFileSystemWatcher(paths, parent)
{
    init();
}

void DelayedFileWatcher::init()
{
    connect(this, &QFileSystemWatcher::directoryChanged, this, &DelayedFileWatcher::slotDirectoryChanged);
    connect(this, &QFileSystemWatcher::fileChanged, this, &DelayedFileWatcher::slotFileChanged);
}

void DelayedFileWatcher::slotDirectoryChanged(const QString &path)
{
    m_dirQueue.enqueue(path);
    QTimer::singleShot(500, this, &DelayedFileWatcher::dequeueDirectory);
}

void DelayedFileWatcher::slotFileChanged(const QString &path)
{
    m_fileQueue.enqueue(path);
    QTimer::singleShot(500, this, &DelayedFileWatcher::dequeueFile);
}

void DelayedFileWatcher::dequeueDirectory()
{
    Q_EMIT delayedDirectoryChanged(m_dirQueue.dequeue());
}

void DelayedFileWatcher::dequeueFile()
{
    Q_EMIT delayedFileChanged(m_fileQueue.dequeue());
}
