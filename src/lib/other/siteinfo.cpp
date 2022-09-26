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
#include "siteinfo.h"
#include "ui_siteinfo.h"
#include "listitemdelegate.h"
#include "webview.h"
#include "webpage.h"
#include "mainapplication.h"
#include "downloaditem.h"
#include "certificateinfowidget.h"
#include "qztools.h"
#include "iconprovider.h"
#include "scripts.h"
#include "networkmanager.h"
#include "locationbar.h"

#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QNetworkDiskCache>
#include <QClipboard>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QShortcut>
#include <QListWidgetItem>

SiteInfo::SiteInfo(WebView *view)
    : QDialog(view)
    , ui(new Ui::SiteInfo)
    , m_certWidget(nullptr)
    , m_view(view)
    , m_imageReply(nullptr)
    , m_baseUrl(view->url())
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    ui->treeTags->setLayoutDirection(Qt::LeftToRight);
    QzTools::centerWidgetOnScreen(this);

    auto* delegate = new ListItemDelegate(24, ui->listWidget);
    delegate->setUpdateParentHeight(true);
    delegate->setUniformItemSizes(true);
    ui->listWidget->setItemDelegate(delegate);

    ui->listWidget->item(0)->setIcon(QIcon::fromTheme(QSL("document-properties"), QIcon(QSL(":/icons/preferences/document-properties.png"))));
    ui->listWidget->item(1)->setIcon(QIcon::fromTheme(QSL("applications-graphics"), QIcon(QSL(":/icons/preferences/applications-graphics.png"))));
    ui->listWidget->item(0)->setSelected(true);

    // General
    ui->heading->setText(QSL("<b>%1</b>:").arg(m_view->title()));
    ui->siteAddress->setText(m_view->url().toString());

    if (m_view->url().scheme() == QL1S("https"))
        ui->securityLabel->setText(tr("<b>Connection is Encrypted.</b>"));
    else
        ui->securityLabel->setText(tr("<b>Connection Not Encrypted.</b>"));

    m_view->page()->runJavaScript(QSL("document.charset"), WebPage::SafeJsWorld, [this](const QVariant &res) {
        ui->encodingLabel->setText(res.toString());
    });

    // Meta
    m_view->page()->runJavaScript(Scripts::getAllMetaAttributes(), WebPage::SafeJsWorld, [this](const QVariant &res) {
        const QVariantList &list = res.toList();
        for (const QVariant &val : list) {
            const QVariantMap &meta = val.toMap();
            QString content = meta.value(QSL("content")).toString();
            QString name = meta.value(QSL("name")).toString();

            if (name.isEmpty())
                name = meta.value(QSL("httpequiv")).toString();

            if (content.isEmpty() || name.isEmpty())
                continue;

            auto* item = new QTreeWidgetItem(ui->treeTags);
            item->setText(0, name);
            item->setText(1, content);
            ui->treeTags->addTopLevelItem(item);
        }
    });

    // Images
    m_view->page()->runJavaScript(Scripts::getAllImages(), WebPage::SafeJsWorld, [this](const QVariant &res) {
        const QVariantList &list = res.toList();
        for (const QVariant &val : list) {
            const QVariantMap &img = val.toMap();
            QString src = img.value(QSL("src")).toString();
            QString alt = img.value(QSL("alt")).toString();
            if (alt.isEmpty()) {
                if (src.indexOf(QLatin1Char('/')) == -1) {
                    alt = src;
                }
                else {
                    int pos = src.lastIndexOf(QLatin1Char('/'));
                    alt = src.mid(pos);
                    alt.remove(QLatin1Char('/'));
                }
            }

            if (src.isEmpty() || alt.isEmpty())
                continue;

            auto* item = new QTreeWidgetItem(ui->treeImages);
            item->setText(0, alt);
            item->setText(1, src);
            ui->treeImages->addTopLevelItem(item);
        }
    });

    /* Permissions */
    addSiteSettings();

    connect(ui->saveButton, SIGNAL(clicked(QAbstractButton*)), this, SLOT(saveImage()));
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(ui->treeImages, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(showImagePreview(QTreeWidgetItem*)));
    connect(ui->treeImages, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(imagesCustomContextMenuRequested(QPoint)));
    connect(ui->treeTags, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tagsCustomContextMenuRequested(QPoint)));
    connect(this, &QDialog::accepted, this, &SiteInfo::saveSiteSettings);

    auto *shortcutTagsCopyAll = new QShortcut(QKeySequence(QSL("Ctrl+C")), ui->treeTags);
    shortcutTagsCopyAll->setContext(Qt::WidgetShortcut);
    connect(shortcutTagsCopyAll, &QShortcut::activated, [=]{copySelectedItems(ui->treeTags, false);});

    auto *shortcutTagsCopyValues = new QShortcut(QKeySequence(QSL("Ctrl+Shift+C")), ui->treeTags);
    shortcutTagsCopyValues->setContext(Qt::WidgetShortcut);
    connect(shortcutTagsCopyValues, &QShortcut::activated, [=]{copySelectedItems(ui->treeTags, true);});

    auto *shortcutImagesCopyAll = new QShortcut(QKeySequence(QSL("Ctrl+C")), ui->treeImages);
    shortcutImagesCopyAll->setContext(Qt::WidgetShortcut);
    connect(shortcutImagesCopyAll, &QShortcut::activated, [=]{copySelectedItems(ui->treeImages, false);});

    auto *shortcutImagesCopyValues = new QShortcut(QKeySequence(QSL("Ctrl+Shift+C")), ui->treeImages);
    shortcutImagesCopyValues->setContext(Qt::WidgetShortcut);
    connect(shortcutImagesCopyValues, &QShortcut::activated, [=]{copySelectedItems(ui->treeImages, true);});

    ui->treeImages->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeImages->sortByColumn(-1, Qt::AscendingOrder);

    ui->treeTags->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeTags->sortByColumn(-1, Qt::AscendingOrder);

    QzTools::setWmClass(QSL("Site Info"), this);
}

