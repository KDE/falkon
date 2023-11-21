/* ============================================================
* FlashCookieManager plugin for Falkon
* Copyright (C) 2014-2018 S. Razi Alavizadeh <s.r.alavizadeh@gmail.com>
* Copyright (C) 2017-2018 David Rosca <nowrep@gmail.com>
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
#include "fcm_plugin.h"
#include "browserwindow.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "fcm_dialog.h"
#include "abstractbuttoninterface.h"
#include "tabbedwebview.h"
#include "fcm_notification.h"
#include "datapaths.h"
#include "../config.h"
#include "statusbar.h"
#include "navigationbar.h"

#include <QTimer>
#include <QSettings>
#include <QDir>
#include <QMenu>

#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
#include <QProcessEnvironment>
#endif

const int refreshInterval = 60 * 1000;

class FCM_Button : public AbstractButtonInterface
{
    Q_OBJECT
public:
    explicit FCM_Button(QObject *parent = nullptr)
        : AbstractButtonInterface(parent)
    {
    }

    QString id() const override
    {
        return QSL("fcm-icon");
    }

    QString name() const override
    {
        return tr("Flash Cookie Manager button");
    }
};

FCM_Plugin::FCM_Plugin()
    : QObject()
{
}

void FCM_Plugin::init(InitState state, const QString &settingsPath)
{
    m_settingsPath = settingsPath;

    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, &FCM_Plugin::mainWindowCreated);
    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, this, &FCM_Plugin::mainWindowDeleted);

    m_timer = new QTimer(this);
    m_timer->setInterval(refreshInterval);
    connect(m_timer, &QTimer::timeout, this, &FCM_Plugin::autoRefresh);

    // start timer if needed
    startStopTimer();

    if (state == StartupInitState && readSettings().value(QL1S("deleteAllOnStartExit")).toBool()) {
        loadFlashCookies();

        removeAllButWhitelisted();
    }

    if (state == LateInitState) {
        const auto windows = mApp->windows();
        for (BrowserWindow* window : windows) {
            mainWindowCreated(window);
        }
    }
}

void FCM_Plugin::unload()
{
    if (m_fcmDialog) {
        m_fcmDialog->close();
    }

    if (mApp->isClosing() && readSettings().value(QL1S("deleteAllOnStartExit")).toBool()) {
        removeAllButWhitelisted();
    }

    const auto windows = mApp->windows();
    for (BrowserWindow* window : windows) {
        mainWindowDeleted(window);
    }

    delete m_fcmDialog;
}

bool FCM_Plugin::testPlugin()
{
    return (QString::fromLatin1(Qz::VERSION) == QLatin1String(FALKON_VERSION));
}

void FCM_Plugin::showSettings(QWidget* parent)
{
    Q_UNUSED(parent)

    showFlashCookieManager();
    m_fcmDialog->showPage(2);
}

void FCM_Plugin::populateExtensionsMenu(QMenu* menu)
{
    auto* showFCM = new QAction(QIcon(QSL(":/flashcookiemanager/data/flash-cookie-manager.png")), tr("Flash Cookie Manager"), menu);
    connect(showFCM, &QAction::triggered, this, &FCM_Plugin::showFlashCookieManager);
    menu->addAction(showFCM);
}

void FCM_Plugin::setFlashCookies(const QList<FlashCookie> &flashCookies)
{
    m_flashCookies = flashCookies;
}

QList<FlashCookie> FCM_Plugin::flashCookies()
{
    if (m_flashCookies.isEmpty()) {
        loadFlashCookies();
    }
    return m_flashCookies;
}

QStringList FCM_Plugin::newCookiesList()
{
    return m_newCookiesList;
}

void FCM_Plugin::clearNewOrigins()
{
    m_newCookiesList.clear();
}

void FCM_Plugin::clearCache()
{
    m_flashCookies.clear();
}

bool FCM_Plugin::isBlacklisted(const FlashCookie &flashCookie)
{
    return readSettings().value(QL1S("flashCookiesBlacklist")).toStringList().contains(flashCookie.origin);
}

bool FCM_Plugin::isWhitelisted(const FlashCookie &flashCookie)
{
    return readSettings().value(QL1S("flashCookiesWhitelist")).toStringList().contains(flashCookie.origin);
}

void FCM_Plugin::removeAllButWhitelisted()
{
    for (const FlashCookie &flashCookie : std::as_const(m_flashCookies)) {
        if (isWhitelisted(flashCookie)) {
            continue;
        }

        removeCookie(flashCookie);
    }
}

QString FCM_Plugin::sharedObjectDirName() const
{
    if (flashPlayerDataPath().contains(QL1S("macromedia"), Qt::CaseInsensitive) ||
            !flashPlayerDataPath().contains(QL1S("/.gnash"), Qt::CaseInsensitive)) {
        return QLatin1String(QL1S("/#SharedObjects/"));
    }
    else {
        return QLatin1String(QL1S("/SharedObjects/"));
    }
}

QString FCM_Plugin::flashPlayerDataPath() const
{
    return DataPaths::currentProfilePath() + QSL("/Pepper Data/Shockwave Flash/WritableRoot/");
}

QVariantHash FCM_Plugin::readSettings() const
{
    if (m_settingsHash.isEmpty()) {
        m_settingsHash.insert(QL1S("autoMode"), QVariant(false));
        m_settingsHash.insert(QL1S("deleteAllOnStartExit"), QVariant(false));
        m_settingsHash.insert(QL1S("notification"), QVariant(false));
        m_settingsHash.insert(QL1S("flashCookiesWhitelist"), QVariant());
        m_settingsHash.insert(QL1S("flashCookiesBlacklist"), QVariant());

        QSettings settings(m_settingsPath + QL1S("/extensions.ini"), QSettings::IniFormat);
        settings.beginGroup(QL1S("FlashCookieManager"));
        QVariantHash::iterator i = m_settingsHash.begin();
        while (i != m_settingsHash.end()) {
            *i = settings.value(i.key(), i.value());
            ++i;
        }

        settings.endGroup();
    }

    return m_settingsHash;
}

void FCM_Plugin::writeSettings(const QVariantHash &hashSettings)
{
    m_settingsHash = hashSettings;

    QSettings settings(m_settingsPath + QL1S(QL1S("/extensions.ini")), QSettings::IniFormat);
    settings.beginGroup(QL1S("FlashCookieManager"));
    QVariantHash::const_iterator i = m_settingsHash.constBegin();
    while (i != m_settingsHash.constEnd()) {
        settings.setValue(i.key(), i.value());
        ++i;
    }

    settings.endGroup();

    startStopTimer();
}

void FCM_Plugin::removeCookie(const FlashCookie &flashCookie)
{
    if (m_flashCookies.contains(flashCookie)) {
        m_flashCookies.removeOne(flashCookie);
        if (QFile(flashCookie.path + QL1C('/') + flashCookie.name).remove()) {
            QDir dir(flashCookie.path);
            dir.rmpath(flashCookie.path);
        }
    }
}

void FCM_Plugin::autoRefresh()
{
    if (m_fcmDialog && m_fcmDialog->isVisible()) {
        return;
    }

    QList<FlashCookie> oldflashCookies = m_flashCookies;
    loadFlashCookies();
    QStringList newCookieList;

    for (const FlashCookie &flashCookie : std::as_const(m_flashCookies)) {
        if (isBlacklisted(flashCookie)) {
            removeCookie(flashCookie);
            continue;
        }

        if (isWhitelisted(flashCookie)) {
            continue;
        }

        bool newCookie = true;
        for (const FlashCookie &oldFlashCookie : std::as_const(oldflashCookies)) {
            if (QString(oldFlashCookie.path + oldFlashCookie.name) ==
                    QString(flashCookie.path + flashCookie.name)) {
                newCookie = false;
                break;
            }
        }

        if (newCookie) {
            newCookieList << flashCookie.path + QL1C('/') + flashCookie.name;
        }
    }

    if (!newCookieList.isEmpty() && readSettings().value(QL1S("notification")).toBool()) {
        m_newCookiesList << newCookieList;
        BrowserWindow* window = mApp->getWindow();
        if (!window) {
            return;
        }
        TabbedWebView* weView = window->weView();
        if (!weView) {
            return;
        }

        auto* notif = new FCM_Notification(this, newCookieList.size());
        weView->addNotification(notif);
    }
}

void FCM_Plugin::showFlashCookieManager()
{
    if (!m_fcmDialog) {
        m_fcmDialog = new FCM_Dialog(this);
    }

    m_fcmDialog->refreshView();
    m_fcmDialog->showPage(0);
    m_fcmDialog->show();
    m_fcmDialog->raise();
}

void FCM_Plugin::mainWindowCreated(BrowserWindow *window)
{
    window->statusBar()->addButton(createStatusBarIcon(window));
    window->navigationBar()->addToolButton(createStatusBarIcon(window));
}

void FCM_Plugin::mainWindowDeleted(BrowserWindow *window)
{
    if (!window) {
        return;
    }

    if (m_fcmDialog && m_fcmDialog->parent() == window) {
        m_fcmDialog->setParent(nullptr);
    }

    window->statusBar()->removeButton(m_statusBarIcons.value(window));
    window->navigationBar()->removeToolButton(m_statusBarIcons.value(window));

    delete m_statusBarIcons.value(window);
    m_statusBarIcons.remove(window);
}

void FCM_Plugin::startStopTimer()
{
    if (readSettings().value(QL1S("autoMode")).toBool()) {
        if (!m_timer->isActive()) {
            if (m_flashCookies.isEmpty()) {
                loadFlashCookies();
            }

            m_timer->start();
        }
    }
    else {
        m_timer->stop();
    }
}

AbstractButtonInterface* FCM_Plugin::createStatusBarIcon(BrowserWindow* mainWindow)
{
    if (m_statusBarIcons.contains(mainWindow)) {
        return m_statusBarIcons.value(mainWindow);
    }

    auto* icon = new FCM_Button(this);
    icon->setIcon(QIcon(QSL(":/flashcookiemanager/data/flash-cookie-manager.png")));
    icon->setTitle(tr("Flash Cookie Manager"));
    icon->setToolTip(tr("Show Flash Cookie Manager"));
    connect(icon, &AbstractButtonInterface::clicked, this, &FCM_Plugin::showFlashCookieManager);

    m_statusBarIcons.insert(mainWindow, icon);

    return icon;
}

void FCM_Plugin::loadFlashCookies()
{
    m_flashCookies.clear();
    loadFlashCookies(flashPlayerDataPath());
}

void FCM_Plugin::loadFlashCookies(QString path)
{
    QDir solDir(path);
    QStringList entryList = solDir.entryList();
    entryList.removeAll(QL1S("."));
    entryList.removeAll(QL1S(".."));

    for (QString entry : std::as_const(entryList)) {
        if (path.endsWith(QL1S("#SharedObjects")) && entry == QL1S("#AppContainer")) {
            // specific to IE and Windows
            continue;
        }

        path.replace(QL1C('\\'), QL1C('/'));
        QFileInfo entryInfo(path + QL1C('/') + entry);
        if (entryInfo.isDir()) {
            loadFlashCookies(entryInfo.filePath());
        }
        else if (entryInfo.isFile() && entryInfo.suffix() == QL1S("sol")) {
            insertFlashCookie(entryInfo.filePath());
        }
    }
}

void FCM_Plugin::insertFlashCookie(const QString &path)
{
    QFile solFile(path);
    if (!solFile.open(QFile::ReadOnly)) {
        return;
    }

    QByteArray file = solFile.readAll();
    for (int i = 0; i < file.size(); ++i) {
        if (!((file.at(i) >= 'a' && file.at(i) <= 'z') || (file.at(i) >= 'A' && file.at(i) <= 'Z') ||
              (file.at(i) >= '0' && file.at(i) <= '9'))) {
            file[i] = ' ';
        }
    }

    QString fileStr = QString(QString::fromUtf8(file));
    fileStr = fileStr.split(QL1C('.'), Qt::SkipEmptyParts).join(QL1S("\n"));

    QFileInfo solFileInfo(solFile);

    FlashCookie flashCookie;
    flashCookie.contents = fileStr;
    flashCookie.name = solFileInfo.fileName();
    flashCookie.path = solFileInfo.canonicalPath();
    flashCookie.size = (int)solFile.size();
    flashCookie.lastModification = solFileInfo.lastModified();
    flashCookie.origin = extractOriginFrom(path);

    m_flashCookies << flashCookie;
}

QString FCM_Plugin::extractOriginFrom(const QString &path)
{
    QString origin = path;
    if (path.startsWith(flashPlayerDataPath() + sharedObjectDirName())) {
        origin.remove(flashPlayerDataPath() + sharedObjectDirName());
        if (origin.indexOf(QL1C('/')) != -1) {
            origin.remove(0, origin.indexOf(QL1C('/')) + 1);
        }
    }
    else if (path.startsWith(flashPlayerDataPath() + QL1S("/macromedia.com/support/flashplayer/sys/"))) {
        origin.remove(flashPlayerDataPath() + QL1S("/macromedia.com/support/flashplayer/sys/"));
        if (origin == QL1S("settings.sol")) {
            return tr("!default");
        }
        else if (origin.startsWith(QL1C('#'))) {
            origin.remove(0, 1);
        }
    }
    else {
        origin.clear();
    }

    int index = origin.indexOf(QL1C('/'));
    if (index == -1) {
        return tr("!other");
    }
    origin.remove(index, origin.size());
    if (origin == QL1S("localhost") || origin == QL1S("local")) {
        origin = QL1S("!localhost");
    }

    return origin;
}

#include "fcm_plugin.moc"
