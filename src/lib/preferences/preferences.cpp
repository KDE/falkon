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
#include "preferences.h"
#include "ui_preferences.h"
#include "browserwindow.h"
#include "bookmarkstoolbar.h"
#include "history.h"
#include "tabwidget.h"
#include "cookiejar.h"
#include "locationbar.h"
#include "autofillmanager.h"
#include "mainapplication.h"
#include "cookiemanager.h"
#include "pluginproxy.h"
#include "pluginsmanager.h"
#include "jsoptions.h"
#include "networkmanager.h"
#include "desktopnotificationsfactory.h"
#include "desktopnotification.h"
#include "thememanager.h"
#include "acceptlanguage.h"
#include "qztools.h"
#include "autofill.h"
#include "settings.h"
#include "datapaths.h"
#include "tabbedwebview.h"
#include "clearprivatedata.h"
#include "useragentdialog.h"
#include "registerqappassociation.h"
#include "profilemanager.h"
#include "html5permissions/html5permissionsdialog.h"
#include "certificatemanager.h"
#include "searchenginesdialog.h"
#include "webscrollbarmanager.h"
#include "protocolhandlerdialog.h"
#include "schememanager.h"
#include "../config.h"
#include "siteinfopermissiondefaultitem.h"

#include <QSettings>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QColorDialog>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QLibraryInfo>
#include <QtWebEngineWidgetsVersion>

static QString createLanguageItem(const QString &lang)
{
    QLocale locale(lang);

    if (locale.language() == QLocale::C) {
        return lang;
    }

    const QString country = QLocale::countryToString(locale.country());
    const QString language = QLocale::languageToString(locale.language());

    if (lang == QLatin1String("es_ES")) {
        return QString::fromUtf8("Castellano");
    }
    if (lang == QLatin1String("nqo")) {
        return QSL("N'ko (nqo)");
    }
    if (lang == QLatin1String("sr")) {
        return QString::fromUtf8("српски екавски");
    }
    if (lang == QLatin1String("sr@ijekavian")) {
        return QString::fromUtf8("српски ијекавски");
    }
    if (lang == QLatin1String("sr@latin")) {
        return QString::fromUtf8("srpski ekavski");
    }
    if (lang == QLatin1String("sr@ijekavianlatin")) {
        return QString::fromUtf8("srpski ijekavski");
    }
    return QStringLiteral("%1, %2 (%3)").arg(language, country, lang);
}

