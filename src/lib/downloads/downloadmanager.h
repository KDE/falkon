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
#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QWidget>
#include <QPointer>
#include <QBasicTimer>

#include "qzcommon.h"

namespace Ui
{
class DownloadManager;
}

class QUrl;
class QNetworkAccessManager;
class QListWidgetItem;
class QWebEngineDownloadItem;
class QWinTaskbarButton;

class DownloadItem;
class DownloadManagerModel;
class WebPage;

class FALKON_EXPORT DownloadManager : public QWidget
{
    Q_OBJECT
public:
    enum DownloadOption { OpenFile, SaveFile, ExternalManager, NoOption };

    struct DownloadInfo {
        WebPage* page;
        QString suggestedFileName;

        bool askWhatToDo;
        bool forceChoosingPath;

        DownloadInfo(WebPage* p = nullptr) {
            page = p;
            suggestedFileName = QString();
            askWhatToDo = true;
            forceChoosingPath = false;
        }
    };

    explicit DownloadManager(QWidget* parent = nullptr);
    ~DownloadManager() override;

    void loadSettings();

    void download(QWebEngineDownloadItem *downloadItem);

    int downloadsCount() const;
    int activeDownloadsCount() const;

    bool canClose();

    bool useExternalManager() const;
    void startExternalManager(const QUrl &url);

    void setLastDownloadPath(const QString &lastPath) { m_lastDownloadPath = lastPath; }
    void setLastDownloadOption(DownloadOption option) { m_lastDownloadOption = option; }

public Q_SLOTS:
    void show();

private Q_SLOTS:
    void clearList();
    void downloadFinished(bool success);

Q_SIGNALS:
    void resized(QSize);
    void downloadsCountChanged();
    void downloadAdded(DownloadItem *item);
    void downloadRemoved(DownloadItem *item);
    void downloadFinished();

private:
    void timerEvent(QTimerEvent* e) override;
    void closeEvent(QCloseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

    void closeDownloadTab(QWebEngineDownloadItem *item) const;
    QWinTaskbarButton *taskbarButton();

    Ui::DownloadManager* ui;
    DownloadManagerModel *m_model;
    QBasicTimer m_timer;

    QString m_lastDownloadPath;
    QString m_downloadPath;
    bool m_useNativeDialog;
    bool m_isClosing;
    bool m_closeOnFinish;
    int m_activeDownloadsCount = 0;

    bool m_useExternalManager;
    QString m_externalExecutable;
    QString m_externalArguments;

    DownloadOption m_lastDownloadOption;

    QPointer<QWinTaskbarButton> m_taskbarButton;
};

#endif // DOWNLOADMANAGER_H
