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
#include "webpage.h"
#include "tabbedwebview.h"
#include "browserwindow.h"
#include "pluginproxy.h"
#include "downloadmanager.h"
#include "mainapplication.h"
#include "checkboxdialog.h"
#include "qztools.h"
#include "speeddial.h"
#include "autofill.h"
#include "popupwebview.h"
#include "popupwindow.h"
#include "iconprovider.h"
#include "qzsettings.h"
#include "useragentmanager.h"
#include "delayedfilewatcher.h"
#include "searchenginesmanager.h"
#include "html5permissions/html5permissionsmanager.h"
#include "sitesettingsmanager.h"
#include "javascript/externaljsobject.h"
#include "tabwidget.h"
#include "networkmanager.h"
#include "webhittestresult.h"
#include "ui_jsconfirm.h"
#include "ui_jsalert.h"
#include "ui_jsprompt.h"
#include "passwordmanager.h"
#include "scripts.h"
#include "ocssupport.h"

#include <iostream>

#include <QDir>
#include <QMouseEvent>
#include <QWebChannel>
#include <QWebEngineHistory>
#include <QWebEngineSettings>
#include <QTimer>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFileDialog>
#include <QAuthenticator>
#include <QPushButton>
#include <QUrlQuery>
#include <QtWebEngineWidgetsVersion>

#include <QWebEngineRegisterProtocolHandlerRequest>

QString WebPage::s_lastUploadLocation = QDir::homePath();
QUrl WebPage::s_lastUnsupportedUrl;
QElapsedTimer WebPage::s_lastUnsupportedUrlTime;
QStringList s_supportedSchemes;

static const bool kEnableJsOutput = qEnvironmentVariableIsSet("FALKON_ENABLE_JS_OUTPUT");
static const bool kEnableJsNonBlockDialogs = qEnvironmentVariableIsSet("FALKON_ENABLE_JS_NONBLOCK_DIALOGS");

WebPage::WebPage(QObject* parent)
    : QWebEnginePage(mApp->webProfile(), parent)
    , m_fileWatcher(nullptr)
    , m_runningLoop(nullptr)
    , m_loadProgress(100)
    , m_blockAlerts(false)
    , m_secureStatus(false)
{
    auto *channel = new QWebChannel(this);
    ExternalJsObject::setupWebChannel(channel, this);
    setWebChannel(channel, SafeJsWorld);

    connect(this, &QWebEnginePage::loadProgress, this, &WebPage::progress);
    connect(this, &QWebEnginePage::loadFinished, this, &WebPage::finished);
    connect(this, &QWebEnginePage::urlChanged, this, &WebPage::urlChanged);
    connect(this, &QWebEnginePage::featurePermissionRequested, this, &WebPage::featurePermissionRequested);
    connect(this, &QWebEnginePage::windowCloseRequested, this, &WebPage::windowCloseRequested);
    connect(this, &QWebEnginePage::fullScreenRequested, this, &WebPage::fullScreenRequested);
    connect(this, &QWebEnginePage::renderProcessTerminated, this, &WebPage::renderProcessTerminated);
    connect(this, &QWebEnginePage::certificateError, this, &WebPage::onCertificateError);

    connect(this, &QWebEnginePage::authenticationRequired, this, [this](const QUrl &url, QAuthenticator *auth) {
        mApp->networkManager()->authentication(url, auth, view());
    });

    connect(this, &QWebEnginePage::proxyAuthenticationRequired, this, [this](const QUrl &, QAuthenticator *auth, const QString &proxyHost) {
        mApp->networkManager()->proxyAuthentication(proxyHost, auth, view());
    });

    // Workaround QWebEnginePage not scrolling to anchors when opened in background tab
    m_contentsResizedConnection = connect(this, &QWebEnginePage::contentsSizeChanged, this, [this]() {
        const QString fragment = url().fragment();
        if (!fragment.isEmpty()) {
            runJavaScript(Scripts::scrollToAnchor(fragment));
        }
        disconnect(m_contentsResizedConnection);
    });

    // Workaround for broken load started/finished signals in QtWebEngine 5.10, 5.11
    connect(this, &QWebEnginePage::loadProgress, this, [this](int progress) {
        if (progress == 100) {
            Q_EMIT loadFinished(true);
        }
    });

    connect(this, &QWebEnginePage::registerProtocolHandlerRequested, this, [this](QWebEngineRegisterProtocolHandlerRequest request) {
        delete m_registerProtocolHandlerRequest;
        m_registerProtocolHandlerRequest = new QWebEngineRegisterProtocolHandlerRequest(request);
    });
    connect(this, &QWebEnginePage::printRequested, this, &WebPage::printRequested);
    connect(this, &QWebEnginePage::selectClientCertificate, this, [this](QWebEngineClientCertificateSelection selection) {
        // TODO: It should prompt user
        selection.select(selection.certificates().at(0));
    });
}

