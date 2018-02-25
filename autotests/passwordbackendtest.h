/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2018 David Rosca <nowrep@gmail.com>
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
#pragma once

#include <QObject>
#include <QVector>

#include "passwordbackends/passwordbackend.h"
#include "passwordmanager.h"

class PasswordBackendTest : public QObject
{
    Q_OBJECT

public:
    explicit PasswordBackendTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void storeTest();
    void removeAllTest();
    void updateLastUsedTest();

protected:
    virtual void reloadBackend() = 0;
    virtual void init() { }
    virtual void cleanup() { }

    PasswordBackend* m_backend;
    QVector<PasswordEntry> m_entries;
};
