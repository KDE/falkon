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
#include "qmluserscript.h"
#include "../../qmlstaticdata.h"

QmlUserScript::QmlUserScript(QObject *parent)
    : QObject(parent)
{
}

QmlUserScript::~QmlUserScript()
{
    QmlStaticData::instance().getUserScriptsSingleton()->remove(this);
}

QWebEngineScript QmlUserScript::webEngineScript() const
{
    return m_webEngineScript;
}

void QmlUserScript::setWebEngineScript(const QWebEngineScript &script)
{
    m_webEngineScript = script;
}

bool QmlUserScript::null() const
{
    return m_webEngineScript.isNull();
}

QString QmlUserScript::name() const
{
    return m_webEngineScript.name();
}

void QmlUserScript::setName(const QString &name)
{
    m_webEngineScript.setName(name);
    Q_EMIT nameChanged(name);
    aboutToUpdateUnderlyingScript();
}

bool QmlUserScript::runsOnSubFrames() const
{
    return m_webEngineScript.runsOnSubFrames();
}

void QmlUserScript::setRunsOnSubFrames(bool runsOnSubFrames)
{
    m_webEngineScript.setRunsOnSubFrames(runsOnSubFrames);
    Q_EMIT runsOnSubFramesChanged(runsOnSubFrames);
    aboutToUpdateUnderlyingScript();
}

int QmlUserScript::worldId() const
{
    return static_cast<int>(m_webEngineScript.worldId());
}

void QmlUserScript::setWorldId(int worldId)
{
    switch (worldId) {
    case QWebEngineScript::MainWorld:
        m_webEngineScript.setWorldId(QWebEngineScript::MainWorld);
        break;
    case QWebEngineScript::ApplicationWorld:
        m_webEngineScript.setWorldId(QWebEngineScript::ApplicationWorld);
        break;
    case QWebEngineScript::UserWorld:
        m_webEngineScript.setWorldId(QWebEngineScript::UserWorld);
        break;
    default:
        break;
    }
    Q_EMIT worldIdChanged(worldId);
    aboutToUpdateUnderlyingScript();
}

QString QmlUserScript::sourceCode() const
{
    return m_webEngineScript.sourceCode();
}

void QmlUserScript::setSourceCode(const QString &sourceCode)
{
    m_webEngineScript.setSourceCode(sourceCode);
    Q_EMIT sourceCodeChanged(sourceCode);
    aboutToUpdateUnderlyingScript();
}

QmlUserScript::InjectionPoint QmlUserScript::injectionPoint() const
{
    return static_cast<InjectionPoint>(m_webEngineScript.injectionPoint());
}

void QmlUserScript::setInjectionPoint(InjectionPoint injectionPoint)
{
    switch (static_cast<QWebEngineScript::InjectionPoint>(injectionPoint)) {
    case QWebEngineScript::DocumentCreation:
        m_webEngineScript.setInjectionPoint(QWebEngineScript::DocumentCreation);
        break;
    case QWebEngineScript::DocumentReady:
        m_webEngineScript.setInjectionPoint(QWebEngineScript::DocumentReady);
        break;
    case QWebEngineScript::Deferred:
        m_webEngineScript.setInjectionPoint(QWebEngineScript::Deferred);
        break;
    default:
        break;
    }
    Q_EMIT injectionPointChanged(injectionPoint);
    aboutToUpdateUnderlyingScript();
}

void QmlUserScript::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != m_basicTimer.timerId()) {
        QObject::timerEvent(e);
        return;
    }
    m_basicTimer.stop();
    QmlStaticData::instance().getUserScriptsSingleton()->insert(this);
}

void QmlUserScript::aboutToUpdateUnderlyingScript()
{
    if (!m_basicTimer.isActive()) {
        QmlStaticData::instance().getUserScriptsSingleton()->remove(this);
    }
    // Defer updates to the next event loop
    m_basicTimer.start(0, this);
}
