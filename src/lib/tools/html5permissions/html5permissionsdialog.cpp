/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2014  David Rosca <nowrep@gmail.com>
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
#include "html5permissionsdialog.h"
#include "ui_html5permissionsdialog.h"
#include "settings.h"
#include "mainapplication.h"
#include "html5permissionsmanager.h"

#include <QtWebEngineWidgetsVersion>


HTML5PermissionsDialog::HTML5PermissionsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::HTML5PermissionsDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    loadSettings();

    ui->treeWidget->header()->resizeSection(0, 220);

    connect(ui->remove, &QPushButton::clicked, this, &HTML5PermissionsDialog::removeEntry);
    connect(ui->feature, SIGNAL(currentIndexChanged(int)), this, SLOT(featureIndexChanged()));
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &HTML5PermissionsDialog::saveSettings);

    showFeaturePermissions(currentFeature());
}

HTML5PermissionsDialog::~HTML5PermissionsDialog()
{
    delete ui;
}

void HTML5PermissionsDialog::showFeaturePermissions(QWebEnginePage::Feature feature)
{
    if (!m_granted.contains(feature) || !m_denied.contains(feature)) {
        return;
    }

    ui->treeWidget->clear();

    const auto grantedSites = m_granted.value(feature);
    for (const QString &site : grantedSites) {
        auto* item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, site);
        item->setText(1, tr("Allow"));
        item->setData(0, Qt::UserRole + 10, Allow);
        ui->treeWidget->addTopLevelItem(item);
    }

    const auto deniedSites = m_denied.value(feature);
    for (const QString &site : deniedSites) {
        auto* item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, site);
        item->setText(1, tr("Deny"));
        item->setData(0, Qt::UserRole + 10, Deny);
        ui->treeWidget->addTopLevelItem(item);
    }
}

void HTML5PermissionsDialog::featureIndexChanged()
{
    showFeaturePermissions(currentFeature());
}

void HTML5PermissionsDialog::removeEntry()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if (!item) {
        return;
    }

    Role role = static_cast<Role>(item->data(0, Qt::UserRole + 10).toInt());
    const QString origin = item->text(0);

    if (role == Allow)
        m_granted[currentFeature()].removeOne(origin);
    else
        m_denied[currentFeature()].removeOne(origin);

    delete item;
}

QWebEnginePage::Feature HTML5PermissionsDialog::currentFeature() const
{
    switch (ui->feature->currentIndex()) {
    case 0:
        return QWebEnginePage::Notifications;
    case 1:
        return QWebEnginePage::Geolocation;
    case 2:
        return QWebEnginePage::MediaAudioCapture;
    case 3:
        return QWebEnginePage::MediaVideoCapture;
    case 4:
        return QWebEnginePage::MediaAudioVideoCapture;
    case 5:
        return QWebEnginePage::MouseLock;
    case 6:
        return QWebEnginePage::DesktopVideoCapture;
    case 7:
        return QWebEnginePage::DesktopAudioVideoCapture;
    default:
        Q_UNREACHABLE();
        return QWebEnginePage::Notifications;
    }
}

void HTML5PermissionsDialog::loadSettings()
{
    Settings settings;
    settings.beginGroup(QSL("HTML5Notifications"));

    m_granted[QWebEnginePage::Notifications] = settings.value(QSL("NotificationsGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::Notifications] = settings.value(QSL("NotificationsDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::Geolocation] = settings.value(QSL("GeolocationGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::Geolocation] = settings.value(QSL("GeolocationDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::MediaAudioCapture] = settings.value(QSL("MediaAudioCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::MediaAudioCapture] = settings.value(QSL("MediaAudioCaptureDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::MediaVideoCapture] = settings.value(QSL("MediaVideoCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::MediaVideoCapture] = settings.value(QSL("MediaVideoCaptureDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::MediaAudioVideoCapture] = settings.value(QSL("MediaAudioVideoCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::MediaAudioVideoCapture] = settings.value(QSL("MediaAudioVideoCaptureDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::MouseLock] = settings.value(QSL("MouseLockGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::MouseLock] = settings.value(QSL("MouseLockDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::DesktopVideoCapture] = settings.value(QSL("DesktopVideoCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::DesktopVideoCapture] = settings.value(QSL("DesktopVideoCaptureDenied"), QStringList()).toStringList();

    m_granted[QWebEnginePage::DesktopAudioVideoCapture] = settings.value(QSL("DesktopAudioVideoCaptureGranted"), QStringList()).toStringList();
    m_denied[QWebEnginePage::DesktopAudioVideoCapture] = settings.value(QSL("DesktopAudioVideoCaptureDenied"), QStringList()).toStringList();

    settings.endGroup();
}

void HTML5PermissionsDialog::saveSettings()
{
    Settings settings;
    settings.beginGroup(QSL("HTML5Notifications"));

    settings.setValue(QSL("NotificationsGranted"), m_granted[QWebEnginePage::Notifications]);
    settings.setValue(QSL("NotificationsDenied"), m_denied[QWebEnginePage::Notifications]);

    settings.setValue(QSL("GeolocationGranted"), m_granted[QWebEnginePage::Geolocation]);
    settings.setValue(QSL("GeolocationDenied"), m_denied[QWebEnginePage::Geolocation]);

    settings.setValue(QSL("MediaAudioCaptureGranted"), m_granted[QWebEnginePage::MediaAudioCapture]);
    settings.setValue(QSL("MediaAudioCaptureDenied"), m_denied[QWebEnginePage::MediaAudioCapture]);

    settings.setValue(QSL("MediaVideoCaptureGranted"), m_granted[QWebEnginePage::MediaVideoCapture]);
    settings.setValue(QSL("MediaVideoCaptureDenied"), m_denied[QWebEnginePage::MediaVideoCapture]);

    settings.setValue(QSL("MediaAudioVideoCaptureGranted"), m_granted[QWebEnginePage::MediaAudioVideoCapture]);
    settings.setValue(QSL("MediaAudioVideoCaptureDenied"), m_denied[QWebEnginePage::MediaAudioVideoCapture]);

    settings.setValue(QSL("MouseLockGranted"), m_granted[QWebEnginePage::MouseLock]);
    settings.setValue(QSL("MouseLockDenied"), m_denied[QWebEnginePage::MouseLock]);

    settings.setValue(QSL("DesktopVideoCaptureGranted"), m_granted[QWebEnginePage::DesktopVideoCapture]);
    settings.setValue(QSL("DesktopVideoCaptureDenied"), m_denied[QWebEnginePage::DesktopVideoCapture]);

    settings.setValue(QSL("DesktopAudioVideoCaptureGranted"), m_granted[QWebEnginePage::DesktopAudioVideoCapture]);
    settings.setValue(QSL("DesktopAudioVideoCaptureDenied"), m_denied[QWebEnginePage::DesktopAudioVideoCapture]);

    settings.endGroup();

//    mApp->html5PermissionsManager()->loadSettings();
}
