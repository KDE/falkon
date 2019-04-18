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

#include <QQmlEngine>
#include <QQmlComponent>

#include "qmlplugininterface.h"
#include "plugins.h"

class QmlEngine;

class QmlPluginLoader : public QObject
{
    Q_OBJECT
public:
    explicit QmlPluginLoader(const QString &name, const QString &path);
    void createComponent();
    QQmlComponent *component() const;
    QmlPluginInterface *instance() const;
private:
    QString m_path;
    QString m_name;
    QmlEngine *m_engine = nullptr;
    QQmlComponent *m_component = nullptr;
    QmlPluginInterface *m_interface = nullptr;

    void initEngineAndComponent();
};

Q_DECLARE_METATYPE(QmlPluginLoader *)