Preferences::Preferences(BrowserWindow* window)
    : QDialog(window)
    , ui(new Ui::Preferences)
    , m_window(window)
    , m_autoFillManager(nullptr)
    , m_pluginsList(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    QzTools::centerWidgetOnScreen(this);

    m_themesManager = new ThemeManager(ui->themesWidget, this);
    m_pluginsList = new PluginsManager(this);
    ui->pluginsFrame->addWidget(m_pluginsList);

#ifdef DISABLE_CHECK_UPDATES
    ui->checkUpdates->setVisible(false);
#endif


    auto setCategoryIcon = [this](int index, const QIcon &icon) {
        ui->listWidget->item(index)->setIcon(QIcon(icon.pixmap(32)));
    };

    setCategoryIcon(0, QIcon(QStringLiteral(":/icons/preferences/general.svg")));
    setCategoryIcon(1, QIcon(QStringLiteral(":/icons/preferences/appearance.svg")));
    setCategoryIcon(2, QIcon(QStringLiteral(":/icons/preferences/tabs.svg")));
    setCategoryIcon(3, QIcon(QStringLiteral(":/icons/preferences/browsing.svg")));
    setCategoryIcon(4, QIcon(QStringLiteral(":/icons/preferences/fonts.svg")));
    setCategoryIcon(5, QIcon(QStringLiteral(":/icons/preferences/shortcuts.svg")));
    setCategoryIcon(6, QIcon(QStringLiteral(":/icons/preferences/downloads.svg")));
    setCategoryIcon(7, QIcon(QStringLiteral(":/icons/preferences/passwords.svg")));
    setCategoryIcon(8, QIcon(QStringLiteral(":/icons/preferences/privacy.svg")));
    setCategoryIcon(9, QIcon(QStringLiteral(":/icons/preferences/notifications.svg")));
    setCategoryIcon(10, QIcon(QStringLiteral(":/icons/preferences/extensions.svg")));
    setCategoryIcon(11, QIcon(QStringLiteral(":/icons/preferences/spellcheck.svg")));
    setCategoryIcon(12, QIcon(QStringLiteral(":/icons/preferences/other.svg")));

    Settings settings;
    //GENERAL URLs
    settings.beginGroup(QSL("Web-URL-Settings"));
    m_homepage = settings.value(QSL("homepage"), QUrl(QSL("falkon:start"))).toUrl();
    m_newTabUrl = settings.value(QSL("newTabUrl"), QUrl(QSL("falkon:speeddial"))).toUrl();
    ui->homepage->setText(QString::fromUtf8(m_homepage.toEncoded()));
    ui->newTabUrl->setText(QString::fromUtf8(m_newTabUrl.toEncoded()));
    settings.endGroup();
    ui->afterLaunch->setCurrentIndex(mApp->afterLaunch());
    ui->checkUpdates->setChecked(settings.value(QSL("Web-Browser-Settings/CheckUpdates"), true).toBool());
    ui->dontLoadTabsUntilSelected->setChecked(settings.value(QSL("Web-Browser-Settings/LoadTabsOnActivation"), true).toBool());

#if defined(Q_OS_WIN) && !defined(Q_OS_OS2)
    if (!mApp->isPortable()) {
        ui->checkDefaultBrowser->setChecked(settings.value(QSL("Web-Browser-Settings/CheckDefaultBrowser"),
                                                           DEFAULT_CHECK_DEFAULTBROWSER).toBool());
        if (mApp->associationManager()->isDefaultForAllCapabilities()) {
            ui->checkNowDefaultBrowser->setText(tr("Default"));
            ui->checkNowDefaultBrowser->setEnabled(false);
        }
        else {
            ui->checkNowDefaultBrowser->setText(tr("Set as default"));
            ui->checkNowDefaultBrowser->setEnabled(true);
            connect(ui->checkNowDefaultBrowser, SIGNAL(clicked()), this, SLOT(makeFalkonDefault()));
        }
    }
    else {
        ui->checkDefaultBrowser->hide();
        ui->checkNowDefaultBrowser->hide();
    }
#else // No Default Browser settings on non-Windows platform
    ui->hSpacerDefaultBrowser->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->hLayoutDefaultBrowser->invalidate();
    delete ui->hLayoutDefaultBrowser;
    delete ui->checkDefaultBrowser;
    delete ui->checkNowDefaultBrowser;
#endif

    ui->newTabFrame->setVisible(false);
    if (m_newTabUrl.isEmpty() || m_newTabUrl.toString() == QL1S("about:blank")) {
        ui->newTab->setCurrentIndex(0);
    }
    else if (m_newTabUrl == m_homepage) {
        ui->newTab->setCurrentIndex(1);
    }
    else if (m_newTabUrl.toString() == QL1S("falkon:speeddial")) {
        ui->newTab->setCurrentIndex(2);
    }
    else {
        ui->newTab->setCurrentIndex(3);
        ui->newTabFrame->setVisible(true);
    }

    afterLaunchChanged(ui->afterLaunch->currentIndex());
    connect(ui->afterLaunch, SIGNAL(currentIndexChanged(int)), this, SLOT(afterLaunchChanged(int)));
    connect(ui->newTab, SIGNAL(currentIndexChanged(int)), this, SLOT(newTabChanged(int)));
    if (m_window) {
        connect(ui->useCurrentBut, &QAbstractButton::clicked, this, &Preferences::useActualHomepage);
        connect(ui->newTabUseCurrent, &QAbstractButton::clicked, this, &Preferences::useActualNewTab);
    }
    else {
        ui->useCurrentBut->setEnabled(false);
        ui->newTabUseCurrent->setEnabled(false);
    }

    // PROFILES
    QString startingProfile = ProfileManager::startingProfile();
    ui->activeProfile->setText(QStringLiteral("<b>") + ProfileManager::currentProfile() + QStringLiteral("</b>"));
    ui->startProfile->addItem(startingProfile);

    const auto names = ProfileManager::availableProfiles();
    for (const QString &name : names) {
        if (startingProfile != name) {
            ui->startProfile->addItem(name);
        }
    }

    connect(ui->createProfile, &QAbstractButton::clicked, this, &Preferences::createProfile);
    connect(ui->deleteProfile, &QAbstractButton::clicked, this, &Preferences::deleteProfile);
    connect(ui->startProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(startProfileIndexChanged(int)));
    startProfileIndexChanged(ui->startProfile->currentIndex());

    //APPEREANCE
    settings.beginGroup(QSL("Browser-View-Settings"));
    ui->showStatusbar->setChecked(settings.value(QSL("showStatusBar"), false).toBool());
    // NOTE: instantBookmarksToolbar and showBookmarksToolbar cannot be both enabled at the same time
    ui->instantBookmarksToolbar->setChecked(settings.value(QSL("instantBookmarksToolbar"), false).toBool());
    ui->showBookmarksToolbar->setChecked(settings.value(QSL("showBookmarksToolbar"), false).toBool());
    ui->instantBookmarksToolbar->setDisabled(settings.value(QSL("showBookmarksToolbar"), false).toBool());
    ui->showBookmarksToolbar->setDisabled(settings.value(QSL("instantBookmarksToolbar")).toBool());
    connect(ui->instantBookmarksToolbar, &QAbstractButton::toggled, ui->showBookmarksToolbar, &QWidget::setDisabled);
    connect(ui->showBookmarksToolbar, &QAbstractButton::toggled, ui->instantBookmarksToolbar, &QWidget::setDisabled);
    ui->showNavigationToolbar->setChecked(settings.value(QSL("showNavigationToolbar"), true).toBool());
    int currentSettingsPage = settings.value(QSL("settingsDialogPage"), 0).toInt(nullptr);
    settings.endGroup();

    //TABS
    settings.beginGroup(QSL("Browser-Tabs-Settings"));
    ui->hideTabsOnTab->setChecked(settings.value(QSL("hideTabsWithOneTab"), false).toBool());
    ui->activateLastTab->setChecked(settings.value(QSL("ActivateLastTabWhenClosingActual"), false).toBool());
    ui->openNewTabAfterActive->setChecked(settings.value(QSL("newTabAfterActive"), true).toBool());
    ui->openNewEmptyTabAfterActive->setChecked(settings.value(QSL("newEmptyTabAfterActive"), false).toBool());
    ui->openPopupsInTabs->setChecked(settings.value(QSL("OpenPopupsInTabs"), false).toBool());
    ui->alwaysSwitchTabsWithWheel->setChecked(settings.value(QSL("AlwaysSwitchTabsWithWheel"), false).toBool());
    ui->switchToNewTabs->setChecked(settings.value(QSL("OpenNewTabsSelected"), false).toBool());
    ui->dontCloseOnLastTab->setChecked(settings.value(QSL("dontCloseWithOneTab"), false).toBool());
    ui->askWhenClosingMultipleTabs->setChecked(settings.value(QSL("AskOnClosing"), false).toBool());
    ui->showClosedTabsButton->setChecked(settings.value(QSL("showClosedTabsButton"), false).toBool());
    ui->showCloseOnInactive->setCurrentIndex(settings.value(QSL("showCloseOnInactiveTabs"), 0).toInt());
    settings.endGroup();

    //AddressBar
    settings.beginGroup(QSL("AddressBar"));
    ui->addressbarCompletion->setCurrentIndex(settings.value(QSL("showSuggestions"), 0).toInt());
    ui->useInlineCompletion->setChecked(settings.value(QSL("useInlineCompletion"), true).toBool());
    ui->completionShowSwitchTab->setChecked(settings.value(QSL("showSwitchTab"), true).toBool());
    ui->alwaysShowGoIcon->setChecked(settings.value(QSL("alwaysShowGoIcon"), false).toBool());
    ui->showZoomLabel->setChecked(settings.value(QSL("showZoomLabel"), true).toBool());
    ui->selectAllOnFocus->setChecked(settings.value(QSL("SelectAllTextOnDoubleClick"), true).toBool());
    ui->selectAllOnClick->setChecked(settings.value(QSL("SelectAllTextOnClick"), false).toBool());
    ui->completionPopupExpandToWindow->setChecked(settings.value(QSL("CompletionPopupExpandToWindow"), false).toBool());
    bool showPBinAB = settings.value(QSL("ShowLoadingProgress"), false).toBool();
    ui->showLoadingInAddressBar->setChecked(showPBinAB);
    ui->adressProgressSettings->setEnabled(showPBinAB);
    ui->progressStyleSelector->setCurrentIndex(settings.value(QSL("ProgressStyle"), 0).toInt());
    bool pbInABuseCC = settings.value(QSL("UseCustomProgressColor"), false).toBool();
    ui->checkBoxCustomProgressColor->setChecked(pbInABuseCC);
    ui->progressBarColorSelector->setEnabled(pbInABuseCC);
    QColor pbColor = settings.value(QSL("CustomProgressColor"), palette().color(QPalette::Highlight)).value<QColor>();
    setProgressBarColorIcon(pbColor);
    connect(ui->customColorToolButton, &QAbstractButton::clicked, this, &Preferences::selectCustomProgressBarColor);
    connect(ui->resetProgressBarcolor, SIGNAL(clicked()), SLOT(setProgressBarColorIcon()));
    settings.endGroup();

    settings.beginGroup(QSL("SearchEngines"));
    bool searchFromAB = settings.value(QSL("SearchFromAddressBar"), true).toBool();
    ui->searchFromAddressBar->setChecked(searchFromAB);
    ui->searchWithDefaultEngine->setEnabled(searchFromAB);
    ui->searchWithDefaultEngine->setChecked(settings.value(QSL("SearchWithDefaultEngine"), true).toBool());
    ui->showABSearchSuggestions->setEnabled(searchFromAB);
    ui->showABSearchSuggestions->setChecked(settings.value(QSL("showSearchSuggestions"), true).toBool());
    connect(ui->searchFromAddressBar, &QAbstractButton::toggled, this, &Preferences::searchFromAddressBarChanged);
    settings.endGroup();

    // BROWSING
    settings.beginGroup(QSL("Web-Browser-Settings"));
    ui->allowPlugins->setChecked(settings.value(QSL("allowPlugins"), true).toBool());
    connect(ui->allowPlugins, &QAbstractButton::toggled, this, &Preferences::allowPluginsToggled);
    ui->allowJavaScript->setChecked(settings.value(QSL("allowJavaScript"), true).toBool());
    ui->linksInFocusChain->setChecked(settings.value(QSL("IncludeLinkInFocusChain"), false).toBool());
    ui->spatialNavigation->setChecked(settings.value(QSL("SpatialNavigation"), false).toBool());
    ui->animateScrolling->setChecked(settings.value(QSL("AnimateScrolling"), true).toBool());
    ui->wheelScroll->setValue(settings.value(QSL("wheelScrollLines"), qApp->wheelScrollLines()).toInt());
    ui->xssAuditing->setChecked(settings.value(QSL("XSSAuditing"), false).toBool());
    ui->printEBackground->setChecked(settings.value(QSL("PrintElementBackground"), true).toBool());
    ui->useNativeScrollbars->setChecked(settings.value(QSL("UseNativeScrollbars"), false).toBool());
    ui->disableVideoAutoPlay->setChecked(settings.value(QSL("DisableVideoAutoPlay"), false).toBool());
    ui->webRTCPublicIpOnly->setChecked(settings.value(QSL("WebRTCPublicIpOnly"), true).toBool());
    ui->dnsPrefetch->setChecked(settings.value(QSL("DNSPrefetch"), true).toBool());
    ui->intPDFViewer->setChecked(settings.value(QSL("intPDFViewer"), false).toBool());
    ui->intPDFViewer->setEnabled(ui->allowPlugins->isChecked());
    ui->screenCaptureEnabled->setChecked(settings.value(QSL("screenCaptureEnabled"), false).toBool());
    ui->hardwareAccel->setChecked(settings.value(QSL("hardwareAccel"), false).toBool());

    const auto levels = WebView::zoomLevels();
    for (int level : levels) {
        ui->defaultZoomLevel->addItem(tr("%1%").arg(QString::number(level)));
    }
    ui->defaultZoomLevel->setCurrentIndex(settings.value(QSL("DefaultZoomLevel"), WebView::zoomLevels().indexOf(100)).toInt());
    ui->closeAppWithCtrlQ->setChecked(settings.value(QSL("closeAppWithCtrlQ"), true).toBool());

    auto* siteSettings = mApp->siteSettingsManager();
    const auto supportedAttribute = mApp->siteSettingsManager()->getSupportedAttribute();
    for (const auto &attribute : supportedAttribute) {
        auto* listItem = new QListWidgetItem(ui->siteSettingsList);
        auto* optionItem = new SiteInfoPermissionDefaultItem(siteSettings->getDefaultPermission(attribute), this);

        ui->siteSettingsList->setItemWidget(listItem, optionItem);
        listItem->setSizeHint(optionItem->sizeHint());
        optionItem->setAttribute(attribute);
    }
    const auto supportedFeatures = mApp->siteSettingsManager()->getSupportedFeatures();
    for (const auto &feature : supportedFeatures) {
        auto* listItem = new QListWidgetItem(ui->siteSettingsList);
        auto* optionItem = new SiteInfoPermissionDefaultItem(siteSettings->getDefaultPermission(feature), this);

        ui->siteSettingsList->setItemWidget(listItem, optionItem);
        listItem->setSizeHint(optionItem->sizeHint());
        optionItem->setFeature(feature);
    }
    {
        auto* listItem = new QListWidgetItem(ui->siteSettingsList);
        auto* optionItem = new SiteInfoPermissionDefaultItem(siteSettings->getDefaultPermission(SiteSettingsManager::poAllowCookies), this);

        ui->siteSettingsList->setItemWidget(listItem, optionItem);
        listItem->setSizeHint(optionItem->sizeHint());
        optionItem->setOption(SiteSettingsManager::poAllowCookies);
    }

    //Cache
    ui->allowCache->setChecked(settings.value(QSL("AllowLocalCache"), true).toBool());
    ui->removeCache->setChecked(settings.value(QSL("deleteCacheOnClose"), false).toBool());
    ui->cacheMB->setValue(settings.value(QSL("LocalCacheSize"), 50).toInt());
    ui->cachePath->setText(settings.value(QSL("CachePath"), mApp->webProfile()->cachePath()).toString());
    connect(ui->allowCache, &QAbstractButton::clicked, this, &Preferences::allowCacheChanged);
    connect(ui->changeCachePath, &QAbstractButton::clicked, this, &Preferences::changeCachePathClicked);
    allowCacheChanged(ui->allowCache->isChecked());

    //PASSWORD MANAGER
    ui->allowPassManager->setChecked(settings.value(QSL("SavePasswordsOnSites"), true).toBool());
    ui->autoCompletePasswords->setChecked(settings.value(QSL("AutoCompletePasswords"), true).toBool());

    //PRIVACY
    //Web storage
    ui->saveHistory->setChecked(settings.value(QSL("allowHistory"), true).toBool());
    ui->deleteHistoryOnClose->setChecked(settings.value(QSL("deleteHistoryOnClose"), false).toBool());
    if (!ui->saveHistory->isChecked()) {
        ui->deleteHistoryOnClose->setEnabled(false);
    }
    connect(ui->saveHistory, &QAbstractButton::toggled, this, &Preferences::saveHistoryChanged);

    // Html5Storage
    ui->html5storage->setChecked(settings.value(QSL("HTML5StorageEnabled"), true).toBool());
    ui->deleteHtml5storageOnClose->setChecked(settings.value(QSL("deleteHTML5StorageOnClose"), false).toBool());
    connect(ui->html5storage, &QAbstractButton::toggled, this, &Preferences::allowHtml5storageChanged);
    // Other
    ui->doNotTrack->setChecked(settings.value(QSL("DoNotTrack"), false).toBool());

    //CSS Style
    ui->userStyleSheet->setText(settings.value(QSL("userStyleSheet"), QSL("")).toString());
    connect(ui->chooseUserStylesheet, &QAbstractButton::clicked, this, &Preferences::chooseUserStyleClicked);
    settings.endGroup();

    //DOWNLOADS
    settings.beginGroup(QSL("DownloadManager"));
    ui->downLoc->setText(settings.value(QSL("defaultDownloadPath"), QSL("")).toString());
    ui->closeDownManOnFinish->setChecked(settings.value(QSL("CloseManagerOnFinish"), false).toBool());
    if (ui->downLoc->text().isEmpty()) {
        ui->askEverytime->setChecked(true);
    }
    else {
        ui->useDefined->setChecked(true);
    }
    ui->useExternalDownManager->setChecked(settings.value(QSL("UseExternalManager"), false).toBool());
    ui->externalDownExecutable->setText(settings.value(QSL("ExternalManagerExecutable"), QSL("")).toString());
    ui->externalDownArguments->setText(settings.value(QSL("ExternalManagerArguments"), QSL("")).toString());

    connect(ui->useExternalDownManager, &QAbstractButton::toggled, this, &Preferences::useExternalDownManagerChanged);


    connect(ui->useDefined, &QAbstractButton::toggled, this, &Preferences::downLocChanged);
    connect(ui->downButt, &QAbstractButton::clicked, this, &Preferences::chooseDownPath);
    connect(ui->chooseExternalDown, &QAbstractButton::clicked, this, &Preferences::chooseExternalDownloadManager);
    downLocChanged(ui->useDefined->isChecked());
    useExternalDownManagerChanged(ui->useExternalDownManager->isChecked());
    settings.endGroup();

    //FONTS
    settings.beginGroup(QSL("Browser-Fonts"));
    QWebEngineSettings* webSettings = mApp->webSettings();
    auto defaultFont = [&](QWebEngineSettings::FontFamily font) -> const QString {
        const QString family = webSettings->fontFamily(font);
        if (!family.isEmpty())
            return family;
        switch (font) {
        case QWebEngineSettings::FixedFont:
            return QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
        case QWebEngineSettings::SerifFont:
            // TODO
        default:
            return QFontDatabase::systemFont(QFontDatabase::GeneralFont).family();
        }
    };
    ui->fontStandard->setCurrentFont(QFont(settings.value(QSL("StandardFont"), defaultFont(QWebEngineSettings::StandardFont)).toString()));
    ui->fontCursive->setCurrentFont(QFont(settings.value(QSL("CursiveFont"), defaultFont(QWebEngineSettings::CursiveFont)).toString()));
    ui->fontFantasy->setCurrentFont(QFont(settings.value(QSL("FantasyFont"), defaultFont(QWebEngineSettings::FantasyFont)).toString()));
    ui->fontFixed->setCurrentFont(QFont(settings.value(QSL("FixedFont"), defaultFont(QWebEngineSettings::FixedFont)).toString()));
    ui->fontSansSerif->setCurrentFont(QFont(settings.value(QSL("SansSerifFont"), defaultFont(QWebEngineSettings::SansSerifFont)).toString()));
    ui->fontSerif->setCurrentFont(QFont(settings.value(QSL("SerifFont"), defaultFont(QWebEngineSettings::SerifFont)).toString()));
    ui->sizeDefault->setValue(settings.value(QSL("DefaultFontSize"), webSettings->fontSize(QWebEngineSettings::DefaultFontSize)).toInt());
    ui->sizeFixed->setValue(settings.value(QSL("FixedFontSize"), webSettings->fontSize(QWebEngineSettings::DefaultFixedFontSize)).toInt());
    ui->sizeMinimum->setValue(settings.value(QSL("MinimumFontSize"), webSettings->fontSize(QWebEngineSettings::MinimumFontSize)).toInt());
    ui->sizeMinimumLogical->setValue(settings.value(QSL("MinimumLogicalFontSize"), webSettings->fontSize(QWebEngineSettings::MinimumLogicalFontSize)).toInt());
    settings.endGroup();

    //KEYBOARD SHORTCUTS
    settings.beginGroup(QSL("Shortcuts"));
    ui->switchTabsAlt->setChecked(settings.value(QSL("useTabNumberShortcuts"), true).toBool());
    ui->loadSpeedDialsCtrl->setChecked(settings.value(QSL("useSpeedDialNumberShortcuts"), true).toBool());
    ui->singleKeyShortcuts->setChecked(settings.value(QSL("useSingleKeyShortcuts"), false).toBool());
    settings.endGroup();

    //NOTIFICATIONS
    ui->useNativeSystemNotifications->setEnabled(mApp->desktopNotifications()->supportsNativeNotifications());

    DesktopNotificationsFactory::Type notifyType;
    settings.beginGroup(QSL("Notifications"));
    ui->notificationTimeout->setValue(settings.value(QSL("Timeout"), 6000).toInt() / 1000);
#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
    notifyType = settings.value(QSL("UseNativeDesktop"), true).toBool() ? DesktopNotificationsFactory::DesktopNative : DesktopNotificationsFactory::PopupWidget;
#else
    notifyType = DesktopNotificationsFactory::PopupWidget;
#endif
    if (ui->useNativeSystemNotifications->isEnabled() && notifyType == DesktopNotificationsFactory::DesktopNative) {
        ui->useNativeSystemNotifications->setChecked(true);
    }
    else {
        ui->useOSDNotifications->setChecked(true);
    }

    connect(ui->notificationPreview, &QPushButton::clicked, this, &Preferences::showNotificationPreview);

    ui->doNotUseNotifications->setChecked(!settings.value(QSL("Enabled"), true).toBool());
    m_notifPosition = settings.value(QSL("Position"), QPoint(10, 10)).toPoint();
    settings.endGroup();

    //SPELLCHECK
    settings.beginGroup(QSL("SpellCheck"));
    ui->spellcheckEnabled->setChecked(settings.value(QSL("Enabled"), false).toBool());
    const QStringList spellcheckLanguages = settings.value(QSL("Languages")).toStringList();
    settings.endGroup();

    auto updateSpellCheckEnabled = [this]() {
        ui->spellcheckLanguages->setEnabled(ui->spellcheckEnabled->isChecked());
        ui->spellcheckNoLanguages->setEnabled(ui->spellcheckEnabled->isChecked());
    };
    updateSpellCheckEnabled();
    connect(ui->spellcheckEnabled, &QCheckBox::toggled, this, updateSpellCheckEnabled);

    QStringList dictionariesDirs = {};
    const QByteArray qtWeDictionariesDirs = qgetenv("QTWEBENGINE_DICTIONARIES_PATH");
    if (!qtWeDictionariesDirs.isNull()) {
        dictionariesDirs.append(QDir::cleanPath(QString::fromLocal8Bit(qtWeDictionariesDirs)));
    }
    else {
#ifdef Q_OS_OSX
        dictionariesDirs.append(QDir::cleanPath(QCoreApplication::applicationDirPath() + QL1S("/../Resources/qtwebengine_dictionaries")));
        dictionariesDirs.append(QDir::cleanPath(QCoreApplication::applicationDirPath() + QL1S("/../Frameworks/QtWebEngineCore.framework/Resources/qtwebengine_dictionaries")));
#else
        dictionariesDirs.append(QDir::cleanPath(QCoreApplication::applicationDirPath() + QL1S("/qtwebengine_dictionaries")));
        dictionariesDirs.append(QDir::cleanPath(QLibraryInfo::location(QLibraryInfo::DataPath) + QL1S("/qtwebengine_dictionaries")));
#endif
    }
    dictionariesDirs.removeDuplicates();

    ui->spellcheckDirectories->setText(dictionariesDirs.join(QL1C('\n')));

    for (const QString &path : dictionariesDirs) {
        QDir dir(path);
        const QStringList files = dir.entryList({QSL("*.bdic")});
        for (const QString &file : files) {
            const QString lang = file.left(file.size() - 5);
            const QString langName = createLanguageItem(lang);
            if (!ui->spellcheckLanguages->findItems(langName, Qt::MatchExactly).isEmpty()) {
                continue;
            }
            auto *item = new QListWidgetItem;
            item->setText(langName);
            item->setData(Qt::UserRole, lang);
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            item->setCheckState(Qt::Unchecked);
            ui->spellcheckLanguages->addItem(item);
        }
    }

    int topIndex = 0;
    for (const QString &lang : spellcheckLanguages) {
        const auto items = ui->spellcheckLanguages->findItems(createLanguageItem(lang), Qt::MatchExactly);
        if (items.isEmpty()) {
            continue;
        }
        QListWidgetItem *item = items.at(0);
        ui->spellcheckLanguages->takeItem(ui->spellcheckLanguages->row(item));
        ui->spellcheckLanguages->insertItem(topIndex++, item);
        item->setCheckState(Qt::Checked);
    }

    if (ui->spellcheckLanguages->count() == 0) {
        ui->spellcheckLanguages->hide();
    } else {
        ui->spellcheckNoLanguages->hide();
    }

    // Proxy Configuration
    settings.beginGroup(QSL("Web-Proxy"));
    int proxyType = settings.value(QSL("ProxyType"), 2).toInt();
    if (proxyType == 0) {
        ui->noProxy->setChecked(true);
    } else if (proxyType == 2) {
        ui->systemProxy->setChecked(true);
    } else if (proxyType == 3) {
        ui->manualProxy->setChecked(true);
        ui->proxyType->setCurrentIndex(0);
    } else {
        ui->manualProxy->setChecked(true);
        ui->proxyType->setCurrentIndex(1);
    }

    ui->proxyServer->setText(settings.value(QSL("HostName"), QSL("")).toString());
    ui->proxyPort->setText(settings.value(QSL("Port"), 8080).toString());
    ui->proxyUsername->setText(settings.value(QSL("Username"), QSL("")).toString());
    ui->proxyPassword->setText(settings.value(QSL("Password"), QSL("")).toString());
    settings.endGroup();

    setManualProxyConfigurationEnabled(ui->manualProxy->isChecked());
    connect(ui->manualProxy, &QAbstractButton::toggled, this, &Preferences::setManualProxyConfigurationEnabled);

    //CONNECTS
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &Preferences::buttonClicked);
    connect(ui->cookieManagerBut, &QAbstractButton::clicked, this, &Preferences::showCookieManager);
    connect(ui->html5permissions, &QAbstractButton::clicked, this, &Preferences::showHtml5Permissions);
    connect(ui->preferredLanguages, &QAbstractButton::clicked, this, &Preferences::showAcceptLanguage);
    connect(ui->deleteHtml5storage, &QAbstractButton::clicked, this, &Preferences::deleteHtml5storage);
    connect(ui->uaManager, &QAbstractButton::clicked, this, &Preferences::openUserAgentManager);
    connect(ui->jsOptionsButton, &QAbstractButton::clicked, this, &Preferences::openJsOptions);
    connect(ui->searchEngines, &QAbstractButton::clicked, this, &Preferences::openSearchEnginesManager);
    connect(ui->certificateManager, &QAbstractButton::clicked, this, &Preferences::openCertificateManager);
    connect(ui->protocolHandlers, &QAbstractButton::clicked, this, &Preferences::openProtocolHandlersManager);
    connect(ui->customSchemes, &QAbstractButton::clicked, this, &Preferences::openSchemesManager);

    connect(ui->listWidget, &QListWidget::currentItemChanged, this, &Preferences::showStackedPage);
    ui->listWidget->itemAt(5, 5)->setSelected(true);

    ui->listWidget->setCurrentRow(currentSettingsPage);

    QSize s = size();
    const QRect &availableGeometryForScreen = screen()->availableGeometry();
    if (availableGeometryForScreen.size().width() < s.width()) {
        s.setWidth(availableGeometryForScreen.size().width() - 50);
    }
    if (availableGeometryForScreen.size().height() < s.height()) {
        s.setHeight(availableGeometryForScreen.size().height() - 50);
    }
    resize(s);

    settings.beginGroup(QSL("Preferences"));
    restoreGeometry(settings.value(QSL("Geometry")).toByteArray());
    settings.endGroup();

    QzTools::setWmClass(QSL("Preferences"), this);
}

