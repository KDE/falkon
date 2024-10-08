/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
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
#include "qztools.h"
#include "siteinfowidget.h"
#include "ui_siteinfowidget.h"
#include "browserwindow.h"
#include "mainapplication.h"
#include "webpage.h"
#include "tabbedwebview.h"
#include "sqldatabase.h"
#include "protocolhandlermanager.h"
#include "networkmanager.h"

#include <QToolTip>

SiteInfoWidget::SiteInfoWidget(BrowserWindow* window, QWidget* parent)
    : LocationBarPopup(parent)
    , ui(new Ui::SiteInfoWidget)
    , m_window(window)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    setPopupAlignment(Qt::AlignLeft);

    WebView* view = m_window->weView();

    ui->titleLabel->setText(tr("<b>Site %1<b/>").arg(view->url().host()));

    bool secure = false;

    if (view->url().scheme() == QL1S("https")) {
        auto* nm = mApp->networkManager();
        QString host = view->url().host();

        if (nm->ignoredSslHosts().contains(host)) {
            ui->secureDescriptionLabel->setText(tr("Any certificate error is <b>permanently</b> ignored."));
        }
        else if (nm->ignoredSslErrors().contains(host)) {
            ui->secureDescriptionLabel->setText(tr("The certificate error is <b>temporarily</b> ignored."));
        }
        else if (nm->rejectedSslErrors().contains(host)) {
            ui->secureDescriptionLabel->setText(tr("Certificate was rejected."));
        }
        else {
            secure = true;
            ui->secureDescriptionLabel->hide();
        }
    }

    if (secure) {
        ui->secureLabel->setText(tr("Your connection to this site is <b>secured</b>."));
        ui->secureIcon->setPixmap(QPixmap(QSL(":/icons/locationbar/safe.png")));
    }
    else {
        ui->secureLabel->setText(tr("Your connection to this site is <b>unsecured</b>."));
        ui->secureIcon->setPixmap(QPixmap(QSL(":/icons/locationbar/unsafe.png")));
    }

    QString scheme = view->url().scheme();
    QString host = view->url().host();

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare(QSL("SELECT sum(count) FROM history WHERE url LIKE ?"));
    query.addBindValue(QSL("%1://%2%").arg(scheme, host));
    query.exec();

    if (query.next()) {
        int count = query.value(0).toInt();
        if (count > 3) {
            ui->historyLabel->setText(tr("This is your <b>%1</b> visit of this site.").arg(QString::number(count) + QLatin1Char('.')));
            ui->historyIcon->setPixmap(QPixmap(QSL(":/icons/locationbar/visit3.png")));
        }
        else if (count == 0) {
            ui->historyLabel->setText(tr("You have <b>never</b> visited this site before."));
            ui->historyIcon->setPixmap(QPixmap(QSL(":/icons/locationbar/visit1.png")));
        }
        else {
            ui->historyIcon->setPixmap(QPixmap(QSL(":/icons/locationbar/visit2.png")));
            QString text;
            if (count == 1) {
                text = tr("first");
            }
            else if (count == 2) {
                text = tr("second");
            }
            else if (count == 3) {
                text = tr("third");
            }
            ui->historyLabel->setText(tr("This is your <b>%1</b> visit of this site.").arg(text));
        }
    }

    updateProtocolHandler();

    connect(ui->pushButton, &QAbstractButton::clicked, m_window->action(QSL("Tools/SiteInfo")), &QAction::trigger);
    connect(ui->protocolHandlerButton, &QPushButton::clicked, this, &SiteInfoWidget::protocolHandlerButtonClicked);
}

SiteInfoWidget::~SiteInfoWidget()
{
    delete ui;
}

void SiteInfoWidget::updateProtocolHandler()
{
    WebPage *page = m_window->weView()->page();

    const QString scheme = page->registerProtocolHandlerRequestScheme();
    const QUrl registeredUrl = mApp->protocolHandlerManager()->protocolHandlers().value(scheme);

    if (!scheme.isEmpty() && registeredUrl != page->registerProtocolHandlerRequestUrl()) {
        ui->protocolHandlerLabel->setText(tr("Register as <b>%1</b> links handler").arg(page->registerProtocolHandlerRequestScheme()));
        ui->protocolHandlerButton->setText(tr("Register"));
    } else {
        ui->protocolHandlerLabel->hide();
        ui->protocolHandlerButton->hide();
        ui->protocolHandlerLine->hide();
    }
}

void SiteInfoWidget::protocolHandlerButtonClicked()
{
    WebPage *page = m_window->weView()->page();

    mApp->protocolHandlerManager()->addProtocolHandler(page->registerProtocolHandlerRequestScheme(), page->registerProtocolHandlerRequestUrl());
    close();
}