WebPage::~WebPage()
{
    delete m_registerProtocolHandlerRequest;

    if (m_runningLoop) {
        m_runningLoop->exit(1);
        m_runningLoop = nullptr;
    }
}

WebView *WebPage::view() const
{
    return static_cast<WebView*>(QWebEngineView::forPage(this));
}

bool WebPage::execPrintPage(QPrinter *printer, int timeout)
{
    QPointer<QEventLoop> loop = new QEventLoop;
    bool result = false;
    QTimer::singleShot(timeout, loop.data(), &QEventLoop::quit);

    connect(view(), &QWebEngineView::printFinished, this, [loop, &result](bool res) {
        if (loop && loop->isRunning()) {
            result = res;
            loop->quit();
        }
    });
    view()->print(printer);

    loop->exec();
    delete loop;

    return result;
}

QVariant WebPage::execJavaScript(const QString &scriptSource, quint32 worldId, int timeout)
{
    QPointer<QEventLoop> loop = new QEventLoop;
    QVariant result;
    QTimer::singleShot(timeout, loop.data(), &QEventLoop::quit);

    runJavaScript(scriptSource, worldId, [loop, &result](const QVariant &res) {
        if (loop && loop->isRunning()) {
            result = res;
            loop->quit();
        }
    });

    loop->exec(QEventLoop::ExcludeUserInputEvents);
    delete loop;

    return result;
}

QPointF WebPage::mapToViewport(const QPointF &pos) const
{
    return QPointF(pos.x() / zoomFactor(), pos.y() / zoomFactor());
}

WebHitTestResult WebPage::hitTestContent(const QPoint &pos) const
{
    return WebHitTestResult(this, pos);
}

void WebPage::scroll(int x, int y)
{
    runJavaScript(QSL("window.scrollTo(window.scrollX + %1, window.scrollY + %2)").arg(x).arg(y), SafeJsWorld);
}

void WebPage::setScrollPosition(const QPointF &pos)
{
    const QPointF v = mapToViewport(pos.toPoint());
    runJavaScript(QSL("window.scrollTo(%1, %2)").arg(v.x()).arg(v.y()), SafeJsWorld);
}

bool WebPage::isRunningLoop()
{
    return m_runningLoop;
}

bool WebPage::isLoading() const
{
    return m_loadProgress < 100;
}

// static
QStringList WebPage::internalSchemes()
{
    return QStringList{
        QSL("http"),
        QSL("https"),
        QSL("file"),
        QSL("ftp"),
        QSL("data"),
        QSL("about"),
        QSL("view-source"),
        QSL("chrome")
    };
}

// static
QStringList WebPage::supportedSchemes()
{
    if (s_supportedSchemes.isEmpty()) {
        s_supportedSchemes = internalSchemes();
    }
    return s_supportedSchemes;
}

// static
void WebPage::addSupportedScheme(const QString &scheme)
{
    s_supportedSchemes = supportedSchemes();
    if (!s_supportedSchemes.contains(scheme)) {
        s_supportedSchemes.append(scheme);
    }
}

// static
void WebPage::removeSupportedScheme(const QString &scheme)
{
    s_supportedSchemes.removeOne(scheme);
}

