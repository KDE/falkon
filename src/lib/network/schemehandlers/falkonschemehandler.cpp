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

    sPage.replace(QLatin1String("%TITLE%"), tr("Start Page"));
    sPage.replace(QLatin1String("%BUTTON-LABEL%"), tr("Search on Web"));
    sPage.replace(QLatin1String("%SEARCH-BY%"), tr("Search results provided by DuckDuckGo"));
    sPage.replace(QLatin1String("%WWW%"), Qz::WIKIADDRESS);
    sPage.replace(QLatin1String("%ABOUT-FALKON%"), tr("About Falkon"));
    sPage.replace(QLatin1String("%PRIVATE-BROWSING%"), mApp->isPrivate() ? tr("<h1>Private Browsing</h1>") : QString());
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

        aPage.replace(QLatin1String("%TITLE%"), tr("About Falkon"));
        aPage.replace(QLatin1String("%ABOUT-FALKON%"), tr("About Falkon"));
        aPage.replace(QLatin1String("%INFORMATIONS-ABOUT-VERSION%"), tr("Information about version"));
        aPage.replace(QLatin1String("%COPYRIGHT%"), tr("Copyright"));

        aPage.replace(QLatin1String("%VERSION-INFO%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Version"),
#ifdef FALKON_GIT_REVISION
                              QString("%1 (%2)").arg(Qz::VERSION, FALKON_GIT_REVISION)));
#else
                              Qz::VERSION));
#endif

        aPage.replace(QLatin1String("%MAIN-DEVELOPER%"), tr("Main developer"));
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

        dPage.replace(QLatin1String("%SITE-TITLE%"), tr("Speed Dial"));
        dPage.replace(QLatin1String("%ADD-TITLE%"), tr("Add New Page"));
        dPage.replace(QLatin1String("%TITLE-EDIT%"), tr("Edit"));
        dPage.replace(QLatin1String("%TITLE-REMOVE%"), tr("Remove"));
        dPage.replace(QLatin1String("%TITLE-RELOAD%"), tr("Reload"));
        dPage.replace(QLatin1String("%TITLE-WARN%"), tr("Are you sure you want to remove this speed dial?"));
        dPage.replace(QLatin1String("%TITLE-WARN-REL%"), tr("Are you sure you want to reload all speed dials?"));
        dPage.replace(QLatin1String("%TITLE-FETCHTITLE%"), tr("Load title from page"));
        dPage.replace(QLatin1String("%JAVASCRIPT-DISABLED%"), tr("SpeedDial requires enabled JavaScript."));
        dPage.replace(QLatin1String("%URL%"), tr("Url"));
        dPage.replace(QLatin1String("%TITLE%"), tr("Title"));
        dPage.replace(QLatin1String("%APPLY%"), tr("Apply"));
        dPage.replace(QLatin1String("%CANCEL%"), tr("Cancel"));
        dPage.replace(QLatin1String("%NEW-PAGE%"), tr("New Page"));
        dPage.replace(QLatin1String("%SETTINGS-TITLE%"), tr("Speed Dial settings"));
        dPage.replace(QLatin1String("%TXT_PLACEMENT%"), tr("Placement: "));
        dPage.replace(QLatin1String("%TXT_AUTO%"), tr("Auto"));
        dPage.replace(QLatin1String("%TXT_COVER%"), tr("Cover"));
        dPage.replace(QLatin1String("%TXT_FIT%"), tr("Fit"));
        dPage.replace(QLatin1String("%TXT_FWIDTH%"), tr("Fit Width"));
        dPage.replace(QLatin1String("%TXT_FHEIGHT%"), tr("Fit Height"));
        dPage.replace(QLatin1String("%TXT_NOTE%"), tr("Use custom wallpaper"));
        dPage.replace(QLatin1String("%TXT_SELECTIMAGE%"), tr("Click to select image"));
        dPage.replace(QLatin1String("%TXT_NRROWS%"), tr("Maximum pages in a row:"));
        dPage.replace(QLatin1String("%TXT_SDSIZE%"), tr("Change size of pages:"));
        dPage.replace(QLatin1String("%TXT_CNTRDLS%"), tr("Center speed dials"));
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
        rPage.replace(QLatin1String("%TITLE%"), tr("Restore Session"));
        rPage.replace(QLatin1String("%OOPS%"), tr("Oops, Falkon crashed."));
        rPage.replace(QLatin1String("%APOLOGIZE%"), tr("We apologize for this. Would you like to restore the last saved state?"));
        rPage.replace(QLatin1String("%TRY-REMOVING%"), tr("Try removing one or more tabs that you think cause troubles"));
        rPage.replace(QLatin1String("%START-NEW%"), tr("Or you can start completely new session"));
        rPage.replace(QLatin1String("%WINDOW%"), tr("Window"));
        rPage.replace(QLatin1String("%WINDOWS-AND-TABS%"), tr("Windows and Tabs"));
        rPage.replace(QLatin1String("%BUTTON-START-NEW%"), tr("Start New Session"));
        rPage.replace(QLatin1String("%BUTTON-RESTORE%"), tr("Restore"));
        rPage.replace(QLatin1String("%JAVASCRIPT-DISABLED%"), tr("Requires enabled JavaScript."));
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

        cPage.replace(QLatin1String("%TITLE%"), tr("Configuration Information"));
        cPage.replace(QLatin1String("%CONFIG%"), tr("Configuration Information"));
        cPage.replace(QLatin1String("%INFORMATIONS-ABOUT-VERSION%"), tr("Information about version"));
        cPage.replace(QLatin1String("%CONFIG-ABOUT%"), tr("This page contains information about Falkon's current configuration - relevant for troubleshooting. Please include this information when submitting bug reports."));
        cPage.replace(QLatin1String("%BROWSER-IDENTIFICATION%"), tr("Browser Identification"));
        cPage.replace(QLatin1String("%PATHS%"), tr("Paths"));
        cPage.replace(QLatin1String("%BUILD-CONFIG%"), tr("Build Configuration"));
        cPage.replace(QLatin1String("%PREFS%"), tr("Preferences"));
        cPage.replace(QLatin1String("%OPTION%"), tr("Option"));
        cPage.replace(QLatin1String("%VALUE%"), tr("Value"));
        cPage.replace(QLatin1String("%PLUGINS%"), tr("Extensions"));
        cPage.replace(QLatin1String("%PL-NAME%"), tr("Name"));
        cPage.replace(QLatin1String("%PL-VER%"), tr("Version"));
        cPage.replace(QLatin1String("%PL-AUTH%"), tr("Author"));
        cPage.replace(QLatin1String("%PL-DESC%"), tr("Description"));

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
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Application version"),
#ifdef FALKON_GIT_REVISION
                              QString("%1 (%2)").arg(Qz::VERSION, FALKON_GIT_REVISION)