void Preferences::allowPluginsToggled(bool checked)
{
    ui->intPDFViewer->setEnabled(checked);
}

void Preferences::chooseExternalDownloadManager()
{
    QString path = QzTools::getOpenFileName(QSL("Preferences-ExternalDownloadManager"), this, tr("Choose executable location..."), QDir::homePath());
    if (path.isEmpty()) {
        return;
    }

    ui->externalDownExecutable->setText(path);
}

void Preferences::showStackedPage(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    int index = ui->listWidget->currentRow();

    ui->caption->setText(QSL("<b>") + item->text() + QSL("</b>"));
    ui->stackedWidget->setCurrentIndex(index);

    if (m_notification) {
        m_notifPosition = m_notification.data()->pos();
        delete m_notification.data();
    }

    if (index == 10) {
        m_pluginsList->load();
    }

    if (index == 7 && !m_autoFillManager) {
        m_autoFillManager = new AutoFillManager(this);
        ui->autoFillFrame->addWidget(m_autoFillManager);
    }
}

void Preferences::showNotificationPreview()
{
    if (ui->useOSDNotifications->isChecked()) {
        if (m_notification) {
            m_notifPosition = m_notification.data()->pos();
            delete m_notification.data();
        }

        m_notification = new DesktopNotification(true);
        m_notification.data()->setHeading(tr("OSD Notification"));
        m_notification.data()->setText(tr("Drag it on the screen to place it where you want."));
        m_notification.data()->move(m_notifPosition);
        m_notification.data()->show();
    }
    else if (ui->useNativeSystemNotifications->isChecked()) {
        mApp->desktopNotifications()->nativeNotificationPreview();
    }
}

