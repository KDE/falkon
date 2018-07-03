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

#include <QObject>
#include <QWebEngineScript>

class QmlUserScript : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool null READ null CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool runsOnSubFrames READ runsOnSubFrames WRITE setRunsOnSubFrames NOTIFY runsOnSubFramesChanged)
    Q_PROPERTY(int worldId READ worldId WRITE setWorldId NOTIFY worldIdChanged)
    Q_PROPERTY(QString sourceCode READ sourceCode WRITE setSourceCode NOTIFY sourceCodeChanged)
    Q_PROPERTY(int injectionPoint READ injectionPoint WRITE setInjectionPoint NOTIFY injectionPointChanged)
public:
    enum InjectionPoint {
        DocumentCreation = QWebEngineScript::DocumentCreation,
        DocumentReady = QWebEngineScript::DocumentReady,
        Deferred = QWebEngineScript::Deferred
    };
    enum ScriptWorldId {
        MainWorld = QWebEngineScript::MainWorld,
        ApplicationWorld = QWebEngineScript::ApplicationWorld,
        UserWorld = QWebEngineScript::UserWorld
    };
    Q_ENUMS(InjectionPoint)
    Q_ENUMS(ScriptWorldId)

    explicit QmlUserScript(QObject *parent = nullptr);
    QWebEngineScript webEngineScript() const;
    void setWebEngineScript(const QWebEngineScript &script);
Q_SIGNALS:
    void nameChanged(const QString &name);
    void runsOnSubFramesChanged(bool runsOnSubFrames);
    void worldIdChanged(int worldId);
    void sourceCodeChanged(const QString &sourceCode);
    void injectionPointChanged(int injectionPoint);
private:
    QWebEngineScript m_webEngineScript;

    bool null() const;
    QString name() const;
    void setName(const QString &name);
    bool runsOnSubFrames() const;
    void setRunsOnSubFrames(bool runsOnSubFrames);
    int worldId() const;
    void setWorldId(int worldId);
    QString sourceCode() const;
    void setSourceCode(const QString &sourceCode);
    int injectionPoint() const;
    void setInjectionPoint(int injectionPoint);
};
