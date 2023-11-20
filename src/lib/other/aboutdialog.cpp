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
#include <QtWebEngineCoreVersion>

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
    aboutHtml += QSL("<div style='margin:0px 20px;'>");
    aboutHtml += tr("<p><b>Application version %1</b><br/>").arg(
#ifdef FALKON_GIT_REVISION
                       QString(QSL("%1 (%2)")).arg(QString::fromLatin1(Qz::VERSION), QL1S(FALKON_GIT_REVISION))
#else
                       QString::fromLatin1(Qz::VERSION)
#endif
                   );
    aboutHtml += tr("<b>QtWebEngine version %1</b></p>").arg(QStringLiteral(QTWEBENGINECORE_VERSION_STR));
    aboutHtml += QStringLiteral("<p>&copy; %1 %2<br/>").arg(QString::fromLatin1(Qz::COPYRIGHT), QString::fromLatin1(Qz::AUTHOR));
    aboutHtml += QStringLiteral("<a href=%1>%1</a></p>").arg(QString::fromLatin1(Qz::WWWADDRESS));
    aboutHtml += QStringLiteral("<p>") + mApp->userAgentManager()->defaultUserAgent() + QStringLiteral("</p>");
    aboutHtml += QStringLiteral("</div>");
    ui->textLabel->setText(aboutHtml);
    setFixedHeight(sizeHint().height());
}