void Preferences::makeFalkonDefault()
{
#if defined(Q_OS_WIN) && !defined(Q_OS_OS2)
    disconnect(ui->checkNowDefaultBrowser, SIGNAL(clicked()), this, SLOT(makeFalkonDefault()));
    ui->checkNowDefaultBrowser->setText(tr("Default"));
    ui->checkNowDefaultBrowser->setEnabled(false);

    if (!mApp->associationManager()->showNativeDefaultAppSettingsUi())
        mApp->associationManager()->registerAllAssociation();
#endif
}

void Preferences::allowCacheChanged(bool state)
{
    ui->removeCache->setEnabled(state);
    ui->maxCacheLabel->setEnabled(state);
    ui->cacheMB->setEnabled(state);
    ui->storeCacheLabel->setEnabled(state);
    ui->cachePath->setEnabled(state);
    ui->changeCachePath->setEnabled(state);
}

void Preferences::useActualHomepage()
{
    if (!m_window)
        return;

    ui->homepage->setText(m_window->weView()->url().toString());
}

void Preferences::useActualNewTab()
{
    if (!m_window)
        return;

    ui->newTabUrl->setText(m_window->weView()->url().toString());
}

void Preferences::chooseDownPath()
{
    QString userFileName = QzTools::getExistingDirectory(QSL("Preferences-ChooseDownPath"), this, tr("Choose download location..."), QDir::homePath());
    if (userFileName.isEmpty()) {
        return;
    }
#ifdef Q_OS_WIN   //QFileDialog::getExistingDirectory returns path with \ instead of / (??)
    userFileName.replace(QLatin1Char('\\'), QLatin1Char('/'));
#endif
    userFileName += QLatin1Char('/');

    ui->downLoc->setText(userFileName);
}

