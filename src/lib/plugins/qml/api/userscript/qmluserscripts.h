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
#pragma once

#include "qmluserscript.h"
#include <QObject>

class QmlUserScripts : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(int size READ size CONSTANT)
    Q_PROPERTY(bool empty READ empty CONSTANT)
public:
    explicit QmlUserScripts(QObject *parent = nullptr);
    Q_INVOKABLE bool contains(QObject *object) const;
    Q_INVOKABLE QmlUserScript *findScript(const QString &name) const;
    Q_INVOKABLE QList<QObject *> findScripts(const QString &name) const;
    Q_INVOKABLE void insert(QObject *object) const;
    Q_INVOKABLE void insert(const QList<QObject*> &list) const;
    Q_INVOKABLE void remove(QObject *object) const;
    Q_INVOKABLE QList<QObject *> toList() const;
private:
    int count() const;
    int size() const;
    bool empty() const;

    QList<QObject *> toQObjectList(QList<QWebEngineScript> list) const;
};
