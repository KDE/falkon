/* ============================================================
 * Falkon - Qt web browser
 * Copyright (C) 2024  Juraj Oravec <jurajoravec@mailo.com>
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

#include "sitesettingstest.h"

#include "mainapplication.h"
#include "tabbedwebview.h"
#include "webpage.h"

#include "autotests.h"
#include "tabwidget.h"


void SiteSettingsTest::initTestCase()
{
    /* Wait until the initial tab (at index 0) in the window is created */
    QTRY_COMPARE(mApp->getWindow()->tabCount(), 1);
}

void SiteSettingsTest::cleanupTestCase()
{
}

void SiteSettingsTest::webAttributeTest()
{
    SiteSettingsManager *siteSettings = mApp->siteSettingsManager();
    siteSettings->setOption(QWebEngineSettings::AutoLoadImages, QUrl(QSL("https://www.falkon.org/")), SiteSettingsManager::Deny);
    siteSettings->setOption(QWebEngineSettings::JavascriptEnabled, QUrl(QSL("https://kde.org/")), SiteSettingsManager::Deny);
    siteSettings->setOption(QWebEngineSettings::LocalStorageEnabled, QUrl(QSL("https://store.falkon.org/")), SiteSettingsManager::Deny);
    siteSettings->setOption(QWebEngineSettings::PlaybackRequiresUserGesture, QUrl(QSL("https://planet.kde.org/")), SiteSettingsManager::Allow);


    WebTab tab;

    checkInternalPage(&tab, QUrl(QSL("falkon:start")));

    checkExternalPage(&tab, QUrl(QSL("https://www.falkon.org/")));
    checkExternalPage(&tab, QUrl(QSL("https://kde.org/")));
    checkExternalPage(&tab, QUrl(QSL("https://store.falkon.org/")));

    checkInternalPage(&tab, QUrl(QSL("falkon:about")));

    checkExternalPage(&tab, QUrl(QSL("https://planet.kde.org/")));
}

bool SiteSettingsTest::checkWebAttributes(WebPage *page, QHash<QWebEngineSettings::WebAttribute, bool> webAttributes)
{
    for (auto it = webAttributes.begin(); it != webAttributes.end(); ++it) {
        if (page->settings()->testAttribute(it.key()) != it.value()) {
            return false;
        }
    }

    return true;
}

void SiteSettingsTest::checkInternalPage(WebTab *tab, QUrl url)
{
    QMap<QWebEngineSettings::WebAttribute, bool> internalWebAttributes = {
         {QWebEngineSettings::AutoLoadImages, true}
        ,{QWebEngineSettings::JavascriptEnabled, true}
        ,{QWebEngineSettings::JavascriptCanOpenWindows, false}
        ,{QWebEngineSettings::JavascriptCanAccessClipboard, true}
        ,{QWebEngineSettings::JavascriptCanPaste, false}
        ,{QWebEngineSettings::AllowWindowActivationFromJavaScript, false}
        ,{QWebEngineSettings::LocalStorageEnabled, true}
        ,{QWebEngineSettings::FullScreenSupportEnabled, mApp->webSettings()->testAttribute(QWebEngineSettings::FullScreenSupportEnabled)}
        ,{QWebEngineSettings::AllowRunningInsecureContent, false}
        ,{QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false}
        ,{QWebEngineSettings::PlaybackRequiresUserGesture, mApp->webSettings()->testAttribute(QWebEngineSettings::PlaybackRequiresUserGesture)}
        ,{QWebEngineSettings::WebRTCPublicInterfacesOnly, false}
    };

    QSignalSpy spy(tab, SIGNAL(loadingChanged(bool)));
    tab->load(url);
    QTRY_COMPARE(spy.count(), 3);

    auto *page = tab->webView()->page();
    for (auto it = internalWebAttributes.begin(); it != internalWebAttributes.end(); ++it) {
        QCOMPARE(page->settings()->testAttribute(it.key()), it.value());
    }
}

void SiteSettingsTest::checkExternalPage(WebTab *tab, QUrl url)
{
    SiteSettingsManager *siteSettings = mApp->siteSettingsManager();

    QSignalSpy spy(tab, SIGNAL(loadingChanged(bool)));
    tab->load(url);
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 3, 20000);

    auto *page = tab->webView()->page();
    QCOMPARE(checkWebAttributes(page, siteSettings->getWebAttributes(url)), true);
}

FALKONTEST_MAIN(SiteSettingsTest)
