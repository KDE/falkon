// Falkon - SiteSettings permission browser dialog
// SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sitesettingsbrowsedialog.h"
#include "ui_sitesettingsbrowsedialog.h"

#include "mainapplication.h"
#include "sqldatabase.h"

const int rolePermission = Qt::UserRole + 10;

SiteSettingsBrowseDialog::SiteSettingsBrowseDialog(QString &name, QString &sqlColumn, QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::SiteSettingsBrowseDialog)
    , m_sqlColumn(sqlColumn)
{
    m_ui->setupUi(this);
    m_ui->nameLabel->setText(name);

    loadItems();

    m_ui->treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);

    connect(m_ui->askButton, &QPushButton::clicked, this, [=]() { setPermission(SiteSettingsManager::Ask); });
    connect(m_ui->allowButton, &QPushButton::clicked, this, [=]() { setPermission(SiteSettingsManager::Allow); });
    connect(m_ui->denyButton, &QPushButton::clicked, this, [=]() { setPermission(SiteSettingsManager::Deny); });
    connect(m_ui->defaultButton, &QPushButton::clicked, this, [=]() { setPermission(SiteSettingsManager::Default); });

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &SiteSettingsBrowseDialog::storeChanges);
}

void SiteSettingsBrowseDialog::loadItems()
{
    QSqlDatabase db = SqlDatabase::instance()->database();
    QString sqlTable = mApp->siteSettingsManager()->sqlTable();

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(QSL("SELECT server, %2 FROM %1 WHERE %2!=?").arg(sqlTable, m_sqlColumn));
    query.addBindValue(SiteSettingsManager::Default);
    query.exec();

    while (query.next()) {
        QString server = query.value(0).toString();
        auto permission =  mApp->siteSettingsManager()->intToPermission(query.value(1).toInt());
        QString permissionName = mApp->siteSettingsManager()->getPermissionName(permission);

        auto* item = new QTreeWidgetItem(m_ui->treeWidget);
        item->setText(0, server);
        item->setText(1, permissionName);
        item->setData(0, rolePermission, permission);

        m_ui->treeWidget->addTopLevelItem(item);
    }
}

void SiteSettingsBrowseDialog::storeChanges()
{
    QUrl url;
    for (QHash<QString, int>::iterator it = m_listModifications.begin(); it != m_listModifications.end(); ++it) {
        url.setHost(it.key());
        mApp->siteSettingsManager()->setOption(m_sqlColumn, url, it.value());
    }
}

void SiteSettingsBrowseDialog::setPermission(const SiteSettingsManager::Permission permission)
{
    QTreeWidgetItem* item = m_ui->treeWidget->currentItem();
    if (!item) {
        return;
    }

    const QString server = item->text(0);
    auto itemPermission = static_cast<SiteSettingsManager::Permission>(item->data(0, Qt::UserRole + 10).toInt());
    if (permission == itemPermission) {
        m_listModifications.remove(server);
    }

    QString permissionName = mApp->siteSettingsManager()->getPermissionName(permission);

    m_listModifications[server] = permission;
    item->setText(1, permissionName);
}

void SiteSettingsBrowseDialog::hideAskButton()
{
    m_ui->askButton->hide();
}

SiteSettingsBrowseDialog::~SiteSettingsBrowseDialog()
{
}