bool SiteInfo::canShowSiteInfo(const QUrl &url)
{
    if (LocationBar::convertUrlToText(url).isEmpty())
        return false;

    if (url.scheme() == QL1S("falkon") || url.scheme() == QL1S("view-source") || url.scheme() == QL1S("extension"))
        return false;

    return true;
}

void SiteInfo::imagesCustomContextMenuRequested(const QPoint &p)
{
    QTreeWidgetItem* item = ui->treeImages->itemAt(p);
    if (!item) {
        return;
    }

    QMenu menu;
    menu.addAction(QIcon::fromTheme(QSL("edit-copy")), tr("Copy Image Location"), QKeySequence(QSL("Ctrl+C")), this, [=]{copySelectedItems(ui->treeImages, false);});
    menu.addAction(tr("Copy Image Name"), QKeySequence(QSL("Ctrl+Shift+C")), this, [=]{copySelectedItems(ui->treeImages, true);});
    menu.addSeparator();
    menu.addAction(QIcon::fromTheme(QSL("document-save")), tr("Save Image to Disk"), this, SLOT(saveImage()));
    menu.exec(ui->treeImages->viewport()->mapToGlobal(p));
}

void SiteInfo::tagsCustomContextMenuRequested(const QPoint &p)
{
    QTreeWidgetItem* item = ui->treeTags->itemAt(p);
    if (!item) {
        return;
    }

    QMenu menu;
    menu.addAction(tr("Copy Values"), QKeySequence(QSL("Ctrl+C")), this, [=]{copySelectedItems(ui->treeTags, false);});
    menu.addAction(tr("Copy Tags and Values"), QKeySequence(QSL("Ctrl+Shift+C")), this, [=]{copySelectedItems(ui->treeTags, true);});
    menu.exec(ui->treeTags->viewport()->mapToGlobal(p));
}

void SiteInfo::copySelectedItems(const QTreeWidget* treeWidget, const bool both)
{
    QList<QTreeWidgetItem*> itemList = treeWidget->selectedItems();
    QString tmpText = QSL("");

    for (int i = 0; i < itemList.size(); ++i) {
        if (i != 0) {
            tmpText.append(QSL("\n"));
        }
        if (both) {
            tmpText.append((itemList[i])->text(0));
            tmpText.append(QSL("\t"));
        }
        tmpText.append((itemList[i])->text(1));
    }
    qApp->clipboard()->setText(tmpText);
}

void SiteInfo::saveImage()
{
    QTreeWidgetItem* item = ui->treeImages->currentItem();
    if (!item) {
        return;
    }

    if (!ui->mediaPreview->scene() || ui->mediaPreview->scene()->items().isEmpty())
        return;

    QGraphicsItem *graphicsItem = ui->mediaPreview->scene()->items().at(0);
    auto *pixmapItem = static_cast<QGraphicsPixmapItem*>(graphicsItem);
    if (graphicsItem->type() != QGraphicsPixmapItem::Type || !pixmapItem)
        return;

    if (!pixmapItem || pixmapItem->pixmap().isNull()) {
        QMessageBox::warning(this, tr("Error!"), tr("This preview is not available!"));
        return;
    }

    QString imageFileName = QzTools::getFileNameFromUrl(QUrl(item->text(1)));
    int index = imageFileName.lastIndexOf(QLatin1Char('.'));
    if (index != -1) {
        imageFileName.truncate(index);
        imageFileName.append(QL1S(".png"));
    }

    QString filePath = QzTools::getSaveFileName(QSL("SiteInfo-DownloadImage"), this, tr("Save image..."),
                                                QDir::homePath() + QDir::separator() + imageFileName,
                                                QSL("*.png"));
    if (filePath.isEmpty()) {
        return;
    }

    if (!pixmapItem->pixmap().save(filePath, "PNG")) {
        QMessageBox::critical(this, tr("Error!"), tr("Cannot write to file!"));
        return;
    }
}