#else
                              Qz::VERSION
#endif
                                                          ) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Qt version"), qVersion()) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Platform"), QzTools::operatingSystemLong()));

        cPage.replace(QLatin1String("%PATHS-TEXT%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Profile"), DataPaths::currentProfilePath()) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Settings"), DataPaths::currentProfilePath() + "/settings.ini") +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Saved session"), SessionManager::defaultSessionPath()) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Data"), allPaths(DataPaths::AppData)) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Themes"), allPaths(DataPaths::Themes)) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Extensions"), allPaths(DataPaths::Plugins)));

#ifdef QT_DEBUG
        QString debugBuild = tr("<b>Enabled</b>");
#else
        QString debugBuild = tr("Disabled");
#endif

#ifdef Q_OS_WIN
#if defined(Q_OS_WIN) && defined(W7API)
        QString w7APIEnabled = tr("<b>Enabled</b>");
#else
        QString w7APIEnabled = tr("Disabled");
#endif
#endif

        QString portableBuild = mApp->isPortable() ? tr("<b>Enabled</b>") : tr("Disabled");

        cPage.replace(QLatin1String("%BUILD-CONFIG-TEXT%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Debug build"), debugBuild) +
#ifdef Q_OS_WIN
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Windows 7 API"), w7APIEnabled) +
#endif
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Portable build"), portableBuild));

        cPage = QzTools::applyDirectionToPage(cPage);
    }

    QString page = cPage;
    page.replace(QLatin1String("%USER-AGENT%"), mApp->webProfile()->httpUserAgent());

    QString pluginsString;
    const QList<Plugins::Plugin> &availablePlugins = mApp->plugins()->availablePlugins();

    for (const Plugins::Plugin &plugin : availablePlugins) {
        PluginSpec spec = plugin.pluginSpec;
        pluginsString.append(QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>").arg(
                                 spec.name, spec.version, spec.author.toHtmlEscaped(), spec.description));
    }

    if (pluginsString.isEmpty()) {
        pluginsString = QString("<tr><td colspan=4 class=\"no-available-plugins\">%1</td></tr>").arg(tr("No available extensions."));
    }

    page.replace(QLatin1String("%PLUGINS-INFO%"), pluginsString);

    QString allGroupsString;
    QSettings* settings = Settings::globalSettings();
    const auto groups = settings->childGroups();
    for (const QString &group : groups) {
        QString groupString = QString("<tr><th colspan=\"2\">[%1]</th></tr>").arg(group);
        settings->beginGroup(group);

        const auto keys = settings->childKeys();
        for (const QString &key : keys) {
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
