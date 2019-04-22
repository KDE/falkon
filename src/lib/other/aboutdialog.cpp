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
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "browserwindow.h"
#include "mainapplication.h"
#include "webpage.h"
#include "useragentmanager.h"
#include "../config.h"

#include <QWebEnginePage>
#include <QWebEngineProfile>

#include <KLocalizedString>

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);
    ui->label->setPixmap(QIcon(QSL(":icons/other/about.svg")).pixmap(QSize(256, 100) * 1.1));

    showAbout();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::showAbout()
{
    QString aboutHtml;
    aboutHtml += "<div style='margin:0px 20px;'>";
    aboutHtml += i18n("<p><b>Application version %1</b><br/>",
#ifdef FALKON_GIT_REVISION
                       QString("%1 (%2)").arg(Qz::VERSION, FALKON_GIT_REVISION)
#else
                       Qz::VERSION
#endif
                   );
    aboutHtml += i18n("<b>QtWebEngine version %1</b></p>", qVersion());
    aboutHtml += QString("<p>&copy; %1 %2<br/>").arg(Qz::COPYRIGHT, Qz::AUTHOR);
    aboutHtml += QString("<a href=%1>%1</a></p>").arg(Qz::WWWADDRESS);
    aboutHtml += "<p>" + mApp->userAgentManager()->defaultUserAgent() + "</p>";
    aboutHtml += "</div>";
    ui->textLabel->setText(aboutHtml);
    setFixedHeight(sizeHint().height());
}
