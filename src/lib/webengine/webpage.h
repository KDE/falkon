/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>
#include <QWebEngineScript>
#include <QWebEngineFullScreenRequest>
#include <QVector>

#include "qzcommon.h"

class QEventLoop;
class QWebEngineDownloadItem;
class QWebEngineRegisterProtocolHandlerRequest;

class WebView;
class WebHitTestResult;
class DelayedFileWatcher;

class FALKON_EXPORT WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    enum JsWorld {
        UnsafeJsWorld = QWebEngineScript::MainWorld,
        SafeJsWorld = QWebEngineScript::ApplicationWorld
    };

    explicit WebPage(QObject* parent = nullptr);
    ~WebPage() override;

    WebView *view() const;

    bool execPrintPage(QPrinter *printer, int timeout = 1000);
    QVariant execJavaScript(const QString &scriptSource, quint32 worldId = UnsafeJsWorld, int timeout = 500);

    QPointF mapToViewport(const QPointF &pos) const;
    WebHitTestResult hitTestContent(const QPoint &pos) const;

    void scroll(int x, int y);
    void setScrollPosition(const QPointF &pos);

    bool javaScriptPrompt(const QUrl &securityOrigin, const QString &msg, const QString &defaultValue, QString* result) override;
    bool javaScriptConfirm(const QUrl &securityOrigin, const QString &msg) override;
    void javaScriptAlert(const QUrl &securityOrigin, const QString &msg) override;
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;

    QStringList autoFillUsernames() const;

    QUrl registerProtocolHandlerRequestUrl() const;
    QString registerProtocolHandlerRequestScheme() const;

    bool isRunningLoop();

    bool isLoading() const;

    static QStringList internalSchemes();
    static QStringList supportedSchemes();
    static void addSupportedScheme(const QString &scheme);
    static void removeSupportedScheme(const QString &scheme);

Q_SIGNALS:
    void privacyChanged(bool status);
    void printRequested();
    void navigationRequestAccepted(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame);

protected Q_SLOTS:
    void progress(int prog);
    void finished();

private Q_SLOTS:
    void urlChanged(const QUrl &url);
    void watchedFileChanged(const QString &file);
    void windowCloseRequested();
    void fullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest);
    void featurePermissionRequested(const QUrl &origin, const QWebEnginePage::Feature &feature);
    void renderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode);

private:
    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;
    bool certificateError(const QWebEngineCertificateError &error) override;
    QStringList chooseFiles(FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes) override;
    QWebEnginePage* createWindow(QWebEnginePage::WebWindowType type) override;

    void handleUnknownProtocol(const QUrl &url);
    void desktopServicesOpen(const QUrl &url);

    static QString s_lastUploadLocation;
    static QUrl s_lastUnsupportedUrl;
    static QTime s_lastUnsupportedUrlTime;

    DelayedFileWatcher* m_fileWatcher;
    QEventLoop* m_runningLoop;

    QStringList m_autoFillUsernames;
    QWebEngineRegisterProtocolHandlerRequest *m_registerProtocolHandlerRequest = nullptr;

    int m_loadProgress;
    bool m_blockAlerts;
    bool m_secureStatus;

    QMetaObject::Connection m_contentsResizedConnection;

    friend class WebView;
};

#endif // WEBPAGE_H
