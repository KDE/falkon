/* ============================================================
 * Falkon - Qt web browser
 * Copyright (C) 2024  Juraj Oravec <jurajoravec@mailo.com>
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

#ifndef SITESETTINGSTEST_H
#define SITESETTINGSTEST_H

#include "sitesettingsmanager.h"

#include <QObject>

class WebPage;
class WebTab;

class SiteSettingsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void webAttributeTest();

private:
    bool checkWebAttributes(WebPage *page, QHash<QWebEngineSettings::WebAttribute, bool> webAttributes);

    void checkInternalPage(WebTab *tab, QUrl url);
    void checkExternalPage(WebTab *tab, QUrl url);
};

#endif // SITESETTINGSTEST_H