void WebPage::urlChanged(const QUrl &url)
{
    Q_UNUSED(url)

    if (isLoading()) {
        m_blockAlerts = false;
    }
}

void WebPage::progress(int prog)
{
    m_loadProgress = prog;

    bool secStatus = url().scheme() == QL1S("https");

    if (secStatus != m_secureStatus) {
        m_secureStatus = secStatus;
        Q_EMIT privacyChanged(secStatus);
    }
}

void WebPage::finished()
{
    progress(100);

    // File scheme watcher
    if (url().scheme() == QLatin1String("file")) {
        QFileInfo info(url().toLocalFile());
        if (info.isFile()) {
            if (!m_fileWatcher) {
                m_fileWatcher = new DelayedFileWatcher(this);
                connect(m_fileWatcher, &DelayedFileWatcher::delayedFileChanged, this, &WebPage::watchedFileChanged);
            }

            const QString filePath = url().toLocalFile();

            if (QFile::exists(filePath) && !m_fileWatcher->files().contains(filePath)) {
                m_fileWatcher->addPath(filePath);
            }
        }
    }
    else if (m_fileWatcher && !m_fileWatcher->files().isEmpty()) {
        m_fileWatcher->removePaths(m_fileWatcher->files());
    }

    // AutoFill
    m_autoFillUsernames = mApp->autoFill()->completePage(this, url());
}

void WebPage::watchedFileChanged(const QString &file)
{
    if (url().toLocalFile() == file) {
        triggerAction(QWebEnginePage::Reload);
    }
}

void WebPage::handleUnknownProtocol(const QUrl &url)
{
    const QString protocol = url.scheme();

    if (protocol == QLatin1String("mailto")) {
        desktopServicesOpen(url);
        return;
    }

    if (qzSettings->blockedProtocols.contains(protocol)) {
        qDebug() << "WebPage::handleUnknownProtocol Protocol" << protocol << "is blocked!";
        return;
    }

    if (qzSettings->autoOpenProtocols.contains(protocol)) {
        desktopServicesOpen(url);
        return;
    }

    CheckBoxDialog dialog(QMessageBox::Yes | QMessageBox::No, view());
    dialog.setDefaultButton(QMessageBox::Yes);

    const QString wrappedUrl = QzTools::alignTextToWidth(url.toString(), QSL("<br/>"), dialog.fontMetrics(), 450);
    const QString text = tr("Falkon cannot handle <b>%1:</b> links. The requested link "
                            "is <ul><li>%2</li></ul>Do you want Falkon to try "
                            "open this link in system application?").arg(protocol, wrappedUrl);

    dialog.setText(text);
    dialog.setCheckBoxText(tr("Remember my choice for this protocol"));
    dialog.setWindowTitle(tr("External Protocol Request"));
    dialog.setIcon(QMessageBox::Question);

    switch (dialog.exec()) {
    case QMessageBox::Yes:
        if (dialog.isChecked()) {
            qzSettings->autoOpenProtocols.append(protocol);
            qzSettings->saveSettings();
        }


        QDesktopServices::openUrl(url);
        break;

    case QMessageBox::No:
        if (dialog.isChecked()) {
            qzSettings->blockedProtocols.append(protocol);
            qzSettings->saveSettings();
        }

        break;

    default:
        break;
    }
}

void WebPage::desktopServicesOpen(const QUrl &url)
{
    // Open same url only once in 2 secs
    const int sameUrlTimeout = 2 * 1000;

    if ((s_lastUnsupportedUrl != url) || (!s_lastUnsupportedUrlTime.isValid()) || (s_lastUnsupportedUrlTime.elapsed() > sameUrlTimeout)) {
        s_lastUnsupportedUrl = url;
        s_lastUnsupportedUrlTime.restart();
        QDesktopServices::openUrl(url);
    }
    else {
        qWarning() << "WebPage::desktopServicesOpen Url" << url << "has already been opened!\n"
                   "Ignoring it to prevent infinite loop!";
    }
}

