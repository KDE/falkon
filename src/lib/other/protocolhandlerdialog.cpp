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
#include "protocolhandlerdialog.h"
#include "ui_protocolhandlerdialog.h"
#include "mainapplication.h"
#include "protocolhandlermanager.h"

ProtocolHandlerDialog::ProtocolHandlerDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ProtocolHandlerDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    init();

    ui->treeWidget->header()->resizeSection(0, 100);
    connect(ui->remove, &QPushButton::clicked, this, &ProtocolHandlerDialog::removeEntry);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ProtocolHandlerDialog::accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ProtocolHandlerDialog::close);
}

ProtocolHandlerDialog::~ProtocolHandlerDialog()
{
    delete ui;
}

void ProtocolHandlerDialog::init()
{
    const auto handlers = mApp->protocolHandlerManager()->protocolHandlers();
    for (auto it = handlers.cbegin(); it != handlers.cend(); ++it) {
        auto *item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, it.key());
        item->setText(1, it.value().host());
        ui->treeWidget->addTopLevelItem(item);
    }
}

void ProtocolHandlerDialog::accepted()
{
    auto handlers = mApp->protocolHandlerManager()->protocolHandlers();
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
        handlers.remove(item->text(0));
    }
    for (auto it = handlers.cbegin(); it != handlers.cend(); ++it) {
        mApp->protocolHandlerManager()->removeProtocolHandler(it.key());
    }
    close();
}

void ProtocolHandlerDialog::removeEntry()
{
    delete ui->treeWidget->currentItem();
}