void SiteInfo::showLoadingText()
{
    delete ui->mediaPreview->scene();
    auto* scene = new QGraphicsScene(ui->mediaPreview);

    scene->addText(tr("Loading..."));

    ui->mediaPreview->setScene(scene);
}

void SiteInfo::showPixmap(QPixmap pixmap)
{
    pixmap.setDevicePixelRatio(devicePixelRatioF());

    delete ui->mediaPreview->scene();
    auto* scene = new QGraphicsScene(ui->mediaPreview);

    if (pixmap.isNull())
        scene->addText(tr("Preview not available"));
    else
        scene->addPixmap(pixmap);

    ui->mediaPreview->setScene(scene);
}

void SiteInfo::showImagePreview(QTreeWidgetItem *item)
{
    if ((!item) || (item->treeWidget()->selectedItems().length() > 1)) {
        return;
    }
    QUrl imageUrl = QUrl::fromEncoded(item->text(1).toUtf8());
    if (imageUrl.isRelative()) {
        imageUrl = m_baseUrl.resolved(imageUrl);
    }

    QPixmap pixmap;
    bool loading = false;

    if (imageUrl.scheme() == QLatin1String("data")) {
        QByteArray encodedUrl = item->text(1).toUtf8();
        QByteArray imageData = encodedUrl.mid(encodedUrl.indexOf(',') + 1);
        pixmap = QzTools::pixmapFromByteArray(imageData);
    }
    else if (imageUrl.scheme() == QLatin1String("file")) {
        pixmap = QPixmap(imageUrl.toLocalFile());
    }
    else if (imageUrl.scheme() == QLatin1String("qrc")) {
        pixmap = QPixmap(imageUrl.toString().mid(3)); // Remove qrc from url
    }
    else {
        delete m_imageReply;
        m_imageReply = mApp->networkManager()->get(QNetworkRequest(imageUrl));
        connect(m_imageReply, &QNetworkReply::finished, this, [this]() {
            if (m_imageReply->error() != QNetworkReply::NoError)
                return;

            const QByteArray &data = m_imageReply->readAll();
            showPixmap(QPixmap::fromImage(QImage::fromData(data)));
        });

        loading = true;
        showLoadingText();
    }

    if (!loading)
        showPixmap(pixmap);
}

SiteInfo::~SiteInfo()
{
    delete ui;
    delete m_certWidget;
}

SiteInfoPermissionItem* SiteInfo::addPermissionOption(SiteSettingsManager::Permission perm)
{
    auto* listItem = new QListWidgetItem(ui->listPermissions);
    auto* optionItem = new SiteInfoPermissionItem(perm, this);

    ui->listPermissions->setItemWidget(listItem, optionItem);
    listItem->setSizeHint(optionItem->sizeHint());

    return optionItem;
}

void SiteInfo::addSiteSettings()
{
    auto siteSettings = mApp->siteSettingsManager()->getSiteSettings(m_baseUrl, mApp->isPrivate());
    const auto supportedAttribute = mApp->siteSettingsManager()->getSupportedAttribute();
    for (const auto &attribute : supportedAttribute) {
        SiteInfoPermissionItem *item = addPermissionOption(siteSettings.attributes[attribute]);
        item->setAttribute(attribute);
    }
    const auto supportedFeatures = mApp->siteSettingsManager()->getSupportedFeatures();
    for (const auto &feature : supportedFeatures) {
        SiteInfoPermissionItem *item = addPermissionOption(siteSettings.features[feature]);
        item->setFeature(feature);
    }
    SiteInfoPermissionItem *item = addPermissionOption(siteSettings.AllowCookies);
    item->setOption(SiteSettingsManager::poAllowCookies);
}

void SiteInfo::saveSiteSettings()
{
    SiteSettings siteSettings;
    int index = 0;
    auto supportedAttribute = mApp->siteSettingsManager()->getSupportedAttribute();
    auto supportedFeatures = mApp->siteSettingsManager()->getSupportedFeatures();

    for (int i = 0; i < supportedAttribute.size(); ++i, ++index) {
        auto* item = static_cast<SiteInfoPermissionItem*>(ui->listPermissions->itemWidget(ui->listPermissions->item(index)));
        siteSettings.attributes[supportedAttribute[i]] = item->permission();
    }
    for (int i = 0; i < supportedFeatures.size(); ++i, ++index) {
        auto* item = static_cast<SiteInfoPermissionItem*>(ui->listPermissions->itemWidget(ui->listPermissions->item(index)));
        siteSettings.features[supportedFeatures[i]] = item->permission();
    }
    auto* item = static_cast<SiteInfoPermissionItem*>(ui->listPermissions->itemWidget(ui->listPermissions->item(index++)));
    siteSettings.AllowCookies = item->permission();
    siteSettings.ZoomLevel = -1;

    siteSettings.server = m_baseUrl.host();

    if (!(siteSettings == mApp->siteSettingsManager()->getSiteSettings(m_baseUrl, mApp->isPrivate()))) {
        mApp->siteSettingsManager()->setSiteSettings(siteSettings);
    }
}