void Preferences::chooseUserStyleClicked()
{
    QString file = QzTools::getOpenFileName(QSL("Preferences-UserStyle"), this, tr("Choose stylesheet location..."), QDir::homePath(), QSL("*.css"));
    if (file.isEmpty()) {
        return;
    }
    ui->userStyleSheet->setText(file);
}

void Preferences::deleteHtml5storage()
{
    ClearPrivateData::clearLocalStorage();

    ui->deleteHtml5storage->setText(tr("Deleted"));
    ui->deleteHtml5storage->setEnabled(false);
}

void Preferences::openUserAgentManager()
{
    auto* dialog = new UserAgentDialog(this);
    dialog->open();
}

void Preferences::downLocChanged(bool state)
{
    ui->downButt->setEnabled(state);
    ui->downLoc->setEnabled(state);
}

void Preferences::setManualProxyConfigurationEnabled(bool state)
{
    ui->proxyType->setEnabled(state);
    ui->proxyServer->setEnabled(state);
    ui->proxyPort->setEnabled(state);
    ui->proxyUsername->setEnabled(state);
    ui->proxyPassword->setEnabled(state);
}

void Preferences::searchFromAddressBarChanged(bool stat)
{
    ui->searchWithDefaultEngine->setEnabled(stat);
    ui->showABSearchSuggestions->setEnabled(stat);
}

