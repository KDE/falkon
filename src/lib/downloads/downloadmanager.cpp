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
#include "downloadmanager.h"
#include "ui_downloadmanager.h"
#include "browserwindow.h"
#include "mainapplication.h"
#include "downloadoptionsdialog.h"
#include "downloaditem.h"
#include "downloadmanagermodel.h"
#include "networkmanager.h"
#include "desktopnotificationsfactory.h"
#include "qztools.h"
#include "webpage.h"
#include "webview.h"
#include "settings.h"
#include "datapaths.h"
#include "tabwidget.h"
#include "tabbedwebview.h"
#include "tabbar.h"
#include "locationbar.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QDir>
#include <QShortcut>
#include <QStandardPaths>
#include <QWebEngineHistory>
#include <QWebEngineDownloadRequest>
#include <QtWebEngineWidgetsVersion>

DownloadManager::DownloadManager(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadManager)
    , m_model(new DownloadManagerModel(this))
    , m_isClosing(false)
    , m_lastDownloadOption(NoOption)
{
    setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint);
    ui->setupUi(this);
    ui->clearButton->setIcon(QIcon::fromTheme(QSL("edit-clear")));
    QzTools::centerWidgetOnScreen(this);

    connect(ui->clearButton, &QAbstractButton::clicked, this, &DownloadManager::clearList);

    auto* clearShortcut = new QShortcut(QKeySequence(QSL("CTRL+L")), this);
    connect(clearShortcut, &QShortcut::activated, this, &DownloadManager::clearList);

    loadSettings();

    QzTools::setWmClass(QSL("Download Manager"), this);

    connect(m_model, &DownloadManagerModel::downloadAdded, this, &DownloadManager::downloadAdded);
}

void DownloadManager::loadSettings()
{
    Settings settings;
    settings.beginGroup(QSL("DownloadManager"));
    m_downloadPath = settings.value(QSL("defaultDownloadPath"), QString()).toString();
    m_lastDownloadPath = settings.value(QSL("lastDownloadPath"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).toString();
    m_closeOnFinish = settings.value(QSL("CloseManagerOnFinish"), false).toBool();
    m_useNativeDialog = settings.value(QSL("useNativeDialog"), DEFAULT_DOWNLOAD_USE_NATIVE_DIALOG).toBool();

    m_useExternalManager = settings.value(QSL("UseExternalManager"), false).toBool();
    m_externalExecutable = settings.value(QSL("ExternalManagerExecutable"), QString()).toString();
    m_externalArguments = settings.value(QSL("ExternalManagerArguments"), QString()).toString();
    settings.endGroup();

    if (!m_externalArguments.contains(QLatin1String("%d"))) {
        m_externalArguments.append(QLatin1String(" %d"));
    }
}

void DownloadManager::show()
{
    m_timer.start(500, this);

    QWidget::show();
    raise();
    activateWindow();
}

void DownloadManager::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    Q_EMIT resized(size());
}

void DownloadManager::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape
        || (e->key() == Qt::Key_W && e->modifiers() == Qt::ControlModifier)) {
        close();
    }

    QWidget::keyPressEvent(e);
}

void DownloadManager::closeDownloadTab(QWebEngineDownloadRequest *item) const
{
    // Attempt to close empty tab that was opened only for loading the download url
    auto testWebView = [](TabbedWebView *view, const QUrl &url) {
        if (!view->webTab()->isRestored()) {
            return false;
        }
        if (view->browserWindow()->tabWidget()->tabBar()->normalTabsCount() < 2) {
            return false;
        }
        WebPage *page = view->page();
        if (page->history()->count() != 0) {
            return false;
        }
        Q_UNUSED(url)
        return true;
    };

    if (!item->page()) {
        return;
    }
    auto *page = qobject_cast<WebPage*>(item->page());
    if (!page) {
        return;
    }
    auto *view = qobject_cast<TabbedWebView*>(page->view());
    if (!view) {
        return;
    }
    if (testWebView(view, QUrl())) {
        view->closeView();
    }
}

void DownloadManager::startExternalManager(const QUrl &url)
{
    QString arguments = m_externalArguments;
    arguments.replace(QLatin1String("%d"), QString::fromUtf8(url.toEncoded()));

    QzTools::startExternalProcess(m_externalExecutable, arguments);
    m_lastDownloadOption = ExternalManager;
}

