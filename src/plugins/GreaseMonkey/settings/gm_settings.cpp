/* ============================================================
* GreaseMonkey plugin for Falkon
* Copyright (C) 2012-2018 David Rosca <nowrep@gmail.com>
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
#include "gm_settings.h"
#include "ui_gm_settings.h"
#include "gm_settingsscriptinfo.h"
#include "../gm_manager.h"
#include "../gm_script.h"
#include "qztools.h"

#include "mainapplication.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QInputDialog>

GM_Settings::GM_Settings(GM_Manager* manager, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::GM_Settings)
    , m_manager(manager)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    ui->iconLabel->setPixmap(QIcon(QSL(":gm/data/icon.svg")).pixmap(32));

    connect(ui->listWidget, &QListWidget::itemDoubleClicked,
            this, &GM_Settings::showItemInfo);
    connect(ui->listWidget, &GM_SettingsListWidget::updateItemRequested,
            this, &GM_Settings::updateItem);
    connect(ui->listWidget, &GM_SettingsListWidget::removeItemRequested,
            this, &GM_Settings::removeItem);
    connect(ui->openDirectory, &QAbstractButton::clicked,
            this, &GM_Settings::openScriptsDirectory);
    connect(ui->newScript, &QAbstractButton::clicked,
            this, &GM_Settings::newScript);
    connect(ui->link, &ClickableLabel::clicked,
            this, &GM_Settings::openUserJs);
    connect(manager, &GM_Manager::scriptsChanged,
            this, &GM_Settings::loadScripts);

    loadScripts();
}

void GM_Settings::openUserJs()
{
    mApp->addNewTab(QUrl(QSL("http://openuserjs.org")));
    close();
}

void GM_Settings::showItemInfo(QListWidgetItem* item)
{
    GM_Script* script = getScript(item);
    if (!script) {
        return;
    }

    auto* dialog = new GM_SettingsScriptInfo(script, this);
    dialog->open();
}

void GM_Settings::updateItem(QListWidgetItem* item)
{
    GM_Script *script = getScript(item);
    if (!script) {
        return;
    }
    script->updateScript();
}

void GM_Settings::removeItem(QListWidgetItem* item)
{
    GM_Script* script = getScript(item);
    if (!script) {
        return;
    }

    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Remove script"),
                                         tr("Are you sure you want to remove '%1'?").arg(script->name()),
                                         QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::Yes) {
        m_manager->removeScript(script);
    }
}

void GM_Settings::itemChanged(QListWidgetItem* item)
{
    GM_Script* script = getScript(item);
    if (!script) {
        return;
    }

    if (item->checkState() == Qt::Checked) {
        m_manager->enableScript(script);
    }
    else {
        m_manager->disableScript(script);
    }
}

void GM_Settings::openScriptsDirectory()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_manager->scriptsDirectory()));
}

void GM_Settings::newScript()
{
    const QString name = QInputDialog::getText(this, tr("Add script"), tr("Choose name for script:"));
    if (name.isEmpty())
        return;

    const QString script = QL1S("// ==UserScript== \n"
                                "// @name        %1 \n"
                                "// @namespace   kde.org \n"
                                "// @description Script description \n"
                                "// @include     * \n"
                                "// @version     1.0.0 \n"
                                "// ==/UserScript==\n\n");

    const QString fileName = QSL("%1/%2.user.js").arg(m_manager->scriptsDirectory(), QzTools::filterCharsFromFilename(name));

    QFile file(QzTools::ensureUniqueFilename(fileName));
    file.open(QFile::WriteOnly);
    file.write(script.arg(name).toUtf8());
    file.close();

    auto *gmScript = new GM_Script(m_manager, file.fileName());
    m_manager->addScript(gmScript);

    auto* dialog = new GM_SettingsScriptInfo(gmScript, this);
    dialog->open();
}

void GM_Settings::loadScripts()
{
    disconnect(ui->listWidget, &QListWidget::itemChanged,
               this, &GM_Settings::itemChanged);

    ui->listWidget->clear();

    const auto allScripts = m_manager->allScripts();
    for (GM_Script* script : allScripts) {
        auto* item = new QListWidgetItem(ui->listWidget);
        item->setText(script->name());
        item->setIcon(script->icon());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(script->isEnabled() ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole + 10, QVariant::fromValue((void*)script));

        connect(script, &GM_Script::updatingChanged, this, [this]() {
            ui->listWidget->viewport()->update();
        });

        ui->listWidget->addItem(item);
    }

    ui->listWidget->sortItems();

    bool itemMoved;
    do {
        itemMoved = false;
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem* topItem = ui->listWidget->item(i);
            QListWidgetItem* bottomItem = ui->listWidget->item(i + 1);
            if (!topItem || !bottomItem) {
                continue;
            }

            if (topItem->checkState() == Qt::Unchecked && bottomItem->checkState() == Qt::Checked) {
                QListWidgetItem* item = ui->listWidget->takeItem(i + 1);
                ui->listWidget->insertItem(i, item);
                itemMoved = true;
            }
        }
    }
    while (itemMoved);

    connect(ui->listWidget, &QListWidget::itemChanged, this, &GM_Settings::itemChanged);
}

GM_Script* GM_Settings::getScript(QListWidgetItem* item)
{
    if (!item) {
        return 0;
    }

    GM_Script* script = static_cast<GM_Script*>(item->data(Qt::UserRole + 10).value<void*>());
    return script;
}

GM_Settings::~GM_Settings()
{
    delete ui;
}
