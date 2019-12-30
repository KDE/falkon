/*
 * Copyright 2009 Benjamin C. Meyer <ben@meyerhome.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */
/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#include "acceptlanguage.h"
#include "ui_acceptlanguage.h"
#include "ui_addacceptlanguage.h"
#include "mainapplication.h"
#include "networkmanager.h"
#include "settings.h"

QStringList AcceptLanguage::defaultLanguage()
{
    QString longCode = QLocale::system().name().replace(QLatin1Char('_'), QLatin1Char('-'));

    if (longCode.size() == 5) {
        QStringList ret;
        ret << longCode << longCode.left(2);
        return ret;
    }

    return QStringList(longCode);
}

QByteArray AcceptLanguage::generateHeader(const QStringList &langs)
{
    if (langs.isEmpty()) {
        return QByteArray();
    }

    QByteArray header;
    header.append(langs.at(0).toLatin1());

    int counter = 8;
    for (int i = 1; i < langs.count(); i++) {
        QString s = "," + langs.at(i) + ";q=0.";
        s.append(QString::number(counter));
        if (counter != 2) {
            counter -= 2;
        }

        header.append(s.toLatin1());
    }

    return header;
}

AcceptLanguage::AcceptLanguage(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::AcceptLanguage)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);
    ui->listWidget->setLayoutDirection(Qt::LeftToRight);

    Settings settings;
    settings.beginGroup("Language");
    const QStringList langs = settings.value("acceptLanguage", defaultLanguage()).toStringList();
    settings.endGroup();

    for (const QString &code : langs) {
        QString code_ = code;
        QLocale loc = QLocale(code_.replace(QLatin1Char('-'), QLatin1Char('_')));
        QString label;

        if (loc.language() == QLocale::C) {
            label = tr("Personal [%1]").arg(code);
        }
        else {
            label = QString("%1/%2 [%3]").arg(loc.languageToString(loc.language()), loc.countryToString(loc.country()), code);
        }

        ui->listWidget->addItem(label);
    }

    connect(ui->add, &QAbstractButton::clicked, this, &AcceptLanguage::addLanguage);
    connect(ui->remove, &QAbstractButton::clicked, this, &AcceptLanguage::removeLanguage);
    connect(ui->up, &QAbstractButton::clicked, this, &AcceptLanguage::upLanguage);
    connect(ui->down, &QAbstractButton::clicked, this, &AcceptLanguage::downLanguage);
}

QStringList AcceptLanguage::expand(const QLocale::Language &language)
{
    QStringList allLanguages;
    QList<QLocale::Country> countries = QLocale::countriesForLanguage(language);
    for (int j = 0; j < countries.size(); ++j) {
        QString languageString;
        if (countries.count() == 1) {
            languageString = QString(QLatin1String("%1 [%2]"))
                             .arg(QLocale::languageToString(language))
                             .arg(QLocale(language).name().split(QLatin1Char('_')).at(0));
        }
        else {
            languageString = QString(QLatin1String("%1/%2 [%3]"))
                             .arg(QLocale::languageToString(language))
                             .arg(QLocale::countryToString(countries.at(j)))
                             .arg(QLocale(language, countries.at(j)).name().split(QLatin1Char('_')).join(QLatin1Char('-')).toLower());

        }
        if (!allLanguages.contains(languageString)) {
            allLanguages.append(languageString);
        }
    }
    return allLanguages;
}

void AcceptLanguage::addLanguage()
{
    Ui_AddAcceptLanguage acceptLangUi;
    QDialog dialog(this);
    acceptLangUi.setupUi(&dialog);
    acceptLangUi.listWidget->setLayoutDirection(Qt::LeftToRight);

    QStringList allLanguages;
    for (int i = 1 + (int)QLocale::C; i <= (int)QLocale::LastLanguage; ++i) {
        allLanguages += expand(QLocale::Language(i));
    }

    acceptLangUi.listWidget->addItems(allLanguages);

    connect(acceptLangUi.listWidget, &QListWidget::itemDoubleClicked, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    if (!acceptLangUi.ownDefinition->text().isEmpty()) {
        QString title = tr("Personal [%1]").arg(acceptLangUi.ownDefinition->text());
        ui->listWidget->addItem(title);
    }
    else {
        QListWidgetItem* c = acceptLangUi.listWidget->currentItem();
        if (!c) {
            return;
        }

        ui->listWidget->addItem(c->text());
    }
}

void AcceptLanguage::removeLanguage()
{
    delete ui->listWidget->currentItem();
}

void AcceptLanguage::upLanguage()
{
    int index = ui->listWidget->currentRow();
    QListWidgetItem* currentItem = ui->listWidget->currentItem();

    if (!currentItem || index == 0) {
        return;
    }

    ui->listWidget->takeItem(index);
    ui->listWidget->insertItem(index - 1, currentItem);
    ui->listWidget->setCurrentItem(currentItem);
}

void AcceptLanguage::downLanguage()
{
    int index = ui->listWidget->currentRow();
    QListWidgetItem* currentItem = ui->listWidget->currentItem();

    if (!currentItem || index == ui->listWidget->count() - 1) {
        return;
    }

    ui->listWidget->takeItem(index);
    ui->listWidget->insertItem(index + 1, currentItem);
    ui->listWidget->setCurrentItem(currentItem);
}

void AcceptLanguage::accept()
{
    QStringList langs;
    for (int i = 0; i < ui->listWidget->count(); i++) {
        QString t = ui->listWidget->item(i)->text();
        QString code = t.mid(t.indexOf(QLatin1Char('[')) + 1);
        code.remove(QLatin1Char(']'));
        langs.append(code);
    }

    Settings settings;
    settings.beginGroup("Language");
    settings.setValue("acceptLanguage", langs);

    mApp->networkManager()->loadSettings();

    QDialog::close();
}

AcceptLanguage::~AcceptLanguage()
{
    delete ui;
}
