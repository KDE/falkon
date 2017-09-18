/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2014-2017 David Rosca <nowrep@gmail.com>
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
#include "datapaths.h"
#include "qztools.h"

#include <QApplication>
#include <QDir>

#include <QStandardPaths>
#include <QTemporaryDir>

Q_GLOBAL_STATIC(DataPaths, qz_data_paths)

DataPaths::DataPaths()
{
    init();
}

// static
void DataPaths::setCurrentProfilePath(const QString &profilePath)
{
    qz_data_paths()->initCurrentProfile(profilePath);
}

// static
void DataPaths::setPortableVersion()
{
    DataPaths* d = qz_data_paths();
    d->m_paths[Config] = d->m_paths[AppData];

    d->m_paths[Profiles] = d->m_paths[Config];
    d->m_paths[Profiles].first().append(QLatin1String("/profiles"));

    d->m_paths[Temp] = d->m_paths[Config];
    d->m_paths[Temp].first().append(QLatin1String("/tmp"));

    // Make sure the Config and Temp paths exists
    QDir dir;
    dir.mkpath(d->m_paths[Config].at(0));
    dir.mkpath(d->m_paths[Temp].at(0));
}

// static
QString DataPaths::path(DataPaths::Path path)
{
    Q_ASSERT(!qz_data_paths()->m_paths[path].isEmpty());

    return qz_data_paths()->m_paths[path].at(0);
}

// static
QStringList DataPaths::allPaths(DataPaths::Path type)
{
    Q_ASSERT(!qz_data_paths()->m_paths[type].isEmpty());

    return qz_data_paths()->m_paths[type];
}

// static
QString DataPaths::currentProfilePath()
{
    return path(CurrentProfile);
}

// static
void DataPaths::clearTempData()
{
    QzTools::removeDir(path(Temp));
}

void DataPaths::init()
{
#if defined(NO_SYSTEM_DATAPATH)
    m_paths[AppData].append(QApplication::applicationDirPath());
#endif
    m_paths[AppData].append(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation));

    for (const QString &location : qAsConst(m_paths[AppData])) {
        initAssetsIn(location);
    }

    m_paths[Config].append(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    m_paths[Profiles].append(m_paths[Config].at(0) + QLatin1String("/profiles"));
    // We also allow to load data from Config path
    initAssetsIn(m_paths[Config].at(0));

    m_tmpdir.reset(new QTemporaryDir(QCoreApplication::applicationName()));
    m_paths[Temp].append(m_tmpdir->path());
    if (!m_tmpdir->isValid()) {
        qWarning() << "Failed to create temporary directory" << m_tmpdir->path();
    }

    m_paths[Cache].append(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    // Make sure the Config and Temp paths exists
    QDir dir;
    dir.mkpath(m_paths[Config].at(0));
    dir.mkpath(m_paths[Temp].at(0));

#ifdef USE_LIBPATH
    m_paths[Plugins].append(QLatin1String(USE_LIBPATH "/falkon"));
#endif
}

void DataPaths::initCurrentProfile(const QString &profilePath)
{
    m_paths[CurrentProfile].append(profilePath);

    if (m_paths[Cache].isEmpty())
        m_paths[Cache].append(m_paths[CurrentProfile].at(0) + QLatin1String("/cache"));

    if (m_paths[Sessions].isEmpty())
        m_paths[Sessions].append(m_paths[CurrentProfile].at(0) + QLatin1String("/sessions"));

    QDir dir;
    dir.mkpath(m_paths[Cache].at(0));
    dir.mkpath(m_paths[Sessions].at(0));
}

void DataPaths::initAssetsIn(const QString &path)
{
    m_paths[Translations].append(path + QLatin1String("/locale"));
    m_paths[Themes].append(path + QLatin1String("/themes"));
    m_paths[Plugins].append(path + QLatin1String("/plugins"));
}
