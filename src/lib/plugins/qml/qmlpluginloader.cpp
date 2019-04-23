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
#include "qmlplugincontext.h"
#include "qmlplugininterface.h"
#include "../config.h"

#include <QDir>
#include <QFileInfo>
#include <QQmlEngine>
#include <QQmlComponent>

#if HAVE_LIBINTL
#include "qml/api/i18n/qmli18n.h"
#endif

Q_GLOBAL_STATIC(QQmlEngine, s_engine)

QmlPluginLoader::QmlPluginLoader(const Plugins::Plugin &plugin)
    : QObject()
    , m_plugin(plugin)
{
}

QString QmlPluginLoader::errorString() const
{
    return m_component ? m_component->errorString() : QString();
}

QmlPluginInterface *QmlPluginLoader::instance() const
{
    return m_interface;
}

void QmlPluginLoader::createComponent()
{
    initEngineAndComponent();

    m_interface = qobject_cast<QmlPluginInterface*>(m_component->create(m_context));
    if (!m_interface) {
        qWarning() << "Failed to create QmlPluginInterface!";
        return;
    }

    connect(m_interface, &QmlPluginInterface::qmlPluginUnloaded, this, [this] {
        m_component->deleteLater();
        m_component = nullptr;
        m_context->deleteLater();
        m_context = nullptr;
    });
}

void QmlPluginLoader::initEngineAndComponent()
{
    if (m_component) {
        return;
    }

    m_component = new QQmlComponent(s_engine(), QDir(m_plugin.pluginPath).filePath(QStringLiteral("main.qml")), this);
    m_context = new QmlPluginContext(m_plugin, s_engine(), this);
#if HAVE_LIBINTL
    auto i18n = new QmlI18n(QFileInfo(m_plugin.pluginPath).fileName());
    s_engine()->globalObject().setProperty(QSL("__falkon_i18n"), s_engine()->newQObject(i18n));
    s_engine()->evaluate(QSL("i18n = function (s) { return __falkon_i18n.i18n(s) };"));
    s_engine()->evaluate(QSL("i18np = function (s1, s2) { return __falkon_i18n.i18np(s1, s2) }"));
#else
    s_engine()->evaluate(QSL("i18n = function (s) { return s; };"));
    s_engine()->evaluate(QSL("i18np = function (s1, s2) { return s1; }"));
#endif
}
