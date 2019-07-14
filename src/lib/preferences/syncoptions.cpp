/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2019 Prasenjit Kumar Shaw <shawprasenjit07@gmail.com>
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
#include "syncoptions.h"
#include "ui_syncoptions.h"
#include "fxalogin.h"
#include "mainapplication.h"
#include "syncmanager.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QPushButton>

SyncOptions::SyncOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SyncOptions)
{
    ui->setupUi(this);

    loginPage = new FxALoginPage(this);
    ui->fxaloginframe->addWidget(loginPage);

    connect(ui->btnSyncNow, &QPushButton::clicked, mApp->syncManager(), &SyncManager::sync);
}

SyncOptions::~SyncOptions()
{
    delete ui;
}