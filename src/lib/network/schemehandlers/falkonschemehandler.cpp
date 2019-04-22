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
#include "falkonschemehandler.h"
#include "qztools.h"
#include "browserwindow.h"
#include "mainapplication.h"
#include "speeddial.h"
#include "pluginproxy.h"
#include "plugininterface.h"
#include "settings.h"
#include "datapaths.h"
#include "iconprovider.h"
#include "sessionmanager.h"
#include "restoremanager.h"
#include "../config.h"

#include <QTimer>
#include <QSettings>
#include <QUrlQuery>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestJob>

#include <KLocalizedString>

static QString authorString(const char* name, const QString &mail)
{
    return QSL("%1 &lt;<a href=\"mailto:%2\">%2</a>&gt;").arg(QString::fromUtf8(name), mail);
}

FalkonSchemeHandler::FalkonSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
{
}

void FalkonSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    if (handleRequest(job)) {
        return;
    }

    QStringList knownPages;
    knownPages << "about" << "start" << "speeddial" << "config" << "restore";

    if (knownPages.contains(job->requestUrl().path()))
        job->reply(QByteArrayLiteral("text/html"), new FalkonSchemeReply(job, job));
    else
        job->fail(QWebEngineUrlRequestJob::UrlInvalid);
}

bool FalkonSchemeHandler::handleRequest(QWebEngineUrlRequestJob *job)
{
    QUrlQuery query(job->requestUrl());
    if (!query.isEmpty() && job->requestUrl().path() == QL1S("restore")) {
        if (mApp->restoreManager()) {
            if (query.hasQueryItem(QSL("new-session"))) {
                mApp->restoreManager()->clearRestoreData();
            } else if (query.hasQueryItem(QSL("restore-session"))) {
                mApp->restoreSession(nullptr, mApp->restoreManager()->restoreData());
            }
        }
        mApp->destroyRestoreManager();
        job->redirect(QUrl(QSL("falkon:start")));
        return true;
    } else if (job->requestUrl().path() == QL1S("reportbug")) {
        job->redirect(QUrl(Qz::BUGSADDRESS));
        return true;
    }

    return false;
}

FalkonSchemeReply::FalkonSchemeReply(QWebEngineUrlRequestJob *job, QObject *parent)
    : QIODevice(parent)
    , m_loaded(false)
    , m_job(job)
{
    m_pageName = m_job->requestUrl().path();
    loadPage();
}

void FalkonSchemeReply::loadPage()
{
    if (m_loaded)
        return;

    QString contents;

    if (m_pageName == QLatin1String("about")) {
        contents = aboutPage();
    } else if (m_pageName == QLatin1String("start")) {
        contents = startPage();
    } else if (m_pageName == QLatin1String("speeddial")) {
        contents = speeddialPage();
    } else if (m_pageName == QLatin1String("config")) {
        contents = configPage();
    } else if (m_pageName == QLatin1String("restore")) {
        contents = restorePage();
    }

    QMutexLocker lock(&m_mutex);
    m_buffer.setData(contents.toUtf8());
    m_buffer.open(QIODevice::ReadOnly);
    lock.unlock();

    emit readyRead();

    m_loaded = true;
}

qint64 FalkonSchemeReply::bytesAvailable() const
{
    QMutexLocker lock(&m_mutex);
    return m_buffer.bytesAvailable();
}

qint64 FalkonSchemeReply::readData(char *data, qint64 maxSize)
{
    QMutexLocker lock(&m_mutex);
    return m_buffer.read(data, maxSize);
}

qint64 FalkonSchemeReply::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

QString FalkonSchemeReply::startPage()
{
    static QString sPage;

    if (!sPage.isEmpty()) {
        return sPage;
    }

    sPage.append(QzTools::readAllFileContents(":html/start.html"));
    sPage.replace(QLatin1String("%ABOUT-IMG%"), QSL("qrc:icons/other/startpage.svg"));

    sPage.replace(QLatin1String("%TITLE%"), i18n("Start Page"));
    sPage.replace(QLatin1String("%BUTTON-LABEL%"), i18n("Search on Web"));
    sPage.replace(QLatin1String("%SEARCH-BY%"), i18n("Search results provided by DuckDuckGo"));
    sPage.replace(QLatin1String("%WWW%"), Qz::WIKIADDRESS);
    sPage.replace(QLatin1String("%ABOUT-FALKON%"), i18n("About Falkon"));
    sPage.replace(QLatin1String("%PRIVATE-BROWSING%"), mApp->isPrivate() ? i18n("<h1>Private Browsing</h1>") : QString());
    sPage = QzTools::applyDirectionToPage(sPage);

    return sPage;
}