void WebPage::windowCloseRequested()
{
    if (!view())
        return;
    view()->closeView();
}

void WebPage::fullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest)
{
    view()->requestFullScreen(fullScreenRequest.toggleOn());

    const bool accepted = fullScreenRequest.toggleOn() == view()->isFullScreen();

    if (accepted)
        fullScreenRequest.accept();
    else
        fullScreenRequest.reject();
}

void WebPage::featurePermissionRequested(const QUrl &origin, const QWebEnginePage::Feature &feature)
{
    if (feature == MouseLock && view()->isFullScreen())
        setFeaturePermission(origin, feature, PermissionGrantedByUser);
    else
        mApp->html5PermissionsManager()->requestPermissions(this, origin, feature);
}

void WebPage::renderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode)
{
    Q_UNUSED(exitCode)

    if (terminationStatus == NormalTerminationStatus)
        return;

    QTimer::singleShot(0, this, [this]() {
        QString page = QzTools::readAllFileContents(QSL(":html/tabcrash.html"));
        page.replace(QL1S("%IMAGE%"), QzTools::pixmapToDataUrl(IconProvider::standardIcon(QStyle::SP_MessageBoxWarning).pixmap(45)).toString());
        page.replace(QL1S("%TITLE%"), tr("Failed loading page"));
        page.replace(QL1S("%HEADING%"), tr("Failed loading page"));
        page.replace(QL1S("%LI-1%"), tr("Something went wrong while loading this page."));
        page.replace(QL1S("%LI-2%"), tr("Try reloading the page or closing some tabs to make more memory available."));
        page.replace(QL1S("%RELOAD-PAGE%"), tr("Reload page"));
        page = QzTools::applyDirectionToPage(page);
        setHtml(page, url());
    });
}

bool WebPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    if (mApp->isClosing()) {
        return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
    }

    if (!mApp->plugins()->acceptNavigationRequest(this, url, type, isMainFrame))
        return false;

    if (url.scheme() == QL1S("falkon")) {
        if (url.path() == QL1S("AddSearchProvider")) {
            QUrlQuery query(url);
            mApp->searchEnginesManager()->addEngine(QUrl(query.queryItemValue(QSL("url"))));
            return false;
        }
    }

    if (url.scheme() == QL1S("ocs") && OcsSupport::instance()->handleUrl(url)) {
        return false;
    }

    const bool result = QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);

    if (result) {
        if (isMainFrame) {
            const bool isWeb = url.scheme() == QL1S("http") || url.scheme() == QL1S("https") || url.scheme() == QL1S("file");

            if (isWeb) {
                auto webAttributes = mApp->siteSettingsManager()->getWebAttributes2(url);
                if (!webAttributes.empty()) {
                    QHash<QWebEngineSettings::WebAttribute, bool>::iterator it;
                    for (it = webAttributes.begin(); it != webAttributes.end(); ++it) {
                        settings()->setAttribute(it.key(), it.value());
                    }
                }
            }



//             const SiteWebEngineSettings siteSettings = mApp->siteSettingsManager()->getWebEngineSettings(url);
//             settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, isWeb ? siteSettings.allowJavaScript : true);
//             settings()->setAttribute(QWebEngineSettings::AutoLoadImages, isWeb ? siteSettings.allowImages : true);
        }
        Q_EMIT navigationRequestAccepted(url, type, isMainFrame);
    }

    return result;
}

void WebPage::onCertificateError(QWebEngineCertificateError error)
{
    auto mutableError = const_cast<QWebEngineCertificateError&>(error);
    if (mApp->networkManager()->certificateError(mutableError, view()))
        mutableError.acceptCertificate();
    else
        mutableError.rejectCertificate();
}

