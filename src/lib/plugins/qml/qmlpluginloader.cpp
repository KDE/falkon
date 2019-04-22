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
#include "qmlpluginloader.h"
#include "qmlengine.h"
#include "../config.h"

#include <QDir>
#include <QQmlContext>

#include <KLocalizedContext>

QmlPluginLoader::QmlPluginLoader(const QString &name, const QString &path)
{
    m_name = name;
    m_path = path;
    initEngineAndComponent();
}

void QmlPluginLoader::createComponent()
{
    m_interface = qobject_cast<QmlPluginInterface*>(m_component->create(m_component->creationContext()));

    if (!m_interface) {
        return;
    }

    m_interface->setEngine(m_engine);
    m_interface->setName(m_name);
    connect(m_interface, &QmlPluginInterface::qmlPluginUnloaded, this, [this] {
        delete m_component;
        delete m_engine;
        initEngineAndComponent();
    });
}

QQmlComponent *QmlPluginLoader::component() const
{
    return m_component;
}

QmlPluginInterface *QmlPluginLoader::instance() const
{
    return m_interface;
}

void QmlPluginLoader::initEngineAndComponent()
{
    m_engine = new QmlEngine();
    KLocalizedContext *context = new KLocalizedContext(this);
    context->setTranslationDomain(QSL("falkon_%1").arg(m_name));
    m_engine->rootContext()->setContextObject(context);
    m_component = new QQmlComponent(m_engine, QDir(m_path).filePath(QStringLiteral("main.qml")));
    m_engine->setExtensionPath(m_path);
    m_engine->setExtensionName(m_name);
}