QString FalkonSchemeReply::aboutPage()
{
    static QString aPage;

    if (aPage.isEmpty()) {
        aPage.append(QzTools::readAllFileContents(":html/about.html"));
        aPage.replace(QLatin1String("%ABOUT-IMG%"), QSL("qrc:icons/other/about.svg"));
        aPage.replace(QLatin1String("%COPYRIGHT-INCLUDE%"), QzTools::readAllFileContents(":html/copyright").toHtmlEscaped());

        aPage.replace(QLatin1String("%TITLE%"), i18n("About Falkon"));
        aPage.replace(QLatin1String("%ABOUT-FALKON%"), i18n("About Falkon"));
        aPage.replace(QLatin1String("%INFORMATIONS-ABOUT-VERSION%"), i18n("Information about version"));
        aPage.replace(QLatin1String("%COPYRIGHT%"), i18n("Copyright"));

        aPage.replace(QLatin1String("%VERSION-INFO%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Version"),
#ifdef FALKON_GIT_REVISION
                              QString("%1 (%2)").arg(Qz::VERSION, FALKON_GIT_REVISION)));
#else
                              Qz::VERSION));
#endif

        aPage.replace(QLatin1String("%MAIN-DEVELOPER%"), i18n("Main developer"));
        aPage.replace(QLatin1String("%MAIN-DEVELOPER-TEXT%"), authorString(Qz::AUTHOR, "nowrep@gmail.com"));
        aPage = QzTools::applyDirectionToPage(aPage);
    }

    return aPage;
}

QString FalkonSchemeReply::speeddialPage()
{
    static QString dPage;

    if (dPage.isEmpty()) {
        dPage.append(QzTools::readAllFileContents(":html/speeddial.html"));
        dPage.replace(QLatin1String("%IMG_PLUS%"), QLatin1String("qrc:html/plus.svg"));
        dPage.replace(QLatin1String("%IMG_CLOSE%"), QLatin1String("qrc:html/close.svg"));
        dPage.replace(QLatin1String("%IMG_EDIT%"), QLatin1String("qrc:html/edit.svg"));
        dPage.replace(QLatin1String("%IMG_RELOAD%"), QLatin1String("qrc:html/reload.svg"));
        dPage.replace(QLatin1String("%LOADING-IMG%"), QLatin1String("qrc:html/loading.gif"));
        dPage.replace(QLatin1String("%IMG_SETTINGS%"), QLatin1String("qrc:html/configure.svg"));

        dPage.replace(QLatin1String("%SITE-TITLE%"), i18n("Speed Dial"));
        dPage.replace(QLatin1String("%ADD-TITLE%"), i18n("Add New Page"));
        dPage.replace(QLatin1String("%TITLE-EDIT%"), i18n("Edit"));
        dPage.replace(QLatin1String("%TITLE-REMOVE%"), i18n("Remove"));
        dPage.replace(QLatin1String("%TITLE-RELOAD%"), i18n("Reload"));
        dPage.replace(QLatin1String("%TITLE-WARN%"), i18n("Are you sure you want to remove this speed dial?"));
        dPage.replace(QLatin1String("%TITLE-WARN-REL%"), i18n("Are you sure you want to reload all speed dials?"));
        dPage.replace(QLatin1String("%TITLE-FETCHTITLE%"), i18n("Load title from page"));
        dPage.replace(QLatin1String("%JAVASCRIPT-DISABLED%"), i18n("SpeedDial requires enabled JavaScript."));
        dPage.replace(QLatin1String("%URL%"), i18n("Url"));
        dPage.replace(QLatin1String("%TITLE%"), i18n("Title"));
        dPage.replace(QLatin1String("%APPLY%"), i18n("Apply"));
        dPage.replace(QLatin1String("%CANCEL%"), i18n("Cancel"));
        dPage.replace(QLatin1String("%NEW-PAGE%"), i18n("New Page"));
        dPage.replace(QLatin1String("%SETTINGS-TITLE%"), i18n("Speed Dial settings"));
        dPage.replace(QLatin1String("%TXT_PLACEMENT%"), i18n("Placement: "));
        dPage.replace(QLatin1String("%TXT_AUTO%"), i18n("Auto"));
        dPage.replace(QLatin1String("%TXT_COVER%"), i18n("Cover"));
        dPage.replace(QLatin1String("%TXT_FIT%"), i18n("Fit"));
        dPage.replace(QLatin1String("%TXT_FWIDTH%"), i18n("Fit Width"));
        dPage.replace(QLatin1String("%TXT_FHEIGHT%"), i18n("Fit Height"));
        dPage.replace(QLatin1String("%TXT_NOTE%"), i18n("Use custom wallpaper"));
        dPage.replace(QLatin1String("%TXT_SELECTIMAGE%"), i18n("Click to select image"));
        dPage.replace(QLatin1String("%TXT_NRROWS%"), i18n("Maximum pages in a row:"));
        dPage.replace(QLatin1String("%TXT_SDSIZE%"), i18n("Change size of pages:"));
        dPage.replace(QLatin1String("%TXT_CNTRDLS%"), i18n("Center speed dials"));
        dPage = QzTools::applyDirectionToPage(dPage);
    }

    QString page = dPage;
    SpeedDial* dial = mApp->plugins()->speedDial();

    page.replace(QLatin1String("%INITIAL-SCRIPT%"), dial->initialScript().toUtf8().toBase64());
    page.replace(QLatin1String("%IMG_BACKGROUND%"), dial->backgroundImage());
    page.replace(QLatin1String("%URL_BACKGROUND%"), dial->backgroundImageUrl());
    page.replace(QLatin1String("%B_SIZE%"), dial->backgroundImageSize());
    page.replace(QLatin1String("%ROW-PAGES%"), QString::number(dial->pagesInRow()));
    page.replace(QLatin1String("%SD-SIZE%"), QString::number(dial->sdSize()));
    page.replace(QLatin1String("%SD-CENTER%"), dial->sdCenter() ? QSL("true") : QSL("false"));

    return page;
}