void DownloadManager::timerEvent(QTimerEvent* e)
{
    QVector<QTime> remTimes;
    QVector<int> progresses;
    QVector<double> speeds;

    if (e->timerId() == m_timer.timerId()) {
        if (!ui->list->count()) {
            ui->speedLabel->clear();
            setWindowTitle(tr("Download Manager"));
            return;
        }
        for (int i = 0; i < ui->list->count(); i++) {
            auto* downItem = qobject_cast<DownloadItem*>(ui->list->itemWidget(ui->list->item(i)));
            if (!downItem || downItem->isCancelled() || !downItem->isDownloading()) {
                continue;
            }
            progresses.append(downItem->progress());
            remTimes.append(downItem->remainingTime());
            speeds.append(downItem->currentSpeed());
        }
        if (remTimes.isEmpty()) {
            return;
        }

        QTime remaining;
        for (const QTime &time : std::as_const(remTimes)) {
            if (time > remaining) {
                remaining = time;
            }
        }

        int progress = 0;
        for (int prog : std::as_const(progresses)) {
            progress += prog;
        }
        progress = progress / progresses.count();

        double speed = 0.00;
        for (double spee : std::as_const(speeds)) {
            speed += spee;
        }

        ui->speedLabel->setText(tr("%1% of %2 files (%3) %4 remaining").arg(QString::number(progress), QString::number(progresses.count()),
                                DownloadItem::currentSpeedToString(speed),
                                DownloadItem::remaingTimeToString(remaining)));
        setWindowTitle(tr("%1% - Download Manager").arg(progress));
    }

    QWidget::timerEvent(e);
}

void DownloadManager::clearList()
{
    QList<DownloadItem*> items;
    for (int i = 0; i < ui->list->count(); i++) {
        auto* downItem = qobject_cast<DownloadItem*>(ui->list->itemWidget(ui->list->item(i)));
        if (!downItem) {
            continue;
        }
        if (downItem->isDownloading()) {
            continue;
        }
        items.append(downItem);
    }
    qDeleteAll(items);
    Q_EMIT downloadsCountChanged();
}

void DownloadManager::download(QWebEngineDownloadRequest *downloadItem)
{
    QElapsedTimer downloadTimer;
    downloadTimer.start();

    closeDownloadTab(downloadItem);

    QString downloadPath;
    bool openFile = false;

    const QString fileName = downloadItem->downloadFileName();

    const bool forceAsk = downloadItem->savePageFormat() != QWebEngineDownloadRequest::UnknownSaveFormat
            || downloadItem->isSavePageDownload();

    if (m_useExternalManager) {
        startExternalManager(downloadItem->url());
    } else if (forceAsk || m_downloadPath.isEmpty()) {
        enum Result { Open = 1, Save = 2, ExternalManager = 3, SavePage = 4, Unknown = 0 };
        Result result = Unknown;

        if (downloadItem->savePageFormat() != QWebEngineDownloadRequest::UnknownSaveFormat) {
            // Save Page requested
            result = SavePage;
        } else if (downloadItem->isSavePageDownload()) {
            // Save x as... requested
            result = Save;
        } else {
            // Ask what to do
            DownloadOptionsDialog optionsDialog(fileName, downloadItem, mApp->activeWindow());
            optionsDialog.showExternalManagerOption(m_useExternalManager);
            optionsDialog.setLastDownloadOption(m_lastDownloadOption);
            result = Result(optionsDialog.exec());
        }

        switch (result) {
        case Open:
            openFile = true;
            downloadPath = QzTools::ensureUniqueFilename(DataPaths::path(DataPaths::Temp) + QLatin1Char('/') + fileName);
            m_lastDownloadOption = OpenFile;
            break;

        case Save:
            downloadPath = QFileDialog::getSaveFileName(mApp->activeWindow(), tr("Save file as..."), m_lastDownloadPath + QLatin1Char('/') + fileName);

            if (!downloadPath.isEmpty()) {
                m_lastDownloadPath = QFileInfo(downloadPath).absolutePath();
                Settings().setValue(QSL("DownloadManager/lastDownloadPath"), m_lastDownloadPath);
                m_lastDownloadOption = SaveFile;
            }
            break;

        case SavePage: {
            const QString mhtml = tr("MIME HTML Archive (*.mhtml)");
            const QString htmlSingle = tr("HTML Page, single (*.html)");
            const QString htmlComplete = tr("HTML Page, complete (*.html)");
            const QString filter = QStringLiteral("%1;;%2;;%3").arg(mhtml, htmlSingle, htmlComplete);

            QString selectedFilter;
            downloadPath = QFileDialog::getSaveFileName(mApp->activeWindow(), tr("Save page as..."),
                                                        m_lastDownloadPath + QLatin1Char('/') + fileName,
                                                        filter, &selectedFilter);

            if (!downloadPath.isEmpty()) {
                m_lastDownloadPath = QFileInfo(downloadPath).absolutePath();
                Settings().setValue(QSL("DownloadManager/lastDownloadPath"), m_lastDownloadPath);
                m_lastDownloadOption = SaveFile;

                QWebEngineDownloadRequest::SavePageFormat format = QWebEngineDownloadRequest::UnknownSaveFormat;

                if (selectedFilter == mhtml) {
                    format = QWebEngineDownloadRequest::MimeHtmlSaveFormat;
                } else if (selectedFilter == htmlSingle) {
                    format = QWebEngineDownloadRequest::SingleHtmlSaveFormat;
                } else if (selectedFilter == htmlComplete) {
                    format = QWebEngineDownloadRequest::CompleteHtmlSaveFormat;
                }

                if (format != QWebEngineDownloadRequest::UnknownSaveFormat) {
                    downloadItem->setSavePageFormat(format);
                }
            }
            break;
        }

        case ExternalManager:
            startExternalManager(downloadItem->url());
            // fallthrough

        default:
            downloadItem->cancel();
            return;
        }
    } else {
        downloadPath = QzTools::ensureUniqueFilename(m_downloadPath + QL1C('/') + fileName);
    }

    if (downloadPath.isEmpty()) {
        downloadItem->cancel();
        return;
    }

    // Set download path and accept
    downloadItem->setDownloadDirectory(QFileInfo(downloadPath).absoluteDir().absolutePath());
    downloadItem->setDownloadFileName(QFileInfo(downloadPath).fileName());
    downloadItem->accept();

    // Create download item
    auto* listItem = new QListWidgetItem(ui->list);
    auto* downItem = new DownloadItem(listItem, downloadItem, QFileInfo(downloadPath).absolutePath(), QFileInfo(downloadPath).fileName(), openFile, this);
    downItem->setDownTimer(downloadTimer);
    connect(downItem, &DownloadItem::deleteItem, m_model, &DownloadManagerModel::removeDownload);
    connect(downItem, &DownloadItem::downloadFinished, this, QOverload<bool>::of(&DownloadManager::downloadFinished));
    connect(downItem, &DownloadItem::downloadFinished, this, QOverload<>::of(&DownloadManager::downloadFinished));
    downItem->startDownloading();
    m_model->addDownload(downItem);
    ui->list->setItemWidget(listItem, downItem);
    listItem->setSizeHint(downItem->sizeHint());
    downItem->show();

    m_activeDownloadsCount++;
    Q_EMIT downloadsCountChanged();
}

