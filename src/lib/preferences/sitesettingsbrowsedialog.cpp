// Falkon - SiteSettings permission browser dialog
// SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sitesettingsbrowsedialog.h"
#include "ui_sitesettingsbrowsedialog.h"

#include "mainapplication.h"
#include "sqldatabase.h"

#include <QCompleter>
#include <QSqlQueryModel>

constexpr int rolePermission = Qt::UserRole;

SiteSettingsBrowseDialog::SiteSettingsBrowseDialog(QString &name, QString &sqlColumn, QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::SiteSettingsBrowseDialog)
    , m_sqlColumn(sqlColumn)
{
    m_ui->setupUi(this);
    m_ui->nameLabel->setText(name);

    loadItems();

    m_completer = new QCompleter(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setFilterMode(Qt::MatchContains);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completerModel = new QSqlQueryModel(this);
    m_completerModel->setQuery(QSL("SELECT server from %1").arg(mApp->siteSettingsManager()->sqlTable()),
                               SqlDatabase::instance()->database());
    m_completer->setModel(m_completerModel);
    m_ui->urlEdit->setCompleter(m_completer);

    m_ui->urlComboBox->addItem(tr("Ask"), SiteSettingsManager::Ask); /* Has to be first to be able to hide it */
    m_ui->urlComboBox->addItem(tr("Allow"), SiteSettingsManager::Allow);
    m_ui->urlComboBox->addItem(tr("Deny"), SiteSettingsManager::Deny);
    m_ui->urlComboBox->addItem(tr("Default"), SiteSettingsManager::Default);

    m_ui->treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);

    connect(m_ui->askButton, &QPushButton::clicked, this, [=]() { setPermission(SiteSettingsManager::Ask); });
    connect(m_ui->allowButton, &QPushButton::clicked, this, [=]() { setPermission(SiteSettingsManager::Allow); });
    connect(m_ui->denyButton, &QPushButton::clicked, this, [=]() { setPermission(SiteSettingsManager::Deny); });
    connect(m_ui->defaultButton, &QPushButton::clicked, this, [=]() { setPermission(SiteSettingsManager::Default); });

    connect(m_ui->createButton, &QPushButton::clicked, this, &SiteSettingsBrowseDialog::createPermission);
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
    auto itemPermission = static_cast<SiteSettingsManager::Permission>(item->data(0, rolePermission).toInt());
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
    m_ui->urlComboBox->removeItem(0);
}

void SiteSettingsBrowseDialog::createPermission()
{
    const QString server = mApp->siteSettingsManager()->adjustUrl(QUrl(m_ui->urlEdit->text()));
    const auto permission = static_cast<SiteSettingsManager::Permission>(m_ui->urlComboBox->currentData(rolePermission).toInt());

    if (server.isEmpty()) {
        /* TODO: Print a status message or something to notify the user that url is not valid */
        qWarning() << "An invalid URL.";
        return;
    }

    const auto list_items = m_ui->treeWidget->findItems(server, Qt::MatchExactly);

    if (list_items.isEmpty()) {
        const QString permissionName = mApp->siteSettingsManager()->getPermissionName(permission);

        auto* item = new QTreeWidgetItem(m_ui->treeWidget);
        item->setText(0, server);
        item->setText(1, permissionName);
        item->setData(0, rolePermission, permission);

        m_ui->treeWidget->addTopLevelItem(item);
        m_ui->treeWidget->setCurrentItem(item);
    }
    else {
        m_ui->treeWidget->setCurrentItem(list_items.first());
    }

    setPermission(permission);
}

SiteSettingsBrowseDialog::~SiteSettingsBrowseDialog() = default;