QString FalkonSchemeReply::restorePage()
{
    static QString rPage;

    if (rPage.isEmpty()) {
        rPage.append(QzTools::readAllFileContents(":html/restore.html"));
        rPage.replace(QLatin1String("%IMAGE%"), QzTools::pixmapToDataUrl(IconProvider::standardIcon(QStyle::SP_MessageBoxWarning).pixmap(45)).toString());
        rPage.replace(QLatin1String("%TITLE%"), i18n("Restore Session"));
        rPage.replace(QLatin1String("%OOPS%"), i18n("Oops, Falkon crashed."));
        rPage.replace(QLatin1String("%APOLOGIZE%"), i18n("We apologize for this. Would you like to restore the last saved state?"));
        rPage.replace(QLatin1String("%TRY-REMOVING%"), i18n("Try removing one or more tabs that you think cause troubles"));
        rPage.replace(QLatin1String("%START-NEW%"), i18n("Or you can start completely new session"));
        rPage.replace(QLatin1String("%WINDOW%"), i18n("Window"));
        rPage.replace(QLatin1String("%WINDOWS-AND-TABS%"), i18n("Windows and Tabs"));
        rPage.replace(QLatin1String("%BUTTON-START-NEW%"), i18n("Start New Session"));
        rPage.replace(QLatin1String("%BUTTON-RESTORE%"), i18n("Restore"));
        rPage.replace(QLatin1String("%JAVASCRIPT-DISABLED%"), i18n("Requires enabled JavaScript."));
        rPage = QzTools::applyDirectionToPage(rPage);
    }

    return rPage;
}