int DownloadManager::downloadsCount() const
{
    return m_model->count();
}

int DownloadManager::activeDownloadsCount() const
{
    return m_activeDownloadsCount;
}

void DownloadManager::downloadFinished(bool success)
{
    m_activeDownloadsCount = 0;
    bool downloadingAllFilesFinished = true;
    for (int i = 0; i < ui->list->count(); i++) {
        auto* downItem = qobject_cast<DownloadItem*>(ui->list->itemWidget(ui->list->item(i)));
        if (!downItem) {
            continue;
        }
        if (downItem->isDownloading()) {
            m_activeDownloadsCount++;
        }
        if (downItem->isCancelled() || !downItem->isDownloading()) {
            continue;
        }
        downloadingAllFilesFinished = false;
    }

    Q_EMIT downloadsCountChanged();

    if (downloadingAllFilesFinished) {
        if (success && qApp->activeWindow() != this) {
            mApp->desktopNotifications()->showNotification(QIcon::fromTheme(QSL("download"), QIcon(QSL(":icons/other/download.svg"))).pixmap(48), tr("Falkon: Download Finished"), tr("All files have been successfully downloaded."));
            if (!m_closeOnFinish) {
                raise();
                activateWindow();
            }
        }
        ui->speedLabel->clear();
        setWindowTitle(tr("Download Manager"));
        if (m_closeOnFinish) {
            close();
        }
    }
}

bool DownloadManager::canClose()
{
    if (m_isClosing) {
        return true;
    }

    bool isDownloading = false;
    for (int i = 0; i < ui->list->count(); i++) {
        auto* downItem = qobject_cast<DownloadItem*>(ui->list->itemWidget(ui->list->item(i)));
        if (!downItem) {
            continue;
        }
        if (downItem->isDownloading()) {
            isDownloading = true;
            break;
        }
    }

    return !isDownloading;
}

bool DownloadManager::useExternalManager() const
{
    return m_useExternalManager;
}

void DownloadManager::closeEvent(QCloseEvent* e)
{
    if (mApp->windowCount() == 0) { // No main windows -> we are going to quit
        if (!canClose()) {
            QMessageBox::StandardButton button = QMessageBox::warning(this, tr("Warning"),
                                                 tr("Are you sure you want to quit? All uncompleted downloads will be cancelled!"), QMessageBox::Yes | QMessageBox::No);
            if (button != QMessageBox::Yes) {
                e->ignore();
                return;
            }
            m_isClosing = true;
        }
        mApp->quitApplication();
    }
    e->accept();
}

DownloadManager::~DownloadManager()
{
    delete ui;
}

