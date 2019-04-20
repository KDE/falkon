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

/**
 * @brief The class exposing QWebEngineScriptCollection to QML
 */
class FALKON_EXPORT QmlUserScripts : public QObject
{
    Q_OBJECT
    /**
     * @brief Number of elements in the collection
     */
    Q_PROPERTY(int count READ count CONSTANT)
    /**
     * @brief Size of the collection
     */
    Q_PROPERTY(int size READ size CONSTANT)
    /**
     * @brief Checks if the collection is empty
     */
    Q_PROPERTY(bool empty READ empty CONSTANT)
public:
    explicit QmlUserScripts(QObject *parent = nullptr);
    /**
     * @brief Checks if the script is in collection
     * @param object of type QmlUserScript
     * @return true if the script is in collection, else false
     */
    Q_INVOKABLE bool contains(QObject *object) const;
    /**
     * @brief Finds a script in collection by name
     * @param name of the script
     * @return object of type QmlUserScript, representing the script of given name
     */
    Q_INVOKABLE QObject *findScript(const QString &name) const;
    /**
     * @brief Finds all scripts in collection by a given name
     * @return list of objects, each of type QmlUserScript, representing the script of given name
     */
    Q_INVOKABLE QList<QObject*> findScripts(const QString &name) const;
    /**
     * @brief Removes a script from collection
     * @param object of type QmlUserScript
     */
    Q_INVOKABLE void remove(QObject *object) const;
    /**
     * @brief Gets all the scripts of the collection
     * @return list of objects, each of type QmlUserScript
     */
    Q_INVOKABLE QList<QObject*> toList() const;

    void insert(QObject *object);
private:
    int count() const;
    int size() const;
    bool empty() const;

    QList<QObject*> toQObjectList(const QList<QWebEngineScript> &list) const;
};
