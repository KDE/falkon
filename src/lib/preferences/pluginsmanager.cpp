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
#include "pluginsmanager.h"
#include "ui_pluginslist.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "plugininterface.h"
#include "pluginlistdelegate.h"
#include "qztools.h"
#include "settings.h"
#include "iconprovider.h"
#include "../config.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QTimer>

PluginsManager::PluginsManager(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PluginsList)
    , m_loaded(false)
{
    ui->setupUi(this);
    ui->list->setLayoutDirection(Qt::LeftToRight);
    ui->butSettings->setIcon(IconProvider::settingsIcon());
    ui->butRemove->setIcon(QIcon::fromTheme(QSL("edit-delete")));

    //Application Extensions
    Settings settings;
    settings.beginGroup("Plugin-Settings");
    bool appPluginsEnabled = settings.value("EnablePlugins", true).toBool();
    settings.endGroup();

    ui->list->setEnabled(appPluginsEnabled);

    connect(ui->butSettings, &QAbstractButton::clicked, this, &PluginsManager::settingsClicked);
    connect(ui->butRemove, &QAbstractButton::clicked, this, &PluginsManager::removeClicked);
    connect(ui->list, &QListWidget::currentItemChanged, this, &PluginsManager::currentChanged);
    connect(ui->list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);
    connect(ui->search, &QLineEdit::textChanged, this, &PluginsManager::addFilter);
    connect(mApp->plugins(), &Plugins::availablePluginsChanged, this, &PluginsManager::refresh);

    ui->list->setItemDelegate(new PluginListDelegate(ui->list));
}

void PluginsManager::load()
{
    if (!m_loaded) {
        refresh();
        m_loaded = true;
    }
}

void PluginsManager::save()
{
    if (!m_loaded) {
        return;
    }

    QStringList allowedPlugins;
    for (int i = 0; i < ui->list->count(); i++) {
        QListWidgetItem* item = ui->list->item(i);

        if (item->checkState() == Qt::Checked) {
            const Plugins::Plugin plugin = item->data(Qt::UserRole + 10).value<Plugins::Plugin>();
            allowedPlugins.append(plugin.pluginId);
        }
    }

    Settings settings;
    settings.beginGroup("Plugin-Settings");
    settings.setValue("AllowedPlugins", allowedPlugins);
    settings.endGroup();
}

void PluginsManager::refresh()
{
    if (m_blockRefresh) {
        return;
    }

    const int oldCurrentRow = ui->list->currentRow();

    ui->list->clear();
    ui->butSettings->setEnabled(false);
    disconnect(ui->list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);

    const QList<Plugins::Plugin> &allPlugins = mApp->plugins()->availablePlugins();

    for (const Plugins::Plugin &plugin : allPlugins) {
        PluginSpec spec = plugin.pluginSpec;

        QListWidgetItem* item = new QListWidgetItem(ui->list);
        QIcon icon = QIcon(spec.icon);
        if (icon.isNull()) {
            icon = QIcon(QSL(":/icons/preferences/extensions.svg"));
        }
        item->setIcon(icon);

        QString pluginInfo = QString("<b>%1</b> %2<br/><i>%3</i><br/>").arg(spec.name, spec.version, spec.author.toHtmlEscaped());
        item->setToolTip(pluginInfo);

        item->setText(spec.name);
        item->setData(Qt::UserRole, spec.version);
        item->setData(Qt::UserRole + 1, spec.author);
        item->setData(Qt::UserRole + 2, spec.description);

        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(plugin.isLoaded() ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole + 10, QVariant::fromValue(plugin));

        ui->list->addItem(item);
    }

    sortItems();

    if (oldCurrentRow >= 0) {
        ui->list->setCurrentRow(qMax(0, oldCurrentRow - 1));
        ui->list->setFocus();
    }

    connect(ui->list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);
}

void PluginsManager::sortItems()
{
    ui->list->sortItems();

    bool itemMoved;
    do {
        itemMoved = false;
        for (int i = 0; i < ui->list->count(); ++i) {
            QListWidgetItem* topItem = ui->list->item(i);
            QListWidgetItem* bottomItem = ui->list->item(i + 1);
            if (!topItem || !bottomItem) {
                continue;
            }

            if (topItem->checkState() == Qt::Unchecked && bottomItem->checkState() == Qt::Checked) {
                QListWidgetItem* item = ui->list->takeItem(i + 1);
                ui->list->insertItem(i, item);
                itemMoved = true;
            }
        }
    }
    while (itemMoved);
}

void PluginsManager::currentChanged(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    const Plugins::Plugin plugin = item->data(Qt::UserRole + 10).value<Plugins::Plugin>();
    ui->butSettings->setEnabled(plugin.isLoaded() && plugin.pluginSpec.hasSettings);
    ui->butRemove->setEnabled(plugin.isRemovable());
}

void PluginsManager::itemChanged(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    Plugins::Plugin plugin = item->data(Qt::UserRole + 10).value<Plugins::Plugin>();

    m_blockRefresh = true;

    if (item->checkState() == Qt::Checked) {
        mApp->plugins()->loadPlugin(&plugin);
    }
    else {
        mApp->plugins()->unloadPlugin(&plugin);
    }

    m_blockRefresh = false;

    disconnect(ui->list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);

    if (item->checkState() == Qt::Checked && !plugin.isLoaded()) {
        item->setCheckState(Qt::Unchecked);
        QMessageBox::critical(this, tr("Error!"), tr("Cannot load extension!"));
    }

    item->setData(Qt::UserRole + 10, QVariant::fromValue(plugin));

    connect(ui->list, &QListWidget::itemChanged, this, &PluginsManager::itemChanged);

    currentChanged(ui->list->currentItem());
}

void PluginsManager::settingsClicked()
{
    QListWidgetItem* item = ui->list->currentItem();
    if (!item || item->checkState() == Qt::Unchecked) {
        return;
    }

    Plugins::Plugin plugin = item->data(Qt::UserRole + 10).value<Plugins::Plugin>();

    if (!plugin.isLoaded()) {
        mApp->plugins()->loadPlugin(&plugin);

        item->setData(Qt::UserRole + 10, QVariant::fromValue(plugin));
    }

    if (plugin.isLoaded() && plugin.pluginSpec.hasSettings) {
        plugin.instance->showSettings(this);
    }
}

void PluginsManager::removeClicked()
{
    QListWidgetItem* item = ui->list->currentItem();
    if (!item) {
        return;
    }

    Plugins::Plugin plugin = item->data(Qt::UserRole + 10).value<Plugins::Plugin>();

    const auto button = QMessageBox::warning(this, tr("Confirmation"),
                                             tr("Are you sure you want to remove '%1'?").arg(plugin.pluginSpec.name),
                                             QMessageBox::Yes | QMessageBox::No);
    if (button != QMessageBox::Yes) {
        return;
    }

    mApp->plugins()->removePlugin(&plugin);
}

void PluginsManager::addFilter(const QString& filter) {
    for (int i = 0; i < ui->list->count(); ++i) {
        const QString& pluginName = ui->list->item(i)->text();

        if (pluginName.contains(filter,Qt::CaseInsensitive) || !filter.size()) {
            ui->list->item(i)->setHidden(false);
        }
        else {
            ui->list->item(i)->setHidden(true);
        }
    }
}

PluginsManager::~PluginsManager()
{
    delete ui;
}
