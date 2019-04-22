/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 David Rosca <nowrep@gmail.com>
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
#include "ocssupport.h"
#include "pluginproxy.h"
#include "datapaths.h"
#include "networkmanager.h"
#include "desktopfile.h"
#include "desktopnotificationsfactory.h"
#include "mainapplication.h"

#include <QDir>
#include <QBuffer>
#include <QUrlQuery>
#include <QNetworkReply>

#include <KZip>
#include <KLocalizedString>

Q_GLOBAL_STATIC(OcsSupport, qz_ocs_support)

static DesktopFile readMetaData(const KArchiveDirectory *directory)
{
    const KArchiveEntry *entry = directory->entry(QSL("metadata.desktop"));
    if (!entry || !entry->isFile()) {
        qWarning() << "No metadata.desktop found";
        return DesktopFile();
    }
    const QString tempDir = DataPaths::path(DataPaths::Temp);
    static_cast<const KArchiveFile*>(entry)->copyTo(tempDir);
    return DesktopFile(tempDir + QL1S("/metadata.desktop"));
}

OcsSupport::OcsSupport(QObject *parent)
    : QObject(parent)
{
}

bool OcsSupport::handleUrl(const QUrl &url)
{
    if (url.host() != QL1S("install")) {
        return false;
    }

    QUrl fileUrl;
    QString fileType;
    QString fileName;

    const auto items = QUrlQuery(url).queryItems(QUrl::FullyDecoded);
    for (const auto &item : items) {
        if (item.first == QL1S("url")) {
            fileUrl = QUrl(item.second);
        } else if (item.first == QL1S("type")) {
            fileType = item.second;
        } else if (item.first == QL1S("filename")) {
            fileName = item.second;
        }
    }

    if (!fileType.startsWith(QL1S("falkon_"))) {
        return false;
    }

    if (fileType != QL1S("falkon_themes") && fileType != QL1S("falkon_extensions")) {
        qWarning() << "Unsupported type" << fileType;
        return false;
    }

    if (!fileUrl.isValid()) {
        qWarning() << "Invalid url" << fileUrl << url;
        return false;
    }

    qInfo() << "Downloading" << fileUrl;

    QNetworkReply *reply = mApp->networkManager()->get(QNetworkRequest(fileUrl));
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Error downloading" << fileUrl << reply->error() << reply->errorString();
            return;
        }
        QBuffer buf;
        buf.setData(reply->readAll());
        KZip zip(&buf);
        if (!zip.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open archive";
            return;
        }
        QString notifyMessage;
        if (fileType == QL1S("falkon_themes")) {
            installTheme(zip.directory());
        } else if (fileType == QL1S("falkon_extensions")) {
            installExtension(zip.directory());
        }
    });

    return true;
}

// static
OcsSupport *OcsSupport::instance()
{
    return qz_ocs_support();
}

void OcsSupport::installTheme(const KArchiveDirectory *directory)
{
    auto showError = []() {
        mApp->desktopNotifications()->showNotification(i18n("Installation failed"), i18n("Failed to install theme"));
    };

    if (directory->entries().size() != 1) {
        qWarning() << "Invalid archive format";
        showError();
        return;
    }

    const QString name = directory->entries().at(0);
    const KArchiveEntry *entry = directory->entry(name);
    if (!entry || !entry->isDirectory()) {
        qWarning() << "Invalid archive format";
        showError();
        return;
    }

    const DesktopFile metaData = readMetaData(static_cast<const KArchiveDirectory*>(entry));

    const QString targetDir = DataPaths::path(DataPaths::Config) + QL1S("/themes");
    QDir().mkpath(targetDir);

    if (QFileInfo::exists(targetDir + QL1C('/') + name)) {
        qWarning() << "Theme" << name << "already exists";
        mApp->desktopNotifications()->showNotification(i18n("Installation failed"), i18n("Theme is already installed"));
        return;
    }

    if (!directory->copyTo(targetDir)) {
        qWarning() << "Failed to copy theme to" << targetDir;
        showError();
        return;
    }

    qInfo() << "Theme installed to" << targetDir;

    mApp->desktopNotifications()->showNotification(i18n("Theme installed"), i18n("'%1' was successfully installed", metaData.name()));
}

void OcsSupport::installExtension(const KArchiveDirectory *directory)
{
    auto showError = []() {
        mApp->desktopNotifications()->showNotification(i18n("Installation failed"), i18n("Failed to install extension"));
    };

    if (directory->entries().size() != 1) {
        qWarning() << "Invalid archive format";
        showError();
        return;
    }

    const QString name = directory->entries().at(0);
    const KArchiveEntry *entry = directory->entry(name);
    if (!entry || !entry->isDirectory()) {
        qWarning() << "Invalid archive format";
        showError();
        return;
    }

    const DesktopFile metaData = readMetaData(static_cast<const KArchiveDirectory*>(entry));
    const QString extensionType = metaData.value(QSL("X-Falkon-Type")).toString();

    QString type;
    if (extensionType == QL1S("Extension/Python")) {
        type = QSL("python");
    } else if (extensionType == QL1S("Extension/Qml")) {
        type = QSL("qml");
    }

    if (type.isEmpty()) {
        qWarning() << "Unsupported extension type" << extensionType;
        showError();
        return;
    }

    const QString targetDir = DataPaths::path(DataPaths::Config) + QL1S("/plugins/");
    QDir().mkpath(targetDir);

    if (QFileInfo::exists(targetDir + QL1S("/") + name)) {
        qWarning() << "Extension" << name << "already exists";
        mApp->desktopNotifications()->showNotification(i18n("Installation failed"), i18n("Extension is already installed"));
        return;
    }

    if (!directory->copyTo(targetDir)) {
        qWarning() << "Failed to copy extension to" << targetDir;
        showError();
        return;
    }

    qInfo() << "Extension installed to" << targetDir;

    const QString fullId = QSL("%1:%2/%3").arg(type, targetDir, name);
    if (!mApp->plugins()->addPlugin(fullId)) {
        qWarning() << "Failed to add plugin" << fullId;
        showError();
        return;
    }

    mApp->desktopNotifications()->showNotification(i18n("Extension installed"), i18n("'%1' was successfully installed", metaData.name()));
}
