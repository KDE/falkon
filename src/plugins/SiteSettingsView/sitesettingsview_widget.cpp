/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2024 Juraj Oravec <jurajoravec@mailo.com>
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
#include "sitesettingsview_widget.h"

#include "tabwidget.h"
#include "browserwindow.h"
#include "mainapplication.h"
#include "sitesettingsmanager.h"
#include "tabbedwebview.h"
#include "webpage.h"
#include "qzsettings.h"

#include <QTreeWidget>
#include <QHeaderView>
#include <QWebEngineProfile>
#include <QSpacerItem>

SiteSettingsView_Widget::SiteSettingsView_Widget(BrowserWindow* window)
{
    m_window = window;

    m_brushGreen = QBrush(QColorConstants::Green);
    m_brushYellow = QBrush(QColorConstants::Yellow);
    m_brushOrange = QBrush(QColorConstants::Svg::orange);
    m_brushRed = QBrush(QColorConstants::Red);
    m_brushBlue = QBrush(QColorConstants::Blue);

    m_availableOptions = {
        SiteSettingsManager::poAllowCookies,
        SiteSettingsManager::poZoomLevel
    };

    QSpacerItem *horizontalSpacer_1 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *horizontalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto *layoutTitle = new QHBoxLayout();
    auto *modeNameLabel = new QLabel(this);
    QString nameLabelText = QSL("<b>%1</b> <font color='%2'>%3</font>").arg(tr("Mode:"));
    if (mApp->isPrivate()) {
        modeNameLabel->setText(nameLabelText.arg(QSL("red"), tr("Private")));
    }
    else {
        modeNameLabel->setText(nameLabelText.arg(QSL("lightgreen"), tr("Normal")));
    }
    auto *isWebLabel = new QLabel(QSL("<b>%1</b> ").arg(tr("isWeb:")), this);
    m_isWebValue = new QLabel(this);
    auto *buttonRefresh = new QToolButton(this);
    buttonRefresh->setIcon(QIcon::fromTheme(QSL("view-refresh")));
    buttonRefresh->setToolTip(tr("Refresh"));

    layoutTitle->addWidget(modeNameLabel);
    layoutTitle->addItem(horizontalSpacer_1);
    layoutTitle->addWidget(isWebLabel);
    layoutTitle->addWidget(m_isWebValue);
    layoutTitle->addItem(horizontalSpacer_2);
    layoutTitle->addWidget(buttonRefresh);

    m_attributes = new QTreeWidget(this);
    m_attributes->setColumnCount(4);
    m_attributes->setHeaderLabels({tr("Real"), tr("Local"), tr("Default"), tr("Attributes")});
    m_attributes->setSortingEnabled(true);
    m_attributes->sortByColumn(3, Qt::AscendingOrder);
    m_attributes->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_attributes->header()->setSectionsMovable(false);
    m_attributes->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    auto attributes = mApp->siteSettingsManager()->getSupportedAttribute();
    for (auto &attr : attributes) {
        auto *item = new QTreeWidgetItem(m_attributes, {QSL(""), QSL(""), QSL(""), mApp->siteSettingsManager()->getOptionName(attr)});
        item->setToolTip(3, mApp->siteSettingsManager()->getOptionName(attr));
        m_attributeItems.insert(attr, item);
    }

    m_features = new QTreeWidget(this);
    m_features->setColumnCount(3);
    m_features->setHeaderLabels({tr("Local"), tr("Default"), tr("Feature")});
    m_features->setSortingEnabled(true);
    m_features->sortByColumn(2, Qt::AscendingOrder);
    m_features->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_features->header()->setSectionsMovable(false);
    m_features->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    auto features = mApp->siteSettingsManager()->getSupportedFeatures();
    for (auto &feature : features) {
        auto *item = new QTreeWidgetItem(m_features, {QSL(""), QSL(""), mApp->siteSettingsManager()->getOptionName(feature)});
        item->setToolTip(2, mApp->siteSettingsManager()->getOptionName(feature));
        m_featureItems.insert(feature, item);
    }

    m_options = new QTreeWidget(this);
    m_options->setColumnCount(3);
    m_options->setHeaderLabels({tr("Real"), tr("Local"), tr("Default"), tr("Option")});
    m_options->setSortingEnabled(true);
    m_options->sortByColumn(3, Qt::AscendingOrder);
    m_options->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_options->header()->setSectionsMovable(false);
    m_options->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    for (auto &option : m_availableOptions) {
        auto *item = new QTreeWidgetItem(m_options, {QSL(""), QSL(""), QSL(""), mApp->siteSettingsManager()->getOptionName(option)});
        item->setToolTip(3, mApp->siteSettingsManager()->getOptionName(option));
        m_optionsItems.insert(option, item);
    }

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    auto *l = new QVBoxLayout(this);
    l->addItem(layoutTitle);
    l->addWidget(m_attributes);
    l->addWidget(m_features);
    l->addWidget(m_options);
    l->addItem(verticalSpacer);
    this->setLayout(l);

    connect(m_window->tabWidget(), &TabWidget::currentChanged, this, &SiteSettingsView_Widget::updateData);
    connect(buttonRefresh, &QToolButton::clicked, this, &SiteSettingsView_Widget::updateData);

    updateData();
}

