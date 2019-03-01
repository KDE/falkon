/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
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
#ifndef DOWNLOADITEM_H
#define DOWNLOADITEM_H

#include <QWidget>
#include <QFile>
#include <QBasicTimer>
#include <QUrl>
#include <QNetworkReply>
#include <QTime>

#include "qzcommon.h"

namespace Ui
{
class DownloadItem;
}

class QListWidgetItem;
class QWebEngineDownloadItem;

class DownloadManager;

class FALKON_EXPORT DownloadItem : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadItem(QListWidgetItem* item, QWebEngineDownloadItem* downloadItem, const QString &path, const QString &fileName, bool openFile, DownloadManager* manager);
    bool isDownloading() { return m_downloading; }
    bool isCancelled();
    QTime remainingTime() { return m_remTime; }
    double currentSpeed() { return m_currSpeed; }
    int progress();
    ~DownloadItem();
    void setDownTimer(const QTime &timer) { m_downTimer = timer; }

    void startDownloading();

    static QString remaingTimeToString(QTime time);
    static QString currentSpeedToString(double speed);

Q_SIGNALS:
    void deleteItem(DownloadItem*);
    void downloadFinished(bool success);

private Q_SLOTS:
    void parentResized(const QSize &size);
    void finished();
    void downloadProgress(qint64 received, qint64 total);
    void stop();
    void openFile();
    void openFolder();
    void customContextMenuRequested(const QPoint &pos);
    void clear();

    void copyDownloadLink();

private:
    void updateDownloadInfo(double currSpeed, qint64 received, qint64 total);
    void mouseDoubleClickEvent(QMouseEvent* e);

    Ui::DownloadItem* ui;

    QListWidgetItem* m_item;
    QWebEngineDownloadItem* m_download;
    QString m_path;
    QString m_fileName;
    QTime m_downTimer;
    QTime m_remTime;
    QUrl m_downUrl;
    bool m_openFile;

    bool m_downloading;
    bool m_downloadStopped;
    double m_currSpeed;
    qint64 m_received;
    qint64 m_total;
};

class FALKON_EXPORT DownloadItem2 : public QObject
{
    Q_OBJECT

public:
    enum Flag {
        CanPause = 1,
        ActiveDownload = 2, // inactive download = eg. download from history
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    enum State {
        DownloadStarting = 0,
        DownloadInProgress,
        DownloadPaused,
        DownloadFinished,
        DownloadCanceled,
        DownloadError
    };

    enum Error {
        NoError = 0,
        UnknownError
    };

    explicit DownloadItem2(QObject *parent = nullptr);

    Flags flags() const;
    State state() const;
    Error error() const;
    bool isError() const;
    bool isPaused() const;
    bool isFinished() const;
    bool isCanceled() const;

    QUrl url() const;
    QString path() const;

    int progress() const;
    qint64 bytesReceived() const;
    qint64 bytesTotal() const;
    qint64 currentSpeed() const;

    virtual void cancel();
    virtual void pause();
    virtual void resume();

Q_SIGNALS:
    void finished();
    void stateChanged(State state);
    void downloadProgress(qint64 received, qint64 total);

protected:
    void setFlags(Flags flags);
    void setState(State state);
    void setError(Error error);
    void setUrl(const QUrl &url);
    void setPath(const QString &path);
    void updateProgress(qint64 received, qint64 total);

    void resetSpeedTimer();

private:
    Flags m_flags;
    State m_state = DownloadStarting;
    Error m_error = NoError;
    QUrl m_url;
    QString m_path;
    qint64 m_bytesReceived = 0;
    qint64 m_bytesTotal = 0;
    qint64 m_currentSpeed = 0;
    QTime m_speedTimer;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DownloadItem2::Flags)

#endif // DOWNLOADITEM_H
