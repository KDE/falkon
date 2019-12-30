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
#include "adblockaddsubscriptiondialog.h"
#include "ui_adblockaddsubscriptiondialog.h"
#include "adblockmanager.h"

AdBlockAddSubscriptionDialog::AdBlockAddSubscriptionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::AdBlockAddSubscriptionDialog)
{
    ui->setupUi(this);

    m_knownSubscriptions << Subscription(QSL("EasyList (English)"), ADBLOCK_EASYLIST_URL)
                         << Subscription(QSL("NoCoin List"), ADBLOCK_NOCOINLIST_URL)
                         << Subscription(QSL("BSI Lista Polska (Polish)"), QSL("http://www.bsi.info.pl/filtrABP.txt"))
                         << Subscription(QSL("Czech List (Czech)"), QSL("http://adblock.dajbych.net/adblock.txt"))
                         << Subscription(QSL("dutchblock (Dutch)"), QSL("http://groenewoudt.net/dutchblock/list.txt"))
                         << Subscription(QSL("EasyList (Spanish)"), QSL("https://easylist-downloads.adblockplus.org/easylistspanish.txt"))
                         << Subscription(QSL("IsraelList (Hebrew)"), QSL("http://secure.fanboy.co.nz/israelilist/IsraelList.txt"))
                         << Subscription(QSL("NLBlock (Dutch)"), QSL("http://www.verzijlbergh.com/adblock/nlblock.txt"))
                         << Subscription(QSL("Peter Lowe's list (English)"), QSL("http://pgl.yoyo.org/adservers/serverlist.php?hostformat=adblockplus&mimetype=plaintext"))
                         << Subscription(QSL("PLgeneral (Polish)"), QSL("http://www.niecko.pl/adblock/adblock.txt"))
                         << Subscription(QSL("Schacks Adblock Plus liste (Danish)"), QSL("http://adblock.schack.dk/block.txt"))
                         << Subscription(QSL("Xfiles (Italian)"), QSL("http://mozilla.gfsolone.com/filtri.txt"))
                         << Subscription(QSL("EasyPrivacy (English)"), QSL("http://easylist-downloads.adblockplus.org/easyprivacy.txt"))
                         << Subscription(QSL("RU Adlist (Russian)"), QSL("https://easylist-downloads.adblockplus.org/advblock.txt"))
                         << Subscription(QSL("ABPindo (Indonesian)"), QSL("https://raw.githubusercontent.com/heradhis/indonesianadblockrules/master/subscriptions/abpindo.txt"))
                         << Subscription(QSL("Easylist China (Chinese)"), QSL("https://easylist-downloads.adblockplus.org/easylistchina.txt"))
                         << Subscription(QSL("Anti-Adblock Killer"), QSL("https://raw.githubusercontent.com/reek/anti-adblock-killer/master/anti-adblock-killer-filters.txt"))
                         << Subscription(tr("Other..."), QString());

    for (const Subscription &subscription : qAsConst(m_knownSubscriptions)) {
        ui->comboBox->addItem(subscription.title);
    }

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
    indexChanged(0);
}

QString AdBlockAddSubscriptionDialog::title() const
{
    return ui->title->text();
}

QString AdBlockAddSubscriptionDialog::url() const
{
    return ui->url->text();
}

void AdBlockAddSubscriptionDialog::indexChanged(int index)
{
    const Subscription subscription = m_knownSubscriptions.at(index);

    // "Other..." entry
    if (subscription.url.isEmpty()) {
        ui->title->clear();
        ui->url->clear();
    }
    else {
        int pos = subscription.title.indexOf(QLatin1Char('('));
        QString title = subscription.title;

        if (pos > 0) {
            title = title.left(pos).trimmed();
        }

        ui->title->setText(title);
        ui->title->setCursorPosition(0);

        ui->url->setText(subscription.url);
        ui->url->setCursorPosition(0);
    }
}

AdBlockAddSubscriptionDialog::~AdBlockAddSubscriptionDialog()
{
    delete ui;
}
