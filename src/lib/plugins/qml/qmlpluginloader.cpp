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
#include <QQmlContext>
#include <QDir>
#include "../config.h"

#if HAVE_LIBINTL
#include "qml/api/i18n/qmli18n.h"
#endif

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
    m_component = new QQmlComponent(m_engine, QDir(m_path).filePath(QStringLiteral("main.qml")));
    m_engine->setExtensionPath(m_path);
    m_engine->setExtensionName(m_name);
#if HAVE_LIBINTL
    auto i18n = new QmlI18n(m_name);
    m_engine->globalObject().setProperty(QSL("__falkon_i18n"), m_engine->newQObject(i18n));
    m_engine->evaluate(QSL("i18n = function (s) { return __falkon_i18n.i18n(s) };"));
    m_engine->evaluate(QSL("i18np = function (s1, s2) { return __falkon_i18n.i18np(s1, s2) }"));
#else
    m_engine->evaluate(QSL("i18n = function (s) { return s; };"));
    m_engine->evaluate(QSL("i18np = function (s1, s2) { return s1; }"));
#endif
}
