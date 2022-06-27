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

#include "qzcommon.h"

#include <QObject>
#include <QBasicTimer>
#include <QWebEngineScript>

/**
 * @brief The class exposing QWebEngineScript to QML
 */
class FALKON_EXPORT QmlUserScript : public QObject
{
    Q_OBJECT
    /**
     * @brief Checks if the UserScript is null
     */
    Q_PROPERTY(bool null READ null CONSTANT)
    /**
     * @brief Name of the UserScript
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    /**
     * @brief Checks if the UserScript runs on sub frames
     */
    Q_PROPERTY(bool runsOnSubFrames READ runsOnSubFrames WRITE setRunsOnSubFrames NOTIFY runsOnSubFramesChanged)
    /**
     * @brief WorldId of the UserScript
     */
    Q_PROPERTY(int worldId READ worldId WRITE setWorldId NOTIFY worldIdChanged)
    /**
     * @brief Source code of the UserScript
     */
    Q_PROPERTY(QString sourceCode READ sourceCode WRITE setSourceCode NOTIFY sourceCodeChanged)
    /**
     * @brief Injection point of the UserScript
     */
    Q_PROPERTY(InjectionPoint injectionPoint READ injectionPoint WRITE setInjectionPoint NOTIFY injectionPointChanged)
public:
    /**
     * @brief The enum exposing QWebEngineScript::InjectionPoint
     */
    enum InjectionPoint {
        DocumentCreation = QWebEngineScript::DocumentCreation, //!< Represents QWebEngineScript::DocumentCreation
        DocumentReady = QWebEngineScript::DocumentReady,       //!< Represents QWebEngineScript::DocumentReady,
        Deferred = QWebEngineScript::Deferred                  //!< Represents QWebEngineScript::Deferred
    };
    /**
     * @brief The enum wrapping QWebEngineScript::ScriptWorldId
     */
    enum ScriptWorldId {
        MainWorld = QWebEngineScript::MainWorld,               //!< Represents QWebEngineScript::MainWorld
        ApplicationWorld = QWebEngineScript::ApplicationWorld, //!< Represents QWebEngineScript::ApplicationWorld
        UserWorld = QWebEngineScript::UserWorld                //!< Represents QWebEngineScript::UserWorld
    };
    Q_ENUM(InjectionPoint)
    Q_ENUM(ScriptWorldId)

    explicit QmlUserScript(QObject *parent = nullptr);
    ~QmlUserScript() override;
    QWebEngineScript webEngineScript() const;
    void setWebEngineScript(const QWebEngineScript &script);
Q_SIGNALS:
    /**
     * @brief The signal emitted when the script name is changed
     */
    void nameChanged(const QString &name);
    /**
     * @brief The signal emitted when runsOnSubFrame property of the script is changed
     */
    void runsOnSubFramesChanged(bool runsOnSubFrames);
    /**
     * @brief The signal emitted when worldId property of the script is changed
     */
    void worldIdChanged(int worldId);
    /**
     * @brief The signal emitted when source code of the script is changed
     */
    void sourceCodeChanged(const QString &sourceCode);
    /**
     * @brief The signal emitted when injectionPoint property of the script is changed
     */
    void injectionPointChanged(int injectionPoint);
private:
    QWebEngineScript m_webEngineScript;
    QBasicTimer m_basicTimer;

    bool null() const;
    QString name() const;
    void setName(const QString &name);
    bool runsOnSubFrames() const;
    void setRunsOnSubFrames(bool runsOnSubFrames);
    int worldId() const;
    void setWorldId(int worldId);
    QString sourceCode() const;
    void setSourceCode(const QString &sourceCode);
    InjectionPoint injectionPoint() const;
    void setInjectionPoint(InjectionPoint injectionPoint);

    void timerEvent(QTimerEvent *e) override;
    void aboutToUpdateUnderlyingScript();
};