void Preferences::saveHistoryChanged(bool stat)
{
    ui->deleteHistoryOnClose->setEnabled(stat);
}

void Preferences::allowHtml5storageChanged(bool stat)
{
    ui->deleteHtml5storageOnClose->setEnabled(stat);
}

void Preferences::showCookieManager()
{
    auto* dialog = new CookieManager(this);
    dialog->show();
}

void Preferences::showHtml5Permissions()
{
    auto* dialog = new HTML5PermissionsDialog(this);
    dialog->open();
}

void Preferences::openJsOptions()
{
    auto* dialog = new JsOptions(this);
    dialog->open();
}

void Preferences::useExternalDownManagerChanged(bool state)
{
    ui->externalDownExecutable->setEnabled(state);
    ui->externalDownArguments->setEnabled(state);
    ui->chooseExternalDown->setEnabled(state);
}

void Preferences::openSearchEnginesManager()
{
    auto* dialog = new SearchEnginesDialog(this);
    dialog->open();
}

void Preferences::openCertificateManager()
{
    auto *dialog = new CertificateManager(this);
    dialog->open();
}

void Preferences::openProtocolHandlersManager()
{
    auto *dialog = new ProtocolHandlerDialog(this);
    dialog->open();
}

void Preferences::openSchemesManager()
{
    auto *dialog = new SchemeManager(this);
    dialog->open();
}

void Preferences::showAcceptLanguage()
{
    auto* dialog = new AcceptLanguage(this);
    dialog->open();
}

void Preferences::newTabChanged(int value)
{
    ui->newTabFrame->setVisible(value == 3);
}

void Preferences::afterLaunchChanged(int value)
{
    ui->dontLoadTabsUntilSelected->setEnabled(value == 3 || value == 4);
}

void Preferences::changeCachePathClicked()
{
    QString path = QzTools::getExistingDirectory(QSL("Preferences-CachePath"), this, tr("Choose cache path..."), ui->cachePath->text());
    if (path.isEmpty()) {
        return;
    }

    ui->cachePath->setText(path);
}

void Preferences::buttonClicked(QAbstractButton* button)
{
    switch (ui->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::ApplyRole:
        saveSettings();
        break;

    case QDialogButtonBox::RejectRole:
        close();
        break;

    case QDialogButtonBox::AcceptRole:
        saveSettings();
        close();
        break;

    default:
        break;
    }
}

void Preferences::createProfile()
{
    QString name = QInputDialog::getText(this, tr("New Profile"), tr("Enter the new profile's name:"));
    name = QzTools::filterCharsFromFilename(name);

    if (name.isEmpty()) {
        return;
    }

    int res = ProfileManager::createProfile(name);

    if (res == -1) {
        QMessageBox::warning(this, tr("Error!"), tr("This profile already exists!"));
        return;
    }

    if (res != 0) {
        QMessageBox::warning(this, tr("Error!"), tr("Cannot create profile directory!"));
        return;
    }

    ui->startProfile->addItem(name);
    ui->startProfile->setCurrentIndex(ui->startProfile->count() - 1);
}

void Preferences::deleteProfile()
{
    QString name = ui->startProfile->currentText();
    QMessageBox::StandardButton button = QMessageBox::warning(this, tr("Confirmation"),
                                         tr("Are you sure you want to permanently delete \"%1\" profile? This action cannot be undone!").arg(name), QMessageBox::Yes | QMessageBox::No);
    if (button != QMessageBox::Yes) {
        return;
    }

    ProfileManager::removeProfile(name);

    ui->startProfile->removeItem(ui->startProfile->currentIndex());
}

void Preferences::startProfileIndexChanged(int index)
{
    const bool current = ui->startProfile->itemText(index) == ProfileManager::currentProfile();

    ui->deleteProfile->setEnabled(!current);
    ui->cannotDeleteActiveProfileLabel->setText(current ? tr("Note: You cannot delete active profile.") : QString());
}