void SiteSettingsView_Widget::updateData(int index)
{
    Q_UNUSED(index)

    auto *tab = m_window->tabWidget()->webTab();
    if (tab == nullptr) {
        return;
    }

    auto url = tab->url();
    /* This condition is copied from webpage.cpp function acceptNavigationRequest()
     * Falkon has hardcoded properties / attributes for non Web pages*/
    const bool isWeb = url.scheme() == QL1S("http") || url.scheme() == QL1S("https") || url.scheme() == QL1S("file");
    const auto siteSettings = mApp->siteSettingsManager()->getSiteSettings(url);

    if (isWeb) {
        m_isWebValue->setText(QSL("<font color='lightgreen'>%1</font>").arg(tr("True")));
    }
    else {
        m_isWebValue->setText(QSL("<font color='red'>%1</font>").arg(tr("False")));
    }

    QHash<QWebEngineSettings::WebAttribute, QTreeWidgetItem*>::iterator it_attr;
    for (it_attr = m_attributeItems.begin(); it_attr != m_attributeItems.end(); ++it_attr) {
        auto attr = it_attr.key();
        auto attrReal = tab->webView()->page()->settings()->testAttribute(attr) ? SiteSettingsManager::Allow : SiteSettingsManager::Deny;
        auto attrDefault = mApp->webProfile()->settings()->testAttribute(attr) ? SiteSettingsManager::Allow : SiteSettingsManager::Deny;
        auto attrLocal = SiteSettingsManager::Default;

        if (!siteSettings.attributes.empty() && siteSettings.attributes.contains(attr)) {
            attrLocal = siteSettings.attributes[attr];
        }
        auto *item = it_attr.value();
        item->setText(0, mApp->siteSettingsManager()->getPermissionName(attrReal));
        item->setForeground(0, permissionColor(attrReal));
        item->setText(1, mApp->siteSettingsManager()->getPermissionName(attrLocal));
        item->setForeground(1, permissionColor(attrLocal));
        item->setText(2, mApp->siteSettingsManager()->getPermissionName(attrDefault));
        item->setForeground(2, permissionColor(attrDefault));
    }

    QHash<QWebEnginePage::Feature, QTreeWidgetItem*>::iterator it_ftr;
    for (it_ftr = m_featureItems.begin(); it_ftr != m_featureItems.end(); ++it_ftr) {
        auto ftr = it_ftr.key();
        auto ftrDefault = mApp->siteSettingsManager()->getDefaultPermission(ftr);
        auto ftrLocal = siteSettings.features[ftr];

        auto *item = it_ftr.value();
        item->setText(0, mApp->siteSettingsManager()->getPermissionName(ftrLocal));
        item->setForeground(0, permissionColor(ftrLocal));
        item->setText(1, mApp->siteSettingsManager()->getPermissionName(ftrDefault));
        item->setForeground(1, permissionColor(ftrDefault));
    }

    QHash<SiteSettingsManager::PageOptions, QTreeWidgetItem*>::iterator it_opt;
    for (it_opt = m_optionsItems.begin(); it_opt != m_optionsItems.end(); ++it_opt) {
        QString optRealText, optLocalText, optDefaultText;
        QBrush optRealColor, optLocalColor, optDefaultColor;
        const auto option = it_opt.key();
        auto *item = it_opt.value();

        switch (option) {
            case SiteSettingsManager::poAllowCookies: {
                SiteSettingsManager::Permission permission;

                optLocalText = mApp->siteSettingsManager()->getPermissionName(siteSettings.AllowCookies);
                optLocalColor = permissionColor(siteSettings.AllowCookies);

                permission = mApp->siteSettingsManager()->getDefaultPermission(option);
                optDefaultText = mApp->siteSettingsManager()->getPermissionName(permission);
                optDefaultColor = permissionColor(permission);
            }
                break;

            case SiteSettingsManager::poZoomLevel: {
                auto zoomLevels = WebView::zoomLevels();
                const int realZoomLevel = tab->webView()->zoomLevel();
                const int defualZoomLevel = qzSettings->defaultZoomLevel;
                const int storedZoomLevel = siteSettings.ZoomLevel;

                optRealText = tr("%1 (%2%)").arg(realZoomLevel).arg(zoomLevels[realZoomLevel]);
                optRealColor = m_brushGreen;

                if (storedZoomLevel == -1) {
                    optLocalText = tr("Default");
                    optLocalColor = m_brushYellow;
                }
                else {
                    optLocalText = tr("%1 (%2%)").arg(storedZoomLevel).arg(zoomLevels[storedZoomLevel]);
                    optLocalColor = m_brushGreen;
                }

                optDefaultText = tr("%1 (%2%)").arg(defualZoomLevel).arg(zoomLevels[defualZoomLevel]);
                optDefaultColor = m_brushGreen;
            }
                break;

            default:
                qDebug() << "Unknown option: " << option;
                continue;
        }

        item->setText(0, optRealText);
        item->setForeground(0, optRealColor);
        item->setText(1, optLocalText);
        item->setForeground(1, optLocalColor);
        item->setText(2, optDefaultText);
        item->setForeground(2, optDefaultColor);
    }
}

QBrush SiteSettingsView_Widget::permissionColor(SiteSettingsManager::Permission permission)
{
    switch (permission) {
        case SiteSettingsManager::Default:
            return m_brushYellow;
        case SiteSettingsManager::Allow:
            return m_brushGreen;
        case SiteSettingsManager::Deny:
            return m_brushRed;
        case SiteSettingsManager::Ask:
            return m_brushOrange;
        default:
            return m_brushBlue;
    }
}

void SiteSettingsView_Widget::loadFinished(WebPage* page)
{
    auto *tab = m_window->tabWidget()->webTab();
    if (tab == nullptr) {
        return;
    }

    if (page == tab->webView()->page()) {
        updateData();
    }
}