QStringList WebPage::chooseFiles(QWebEnginePage::FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes)
{
    Q_UNUSED(acceptedMimeTypes);

    QStringList files;
    QString suggestedFileName = s_lastUploadLocation;
    if (!oldFiles.isEmpty())
        suggestedFileName = oldFiles.at(0);

    switch (mode) {
    case FileSelectOpen:
        files = QStringList(QzTools::getOpenFileName(QSL("WebPage-ChooseFile"), view(), tr("Choose file..."), suggestedFileName));
        break;

    case FileSelectOpenMultiple:
        files = QzTools::getOpenFileNames(QSL("WebPage-ChooseFile"), view(), tr("Choose files..."), suggestedFileName);
        break;

    default:
        files = QWebEnginePage::chooseFiles(mode, oldFiles, acceptedMimeTypes);
        break;
    }

    if (!files.isEmpty())
        s_lastUploadLocation = files.at(0);

    return files;
}

QStringList WebPage::autoFillUsernames() const
{
    return m_autoFillUsernames;
}

QUrl WebPage::registerProtocolHandlerRequestUrl() const
{
    if (m_registerProtocolHandlerRequest && url().host() == m_registerProtocolHandlerRequest->origin().host()) {
        return m_registerProtocolHandlerRequest->origin();
    }
    return {};
}

QString WebPage::registerProtocolHandlerRequestScheme() const
{
    if (m_registerProtocolHandlerRequest && url().host() == m_registerProtocolHandlerRequest->origin().host()) {
        return m_registerProtocolHandlerRequest->scheme();
    }
    return {};
}

bool WebPage::javaScriptPrompt(const QUrl &securityOrigin, const QString &msg, const QString &defaultValue, QString* result)
{
    if (!kEnableJsNonBlockDialogs) {
        return QWebEnginePage::javaScriptPrompt(securityOrigin, msg, defaultValue, result);
    }

    if (m_runningLoop) {
        return false;
    }

    auto *widget = new QFrame(view()->overlayWidget());

    widget->setObjectName("jsFrame");
    auto* ui = new Ui_jsPrompt();
    ui->setupUi(widget);
    ui->message->setText(msg);
    ui->lineEdit->setText(defaultValue);
    ui->lineEdit->setFocus();
    widget->resize(view()->size());
    widget->show();

    QAbstractButton *clicked = nullptr;
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, [&](QAbstractButton *button) {
        clicked = button;
    });

    connect(view(), &WebView::viewportResized, widget, QOverload<const QSize &>::of(&QFrame::resize));
    connect(ui->lineEdit, SIGNAL(returnPressed()), ui->buttonBox->button(QDialogButtonBox::Ok), SLOT(animateClick()));

    QEventLoop eLoop;
    m_runningLoop = &eLoop;
    connect(ui->buttonBox, &QDialogButtonBox::clicked, &eLoop, &QEventLoop::quit);

    if (eLoop.exec() == 1) {
        return result;
    }
    m_runningLoop = nullptr;

    QString x = ui->lineEdit->text();
    bool _result = ui->buttonBox->buttonRole(clicked) == QDialogButtonBox::AcceptRole;
    *result = x;

    delete widget;
    view()->setFocus();

    return _result;
}

bool WebPage::javaScriptConfirm(const QUrl &securityOrigin, const QString &msg)
{
    if (!kEnableJsNonBlockDialogs) {
        return QWebEnginePage::javaScriptConfirm(securityOrigin, msg);
    }

    if (m_runningLoop) {
        return false;
    }

    auto *widget = new QFrame(view()->overlayWidget());

    widget->setObjectName("jsFrame");
    auto* ui = new Ui_jsConfirm();
    ui->setupUi(widget);
    ui->message->setText(msg);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setFocus();
    widget->resize(view()->size());
    widget->show();

    QAbstractButton *clicked = nullptr;
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, [&](QAbstractButton *button) {
        clicked = button;
    });

    connect(view(), &WebView::viewportResized, widget, QOverload<const QSize &>::of(&QFrame::resize));

    QEventLoop eLoop;
    m_runningLoop = &eLoop;
    connect(ui->buttonBox, &QDialogButtonBox::clicked, &eLoop, &QEventLoop::quit);

    if (eLoop.exec() == 1) {
        return false;
    }
    m_runningLoop = nullptr;

    bool result = ui->buttonBox->buttonRole(clicked) == QDialogButtonBox::AcceptRole;

    delete widget;
    view()->setFocus();

    return result;
}