void Preferences::closeEvent(QCloseEvent* event)
{
    Settings settings;
    settings.beginGroup(QSL("Browser-View-Settings"));
    settings.setValue(QSL("settingsDialogPage"), ui->stackedWidget->currentIndex());
    settings.endGroup();

    event->accept();
}

void Preferences::saveSettings()
{
    Settings settings;
    //GENERAL URLs
    QUrl homepage = QUrl::fromUserInput(ui->homepage->text());

    settings.beginGroup(QSL("Web-URL-Settings"));
    settings.setValue(QSL("homepage"), homepage);
    settings.setValue(QSL("afterLaunch"), ui->afterLaunch->currentIndex());

    switch (ui->newTab->currentIndex()) {
    case 0:
        settings.setValue(QSL("newTabUrl"), QUrl());
        break;

    case 1:
        settings.setValue(QSL("newTabUrl"), homepage);
        break;

    case 2:
        settings.setValue(QSL("newTabUrl"), QUrl(QSL("falkon:speeddial")));
        break;

    case 3:
        settings.setValue(QSL("newTabUrl"), QUrl::fromUserInput(ui->newTabUrl->text()));
        break;

    default:
        break;
    }

    settings.endGroup();
    //PROFILES
    /*
     *
     *
     *
     */

    //WINDOW
    settings.beginGroup(QSL("Browser-View-Settings"));
    settings.setValue(QSL("showStatusBar"), ui->showStatusbar->isChecked());
    settings.setValue(QSL("instantBookmarksToolbar"), ui->instantBookmarksToolbar->isChecked());
    settings.setValue(QSL("showBookmarksToolbar"), ui->showBookmarksToolbar->isChecked());
    settings.setValue(QSL("showNavigationToolbar"), ui->showNavigationToolbar->isChecked());
    settings.endGroup();

    //TABS
    settings.beginGroup(QSL("Browser-Tabs-Settings"));
    settings.setValue(QSL("hideTabsWithOneTab"), ui->hideTabsOnTab->isChecked());
    settings.setValue(QSL("ActivateLastTabWhenClosingActual"), ui->activateLastTab->isChecked());
    settings.setValue(QSL("newTabAfterActive"), ui->openNewTabAfterActive->isChecked());
    settings.setValue(QSL("newEmptyTabAfterActive"), ui->openNewEmptyTabAfterActive->isChecked());
    settings.setValue(QSL("OpenPopupsInTabs"), ui->openPopupsInTabs->isChecked());
    settings.setValue(QSL("AlwaysSwitchTabsWithWheel"), ui->alwaysSwitchTabsWithWheel->isChecked());
    settings.setValue(QSL("OpenNewTabsSelected"), ui->switchToNewTabs->isChecked());
    settings.setValue(QSL("dontCloseWithOneTab"), ui->dontCloseOnLastTab->isChecked());
    settings.setValue(QSL("AskOnClosing"), ui->askWhenClosingMultipleTabs->isChecked());
    settings.setValue(QSL("showClosedTabsButton"), ui->showClosedTabsButton->isChecked());
    settings.setValue(QSL("showCloseOnInactiveTabs"), ui->showCloseOnInactive->currentIndex());
    settings.endGroup();

    //DOWNLOADS
    settings.beginGroup(QSL("DownloadManager"));
    if (ui->askEverytime->isChecked()) {
        settings.setValue(QSL("defaultDownloadPath"), QSL(""));
    }
    else {
        settings.setValue(QSL("defaultDownloadPath"), ui->downLoc->text());
    }
    settings.setValue(QSL("CloseManagerOnFinish"), ui->closeDownManOnFinish->isChecked());
    settings.setValue(QSL("UseExternalManager"), ui->useExternalDownManager->isChecked());
    settings.setValue(QSL("ExternalManagerExecutable"), ui->externalDownExecutable->text());
    settings.setValue(QSL("ExternalManagerArguments"), ui->externalDownArguments->text());

    settings.endGroup();

    //FONTS
    settings.beginGroup(QSL("Browser-Fonts"));
    settings.setValue(QSL("StandardFont"), ui->fontStandard->currentFont().family());
    settings.setValue(QSL("CursiveFont"), ui->fontCursive->currentFont().family());
    settings.setValue(QSL("FantasyFont"), ui->fontFantasy->currentFont().family());
    settings.setValue(QSL("FixedFont"), ui->fontFixed->currentFont().family());
    settings.setValue(QSL("SansSerifFont"), ui->fontSansSerif->currentFont().family());
    settings.setValue(QSL("SerifFont"), ui->fontSerif->currentFont().family());

    settings.setValue(QSL("DefaultFontSize"), ui->sizeDefault->value());
    settings.setValue(QSL("FixedFontSize"), ui->sizeFixed->value());
    settings.setValue(QSL("MinimumFontSize"), ui->sizeMinimum->value());
    settings.setValue(QSL("MinimumLogicalFontSize"), ui->sizeMinimumLogical->value());
    settings.endGroup();

    //KEYBOARD SHORTCUTS
    settings.beginGroup(QSL("Shortcuts"));
    settings.setValue(QSL("useTabNumberShortcuts"), ui->switchTabsAlt->isChecked());
    settings.setValue(QSL("useSpeedDialNumberShortcuts"), ui->loadSpeedDialsCtrl->isChecked());
    settings.setValue(QSL("useSingleKeyShortcuts"), ui->singleKeyShortcuts->isChecked());
    settings.endGroup();

    //BROWSING
    settings.beginGroup(QSL("Web-Browser-Settings"));
    settings.setValue(QSL("allowPlugins"), ui->allowPlugins->isChecked());
    settings.setValue(QSL("allowJavaScript"), ui->allowJavaScript->isChecked());
    settings.setValue(QSL("IncludeLinkInFocusChain"), ui->linksInFocusChain->isChecked());
    settings.setValue(QSL("SpatialNavigation"), ui->spatialNavigation->isChecked());
    settings.setValue(QSL("AnimateScrolling"), ui->animateScrolling->isChecked());
    settings.setValue(QSL("wheelScrollLines"), ui->wheelScroll->value());
    settings.setValue(QSL("DoNotTrack"), ui->doNotTrack->isChecked());
    settings.setValue(QSL("CheckUpdates"), ui->checkUpdates->isChecked());
    settings.setValue(QSL("LoadTabsOnActivation"), ui->dontLoadTabsUntilSelected->isChecked());
    settings.setValue(QSL("DefaultZoomLevel"), ui->defaultZoomLevel->currentIndex());
    settings.setValue(QSL("XSSAuditing"), ui->xssAuditing->isChecked());
    settings.setValue(QSL("PrintElementBackground"), ui->printEBackground->isChecked());
    settings.setValue(QSL("closeAppWithCtrlQ"), ui->closeAppWithCtrlQ->isChecked());
    settings.setValue(QSL("UseNativeScrollbars"), ui->useNativeScrollbars->isChecked());
    settings.setValue(QSL("DisableVideoAutoPlay"), ui->disableVideoAutoPlay->isChecked());
    settings.setValue(QSL("WebRTCPublicIpOnly"), ui->webRTCPublicIpOnly->isChecked());
    settings.setValue(QSL("DNSPrefetch"), ui->dnsPrefetch->isChecked());
    settings.setValue(QSL("intPDFViewer"), ui->intPDFViewer->isChecked());
    settings.setValue(QSL("screenCaptureEnabled"), ui->screenCaptureEnabled->isChecked());
    settings.setValue(QSL("hardwareAccel"), ui->hardwareAccel->isChecked());
#ifdef Q_OS_WIN
    settings.setValue(QSL("CheckDefaultBrowser"), ui->checkDefaultBrowser->isChecked());
#endif
    //Cache
    settings.setValue(QSL("AllowLocalCache"), ui->allowCache->isChecked());
    settings.setValue(QSL("deleteCacheOnClose"), ui->removeCache->isChecked());
    settings.setValue(QSL("LocalCacheSize"), ui->cacheMB->value());
    settings.setValue(QSL("CachePath"), ui->cachePath->text());
    //CSS Style
    settings.setValue(QSL("userStyleSheet"), ui->userStyleSheet->text());

    //PASSWORD MANAGER
    settings.setValue(QSL("SavePasswordsOnSites"), ui->allowPassManager->isChecked());
    settings.setValue(QSL("AutoCompletePasswords"), ui->autoCompletePasswords->isChecked());

    //PRIVACY
    //Web storage
    settings.setValue(QSL("allowHistory"), ui->saveHistory->isChecked());
    settings.setValue(QSL("deleteHistoryOnClose"), ui->deleteHistoryOnClose->isChecked());
    settings.setValue(QSL("HTML5StorageEnabled"), ui->html5storage->isChecked());
    settings.setValue(QSL("deleteHTML5StorageOnClose"), ui->deleteHtml5storageOnClose->isChecked());
    settings.endGroup();

    //NOTIFICATIONS
    settings.beginGroup(QSL("Notifications"));
    settings.setValue(QSL("Timeout"), ui->notificationTimeout->value() * 1000);
    settings.setValue(QSL("Enabled"), !ui->doNotUseNotifications->isChecked());
    settings.setValue(QSL("UseNativeDesktop"), ui->useNativeSystemNotifications->isChecked());
    settings.setValue(QSL("Position"), m_notification.data() ? m_notification.data()->pos() : m_notifPosition);
    settings.endGroup();

    //SPELLCHECK
    settings.beginGroup(QSL("SpellCheck"));
    settings.setValue(QSL("Enabled"), ui->spellcheckEnabled->isChecked());
    QStringList languages;
    for (int i = 0; i < ui->spellcheckLanguages->count(); ++i) {
        QListWidgetItem *item = ui->spellcheckLanguages->item(i);
        if (item->checkState() == Qt::Checked) {
            languages.append(item->data(Qt::UserRole).toString());
        }
    }
    settings.setValue(QSL("Languages"), languages);
    settings.endGroup();

    //OTHER
    //AddressBar
    settings.beginGroup(QSL("AddressBar"));
    settings.setValue(QSL("showSuggestions"), ui->addressbarCompletion->currentIndex());
    settings.setValue(QSL("useInlineCompletion"), ui->useInlineCompletion->isChecked());
    settings.setValue(QSL("alwaysShowGoIcon"), ui->alwaysShowGoIcon->isChecked());
    settings.setValue(QSL("showZoomLabel"), ui->showZoomLabel->isChecked());
    settings.setValue(QSL("showSwitchTab"), ui->completionShowSwitchTab->isChecked());
    settings.setValue(QSL("SelectAllTextOnDoubleClick"), ui->selectAllOnFocus->isChecked());
    settings.setValue(QSL("SelectAllTextOnClick"), ui->selectAllOnClick->isChecked());
    settings.setValue(QSL("CompletionPopupExpandToWindow"), ui->completionPopupExpandToWindow->isChecked());
    settings.setValue(QSL("ShowLoadingProgress"), ui->showLoadingInAddressBar->isChecked());
    settings.setValue(QSL("ProgressStyle"), ui->progressStyleSelector->currentIndex());
    settings.setValue(QSL("UseCustomProgressColor"), ui->checkBoxCustomProgressColor->isChecked());
    settings.setValue(QSL("CustomProgressColor"), ui->customColorToolButton->property("ProgressColor").value<QColor>());
    settings.endGroup();

    settings.beginGroup(QSL("SearchEngines"));
    settings.setValue(QSL("SearchFromAddressBar"), ui->searchFromAddressBar->isChecked());
    settings.setValue(QSL("SearchWithDefaultEngine"), ui->searchWithDefaultEngine->isChecked());
    settings.setValue(QSL("showSearchSuggestions"), ui->showABSearchSuggestions->isChecked());
    settings.endGroup();

    //Proxy Configuration
    int proxyType;
    if (ui->noProxy->isChecked()) {
        proxyType = 0;
    } else if (ui->systemProxy->isChecked()) {
        proxyType = 2;
    } else if (ui->proxyType->currentIndex() == 0) { // Http
        proxyType = 3;
    } else { // Socks5
        proxyType = 4;
    }

    settings.beginGroup(QSL("Web-Proxy"));
    settings.setValue(QSL("ProxyType"), proxyType);
    settings.setValue(QSL("HostName"), ui->proxyServer->text());
    settings.setValue(QSL("Port"), ui->proxyPort->text().toInt());
    settings.setValue(QSL("Username"), ui->proxyUsername->text());
    settings.setValue(QSL("Password"), ui->proxyPassword->text());
    settings.endGroup();

    ProfileManager::setStartingProfile(ui->startProfile->currentText());

    m_pluginsList->save();
    m_themesManager->save();
    mApp->cookieJar()->loadSettings();
    mApp->history()->loadSettings();
    mApp->reloadSettings();
    mApp->desktopNotifications()->loadSettings();
    mApp->autoFill()->loadSettings();
    mApp->networkManager()->loadSettings();

    WebScrollBarManager::instance()->loadSettings();
}

Preferences::~Preferences()
{
    Settings().setValue(QSL("Preferences/Geometry"), saveGeometry());

    delete ui;
    delete m_autoFillManager;
    delete m_pluginsList;
    delete m_notification.data();
}

void Preferences::setProgressBarColorIcon(QColor color)
{
    const int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QPixmap pm(QSize(size, size));
    if (!color.isValid()) {
        color = palette().color(QPalette::Highlight);
    }
    pm.fill(color);
    ui->customColorToolButton->setIcon(pm);
    ui->customColorToolButton->setProperty("ProgressColor", color);
}

void Preferences::selectCustomProgressBarColor()
{
    QColor newColor = QColorDialog::getColor(ui->customColorToolButton->property("ProgressColor").value<QColor>(), this, tr("Select Color"));
    if (newColor.isValid()) {
        setProgressBarColorIcon(newColor);
    }
}
