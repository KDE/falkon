/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2014-2018 David Rosca <nowrep@gmail.com>
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
#include "sqldatabase.h"

#include <QApplication>
#include <QThreadStorage>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

QThreadStorage<QSqlDatabase> s_databases;

Q_GLOBAL_STATIC(SqlDatabase, qz_sql_database)

// SqlQueryJob
SqlQueryJob::SqlQueryJob(QObject *parent)
    : QObject(parent)
{
}

SqlQueryJob::SqlQueryJob(const QString &query, QObject *parent)
    : QObject(parent)
{
    setQuery(query);
}

void SqlQueryJob::setQuery(const QString &query)
{
    m_query = query;
}

void SqlQueryJob::addBindValue(const QVariant &value)
{
    m_boundValues.append(value);
}

QSqlError SqlQueryJob::error() const
{
    return m_error;
}

QVariant SqlQueryJob::lastInsertId() const
{
    return m_lastInsertId;
}

int SqlQueryJob::numRowsAffected() const
{
    return m_numRowsAffected;
}

QVector<QSqlRecord> SqlQueryJob::records() const
{
    return m_records;
}

void SqlQueryJob::start()
{
    struct Result {
        QSqlError error;
        QVariant lastInsertId;
        int numRowsAffected;
        QVector<QSqlRecord> records;
    };

    const QString query = m_query;
    m_query.clear();
    const QVector<QVariant> boundValues = m_boundValues;
    m_boundValues.clear();

    auto watcher = new QFutureWatcher<Result>(this);
    connect(watcher, &QFutureWatcher<Result>::finished, this, [=]() {
        deleteLater();
        const auto result = watcher->result();
        m_error = result.error;
        m_lastInsertId = result.lastInsertId;
        m_numRowsAffected = result.numRowsAffected;
        m_records = result.records;
        Q_EMIT finished(this);
    });

    watcher->setFuture(QtConcurrent::run([=]() {
        QSqlQuery q(SqlDatabase::instance()->database());
        q.prepare(query);
        for (const QVariant &value : boundValues) {
            q.addBindValue(value);
        }
        q.exec();
        Result res;
        res.error = q.lastError();
        res.lastInsertId = q.lastInsertId();
        res.numRowsAffected = q.numRowsAffected();
        while (q.next()) {
            res.records.append(q.record());
        }
        return res;
    }));
}

// SqlDatabase
SqlDatabase::SqlDatabase(QObject* parent)
    : QObject(parent)
{
}

SqlDatabase::~SqlDatabase()
= default;

QSqlDatabase SqlDatabase::database()
{
    if (QThread::currentThread() == qApp->thread()) {
        return QSqlDatabase::database();
    }

    if (!s_databases.hasLocalData()) {
        const QString threadStr = QStringLiteral("Falkon/%1").arg((quintptr) QThread::currentThread());
        QSqlDatabase::removeDatabase(threadStr);
        QSqlDatabase db = QSqlDatabase::addDatabase(QSL("QSQLITE"), threadStr);
        db.setDatabaseName(m_databaseName);
        db.setConnectOptions(m_connectOptions);
        db.open();
        s_databases.setLocalData(db);
    }

    return s_databases.localData();
}

void SqlDatabase::setDatabase(const QSqlDatabase &database)
{
    m_databaseName = database.databaseName();
    m_connectOptions = database.connectOptions();
}

// instance
SqlDatabase* SqlDatabase::instance()
{
    return qz_sql_database();
}
