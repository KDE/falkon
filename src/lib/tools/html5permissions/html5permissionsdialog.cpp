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
#include "sitesettingsmanager.h"

#include <QtWebEngineWidgetsVersion>
#include <sqldatabase.h>


HTML5PermissionsDialog::HTML5PermissionsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::HTML5PermissionsDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

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
    ui->treeWidget->clear();

    QString column = mApp->siteSettingsManager()->sqlColumnFromWebEngineFeature(feature);
    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(QSL("SELECT id, server, %1 FROM site_settings WHERE %1 != ?").arg(column));
    query.addBindValue(SiteSettingsManager::Default);
    query.exec();

    while (query.next()) {
        int id = query.value(0).toInt();
        if (m_removed.contains(feature) && m_removed[feature].contains(id)) {
            continue;
        }

        auto* item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, query.value(1).toString());

        auto perm = static_cast<SiteSettingsManager::Permission>(query.value(2).toInt());
        if (perm == SiteSettingsManager::Allow) {
            item->setText(1, tr("Allow"));
        }
        else {
            item->setText(1, tr("Deny"));
        }
        item->setData(0, Qt::UserRole + 10, id);
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

    const int domainId = item->data(0, Qt::UserRole + 10).toInt();
    m_removed[currentFeature()].append(domainId);

    delete item;
}

QWebEnginePage::Feature HTML5PermissionsDialog::currentFeature() const
{
    return indexToFeature(ui->feature->currentIndex());
}

QWebEnginePage::Feature HTML5PermissionsDialog::indexToFeature(const int index) const
{
    switch (index) {
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

void HTML5PermissionsDialog::saveSettings()
{
    QSqlQuery query(SqlDatabase::instance()->database());

    for (int i = 0; i < 8; ++i) {
        const QWebEnginePage::Feature feature = indexToFeature(i);
        const QString column = mApp->siteSettingsManager()->sqlColumnFromWebEngineFeature(feature);
        query.prepare(QSL("UPDATE site_settings SET %1 = 0 WHERE id = ?").arg(column));
        query.addBindValue(m_removed[feature]);

        if (!query.execBatch()) {
            qDebug() << query.lastError();
        }
    }
}
