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
#include "qmluserscripts.h"
#include "mainapplication.h"
#include <QWebEngineProfile>
#include <QWebEngineScriptCollection>

QmlUserScripts::QmlUserScripts(QObject *parent)
    : QObject(parent)
{
}

QmlUserScripts::~QmlUserScripts()
{
    // remove scripts added by the plugin
    for (const QWebEngineScript &webEngineScript : m_webEngineScripts) {
        mApp->webProfile()->scripts()->remove(webEngineScript);
    }
}

int QmlUserScripts::count() const
{
    return mApp->webProfile()->scripts()->count();
}

int QmlUserScripts::size() const
{
    return mApp->webProfile()->scripts()->size();
}

bool QmlUserScripts::empty() const
{
    return mApp->webProfile()->scripts()->isEmpty();
}

QList<QObject *> QmlUserScripts::toQObjectList(QList<QWebEngineScript> list) const
{
    QList<QObject *> userScriptList;
    for (const QWebEngineScript &script : list) {
        QmlUserScript *userScript = new QmlUserScript();
        userScript->setWebEngineScript(script);
        userScriptList.append(userScript);
    }
    return userScriptList;
}

/**
 * @brief Checks if the script is in collection
 * @param object of type QmlUserScript
 * @return true if the the script in in collection, else false
 */
bool QmlUserScripts::contains(QObject *object) const
{
    QmlUserScript *userScript = qobject_cast<QmlUserScript*>(object);
    if (!userScript) {
        return false;
    }
    QWebEngineScript webEngineScript = userScript->webEngineScript();
    return mApp->webProfile()->scripts()->contains(webEngineScript);
}

/**
 * @brief Finds a script in collection by name
 * @param name of the script
 * @return object of type QmlUserScript, representing the script of given name
 */
QObject *QmlUserScripts::findScript(const QString &name) const
{
    QWebEngineScript webEngineScript = mApp->webProfile()->scripts()->findScript(name);
    QmlUserScript *qmlUserScript = new QmlUserScript();
    qmlUserScript->setWebEngineScript(webEngineScript);
    return qmlUserScript;
}

/**
 * @brief Finds all scripts in collection by a given name
 * @return list of objects, each of type QmlUserScript, representing the script of given name
 */
QList<QObject*> QmlUserScripts::findScripts(const QString &name) const
{
    QList<QWebEngineScript> list = mApp->webProfile()->scripts()->findScripts(name);
    return toQObjectList(list);
}

/**
 * @brief Inserts a script into collection
 * @param object of type QmlUserScript
 */
void QmlUserScripts::insert(QObject *object)
{
    QmlUserScript *userScript = qobject_cast<QmlUserScript*>(object);
    if (!userScript) {
        return;
    }
    QWebEngineScript webEngineScript = userScript->webEngineScript();
    mApp->webProfile()->scripts()->insert(webEngineScript);
    m_webEngineScripts.append(webEngineScript);
}

/**
 * @brief Inserts a list of scripts into collection
 * @param list of objects, each of type QmlUserScript
 */
void QmlUserScripts::insert(const QList<QObject *> &list)
{
    for (QObject *object : list) {
        QmlUserScript *userScript = qobject_cast<QmlUserScript*>(object);
        if (!userScript) {
            continue;
        }
        QWebEngineScript webEngineScript = userScript->webEngineScript();
        mApp->webProfile()->scripts()->insert(webEngineScript);
        m_webEngineScripts.append(webEngineScript);
    }
}

/**
 * @brief Removes a script from collection
 * @param object of type QmlUserScript
 */
void QmlUserScripts::remove(QObject *object) const
{
    QmlUserScript *userScript = qobject_cast<QmlUserScript*>(object);
    if (!userScript) {
        return;
    }
    QWebEngineScript webEngineScript = userScript->webEngineScript();
    mApp->webProfile()->scripts()->remove(webEngineScript);
}

/**
 * @brief Gets all the scripts of the collection
 * @return list of objects, each of type QmlUserScript
 */
QList<QObject *> QmlUserScripts::toList() const
{
    QList<QWebEngineScript> list = mApp->webProfile()->scripts()->toList();
    return toQObjectList(list);
}