void WebPage::javaScriptAlert(const QUrl &securityOrigin, const QString &msg)
{
    Q_UNUSED(securityOrigin)

    if (m_blockAlerts || m_runningLoop) {
        return;
    }

    if (!kEnableJsNonBlockDialogs) {
        QString title = tr("JavaScript alert");
        if (!url().host().isEmpty()) {
            title.append(QSL(" - %1").arg(url().host()));
        }

        CheckBoxDialog dialog(QMessageBox::Ok, view());
        dialog.setDefaultButton(QMessageBox::Ok);
        dialog.setWindowTitle(title);
        dialog.setText(msg);
        dialog.setCheckBoxText(tr("Prevent this page from creating additional dialogs"));
        dialog.setIcon(QMessageBox::Information);
        dialog.exec();

        m_blockAlerts = dialog.isChecked();
        return;
    }

    auto *widget = new QFrame(view()->overlayWidget());

    widget->setObjectName("jsFrame");
    auto* ui = new Ui_jsAlert();
    ui->setupUi(widget);
    ui->message->setText(msg);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setFocus();
    widget->resize(view()->size());
    widget->show();

    connect(view(), &WebView::viewportResized, widget, QOverload<const QSize &>::of(&QFrame::resize));

    QEventLoop eLoop;
    m_runningLoop = &eLoop;
    connect(ui->buttonBox, &QDialogButtonBox::clicked, &eLoop, &QEventLoop::quit);

    if (eLoop.exec() == 1) {
        return;
    }
    m_runningLoop = nullptr;

    m_blockAlerts = ui->preventAlerts->isChecked();

    delete widget;

    view()->setFocus();
}

void WebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
    if (!kEnableJsOutput) {
        return;
    }

    switch (level) {
    case InfoMessageLevel:
        std::cout << "[I] ";
        break;

    case WarningMessageLevel:
        std::cout << "[W] ";
        break;

    case ErrorMessageLevel:
        std::cout << "[E] ";
        break;
    }

    std::cout << qPrintable(sourceID) << ":" << lineNumber << " " << qPrintable(message);
}

QWebEnginePage* WebPage::createWindow(QWebEnginePage::WebWindowType type)
{
    auto *tView = qobject_cast<TabbedWebView*>(view());
    BrowserWindow *window = tView ? tView->browserWindow() : mApp->getWindow();

    auto createTab = [=](Qz::NewTabPositionFlags pos) {
        int index = window->tabWidget()->addView(QUrl(), pos);
        TabbedWebView* view = window->weView(index);
        view->setPage(new WebPage);
        if (tView) {
            tView->webTab()->addChildTab(view->webTab());
        }
        // Workaround focus issue when creating tab
        if (pos.testFlag(Qz::NT_SelectedTab)) {
            QPointer<TabbedWebView> pview = view;
            pview->setFocus();
            QTimer::singleShot(100, this, [pview]() {
                if (pview && pview->webTab()->isCurrentTab()) {
                    pview->setFocus();
                }
            });
        }
        return view->page();
    };

    switch (type) {
    case QWebEnginePage::WebBrowserWindow: {
        BrowserWindow *window = mApp->createWindow(Qz::BW_NewWindow);
        auto *page = new WebPage;
        window->setStartPage(page);
        return page;
    }

    case QWebEnginePage::WebDialog:
        if (!qzSettings->openPopupsInTabs) {
            auto* view = new PopupWebView;
            view->setPage(new WebPage);
            auto* popup = new PopupWindow(view);
            popup->show();
            window->addDeleteOnCloseWidget(popup);
            return view->page();
        }
        // else fallthrough

    case QWebEnginePage::WebBrowserTab:
        return createTab(Qz::NT_CleanSelectedTab);

    case QWebEnginePage::WebBrowserBackgroundTab:
        return createTab(Qz::NT_CleanNotSelectedTab);

    default:
        break;
    }

    return nullptr;
}