QString FalkonSchemeReply::configPage()
{
    static QString cPage;

    if (cPage.isEmpty()) {
        cPage.append(QzTools::readAllFileContents(":html/config.html"));
        cPage.replace(QLatin1String("%ABOUT-IMG%"), QSL("qrc:icons/other/about.svg"));

        cPage.replace(QLatin1String("%TITLE%"), i18n("Configuration Information"));
        cPage.replace(QLatin1String("%CONFIG%"), i18n("Configuration Information"));
        cPage.replace(QLatin1String("%INFORMATIONS-ABOUT-VERSION%"), i18n("Information about version"));
        cPage.replace(QLatin1String("%CONFIG-ABOUT%"), i18n("This page contains information about Falkon's current configuration - relevant for troubleshooting. Please include this information when submitting bug reports."));
        cPage.replace(QLatin1String("%BROWSER-IDENTIFICATION%"), i18n("Browser Identification"));
        cPage.replace(QLatin1String("%PATHS%"), i18n("Paths"));
        cPage.replace(QLatin1String("%BUILD-CONFIG%"), i18n("Build Configuration"));
        cPage.replace(QLatin1String("%PREFS%"), i18n("Preferences"));
        cPage.replace(QLatin1String("%OPTION%"), i18n("Option"));
        cPage.replace(QLatin1String("%VALUE%"), i18n("Value"));
        cPage.replace(QLatin1String("%PLUGINS%"), i18n("Extensions"));
        cPage.replace(QLatin1String("%PL-NAME%"), i18n("Name"));
        cPage.replace(QLatin1String("%PL-VER%"), i18n("Version"));
        cPage.replace(QLatin1String("%PL-AUTH%"), i18n("Author"));
        cPage.replace(QLatin1String("%PL-DESC%"), i18n("Description"));

        auto allPaths = [](DataPaths::Path type) {
            QString out;
            const auto paths = DataPaths::allPaths(type);
            for (const QString &path : paths) {
                if (!out.isEmpty()) {
                    out.append(QSL("<br>"));
                }
                out.append(path);
            }
            return out;
        };

        cPage.replace(QLatin1String("%VERSION-INFO%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Application version"),
#ifdef FALKON_GIT_REVISION
                              QString("%1 (%2)").arg(Qz::VERSION, FALKON_GIT_REVISION)
#else
                              Qz::VERSION
#endif
                                                          ) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Qt version"), qVersion()) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Platform"), QzTools::operatingSystemLong()));

        cPage.replace(QLatin1String("%PATHS-TEXT%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Profile"), DataPaths::currentProfilePath()) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Settings"), DataPaths::currentProfilePath() + "/settings.ini") +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Saved session"), SessionManager::defaultSessionPath()) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Data"), allPaths(DataPaths::AppData)) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Themes"), allPaths(DataPaths::Themes)) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Extensions"), allPaths(DataPaths::Plugins)));

#ifdef QT_DEBUG
        QString debugBuild = i18n("<b>Enabled</b>");
#else
        QString debugBuild = i18n("Disabled");
#endif

#ifdef Q_OS_WIN
#if defined(Q_OS_WIN) && defined(W7API)
        QString w7APIEnabled = i18n("<b>Enabled</b>");
#else
        QString w7APIEnabled = i18n("Disabled");
#endif
#endif

        QString portableBuild = mApp->isPortable() ? i18n("<b>Enabled</b>") : i18n("Disabled");

        cPage.replace(QLatin1String("%BUILD-CONFIG-TEXT%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Debug build"), debugBuild) +
#ifdef Q_OS_WIN
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Windows 7 API"), w7APIEnabled) +
#endif
                      QString("<dt>%1</dt><dd>%2<dd>").arg(i18n("Portable build"), portableBuild));

        cPage = QzTools::applyDirectionToPage(cPage);
    }

    QString page = cPage;
    page.replace(QLatin1String("%USER-AGENT%"), mApp->webProfile()->httpUserAgent());

    QString pluginsString;
    const QList<Plugins::Plugin> &availablePlugins = mApp->plugins()->availablePlugins();

    foreach (const Plugins::Plugin &plugin, availablePlugins) {
        PluginSpec spec = plugin.pluginSpec;
        pluginsString.append(QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>").arg(
                                 spec.name, spec.version, spec.author.toHtmlEscaped(), spec.description));
    }

    if (pluginsString.isEmpty()) {
        pluginsString = QString("<tr><td colspan=4 class=\"no-available-plugins\">%1</td></tr>").arg(i18n("No available extensions."));
    }

    page.replace(QLatin1String("%PLUGINS-INFO%"), pluginsString);

    QString allGroupsString;
    QSettings* settings = Settings::globalSettings();
    foreach (const QString &group, settings->childGroups()) {
        QString groupString = QString("<tr><th colspan=\"2\">[%1]</th></tr>").arg(group);
        settings->beginGroup(group);

        foreach (const QString &key, settings->childKeys()) {
            const QVariant keyValue = settings->value(key);
            QString keyString;

            switch (keyValue.type()) {
            case QVariant::ByteArray:
                keyString = QLatin1String("QByteArray");
                break;

            case QVariant::Point: {
                const QPoint point = keyValue.toPoint();
                keyString = QString("QPoint(%1, %2)").arg(point.x()).arg(point.y());
                break;
            }

            case QVariant::StringList:
                keyString = keyValue.toStringList().join(",");
                break;

            default:
                keyString = keyValue.toString();
            }

            if (keyString.isEmpty()) {
                keyString = QLatin1String("\"empty\"");
            }

            groupString.append(QString("<tr><td>%1</td><td>%2</td></tr>").arg(key, keyString.toHtmlEscaped()));
        }

        settings->endGroup();
        allGroupsString.append(groupString);
    }

    page.replace(QLatin1String("%PREFS-INFO%"), allGroupsString);

    return page;
}
