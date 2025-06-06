/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2014-2018 David Rosca <nowrep@gmail.com>
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
#include "bookmarkstoolbarbutton.h"
#include "bookmarkstools.h"
#include "bookmarksmodel.h"
#include "bookmarkitem.h"
#include "bookmarks.h"
#include "mainapplication.h"
#include "enhancedmenu.h"
#include "statusbar.h"

#include <QStyle>
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOptionButton>
#include <QDrag>
#include <QMimeData>
#include <QtGuiVersion>

#define MAX_WIDTH 150
#define SEPARATOR_WIDTH 8
#define PADDING 5

BookmarksToolbarButton::BookmarksToolbarButton(BookmarkItem* bookmark, QWidget* parent)
    : QPushButton(parent)
    , m_bookmark(bookmark)
    , m_window(nullptr)
    , m_showOnlyIcon(false)
{
    init();

    if (m_bookmark->isFolder()) {
        setAcceptDrops(true);
    }
}

BookmarkItem* BookmarksToolbarButton::bookmark() const
{
    return m_bookmark;
}

void BookmarksToolbarButton::setMainWindow(BrowserWindow* window)
{
    m_window = window;
}

bool BookmarksToolbarButton::showOnlyIcon() const
{
    return m_showOnlyIcon;
}

void BookmarksToolbarButton::setShowOnlyIcon(bool show)
{
    m_showOnlyIcon = show;
    updateGeometry();
    update();
}

bool BookmarksToolbarButton::showOnlyText() const
{
    return m_showOnlyText;
}

void BookmarksToolbarButton::setShowOnlyText(bool show)
{
    m_showOnlyText = show;
    updateGeometry();
    update();
}

QSize BookmarksToolbarButton::sizeHint() const
{
    int width = PADDING * 2;
    if (!m_showOnlyText) {
        width += 16;
    }

    if (m_bookmark->isSeparator()) {
        width = SEPARATOR_WIDTH;
    }
    else if (!m_showOnlyIcon) {
        width += PADDING * 2 + fontMetrics().horizontalAdvance(m_bookmark->title());

        if (menu()) {
            width += PADDING + 8;
        }
    }

    QSize s = QPushButton::sizeHint();
    s.setWidth(qMin(width, MAX_WIDTH));
    return s;
}

QSize BookmarksToolbarButton::minimumSizeHint() const
{
    int width = PADDING * 2;
    if (!m_showOnlyText) {
        width += 16;
    }

    if (m_bookmark->isSeparator()) {
        width = SEPARATOR_WIDTH;
    }
    else if (!m_showOnlyIcon && menu()) {
        width += PADDING + 8;
    }

    QSize s = QPushButton::minimumSizeHint();
    s.setWidth(width);
    return s;
}

void BookmarksToolbarButton::createMenu()
{
    if (!menu()->isEmpty()) {
        return;
    }

    Menu* m = qobject_cast<Menu*>(menu());
    Q_ASSERT(m);
    QObject::connect(m, &QMenu::aboutToHide, [=]() {
        mApp->getWindow()->statusBar()->clearMessage();
    });

    BookmarksTools::addFolderContentsToMenu(this, m, m_bookmark);
}

void BookmarksToolbarButton::menuAboutToShow()
{
    Q_ASSERT(qobject_cast<Menu*>(sender()));
    Menu *menu = static_cast<Menu*>(sender());

    const auto menuActions = menu->actions();
    for (QAction *action : menuActions) {
        BookmarkItem *item = static_cast<BookmarkItem*>(action->data().value<void*>());
        if (item && item->type() == BookmarkItem::Url && action->icon().isNull()) {
            action->setIcon(item->icon());
        }
    }
}

void BookmarksToolbarButton::menuMiddleClicked(Menu* menu)
{
    BookmarkItem* item = static_cast<BookmarkItem*>(menu->menuAction()->data().value<void*>());
    Q_ASSERT(item);
    openFolder(item);
}

void BookmarksToolbarButton::bookmarkActivated(BookmarkItem* item)
{
    if (auto* action = qobject_cast<QAction*>(sender())) {
        item = static_cast<BookmarkItem*>(action->data().value<void*>());
    }

    Q_ASSERT(item);
    openBookmark(item);
}

void BookmarksToolbarButton::bookmarkCtrlActivated(BookmarkItem* item)
{
    if (auto* action = qobject_cast<QAction*>(sender())) {
        item = static_cast<BookmarkItem*>(action->data().value<void*>());
    }

    Q_ASSERT(item);
    openBookmarkInNewTab(item);
}

void BookmarksToolbarButton::bookmarkShiftActivated(BookmarkItem* item)
{
    if (auto* action = qobject_cast<QAction*>(sender())) {
        item = static_cast<BookmarkItem*>(action->data().value<void*>());
    }

    Q_ASSERT(item);
    openBookmarkInNewWindow(item);
}

void BookmarksToolbarButton::openFolder(BookmarkItem* item)
{
    Q_ASSERT(item->isFolder());

    if (m_window) {
        BookmarksTools::openFolderInTabs(m_window, item);
    }
}

void BookmarksToolbarButton::openBookmark(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    if (m_window) {
        BookmarksTools::openBookmark(m_window, item);
    }
}

void BookmarksToolbarButton::openBookmarkInNewTab(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    if (m_window) {
        BookmarksTools::openBookmarkInNewTab(m_window, item);
    }
}

void BookmarksToolbarButton::openBookmarkInNewWindow(BookmarkItem* item)
{
    Q_ASSERT(item->isUrl());

    BookmarksTools::openBookmarkInNewWindow(item);
}

void BookmarksToolbarButton::init()
{
    Q_ASSERT(m_bookmark);

    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    setToolTip(createTooltip());

    if (m_bookmark->isFolder()) {
        Menu* m = new Menu(this);
        setMenu(m);
        createMenu();
    }
}

QString BookmarksToolbarButton::createTooltip() const
{
    if (!m_bookmark->description().isEmpty()) {
        if (!m_bookmark->urlString().isEmpty()) {
            return QSL("%1\n%2").arg(m_bookmark->description(), m_bookmark->urlString());
        }
        return m_bookmark->description();
    }

    if (!m_bookmark->title().isEmpty() && !m_bookmark->url().isEmpty()) {
        return QSL("%1\n%2").arg(m_bookmark->title(), m_bookmark->urlString());
    }

    if (!m_bookmark->title().isEmpty()) {
        return m_bookmark->title();
    }

    return m_bookmark->urlString();
}

void BookmarksToolbarButton::enterEvent(QEnterEvent* event)
{
    QPushButton::enterEvent(event);

    mApp->getWindow()->statusBar()->showMessage(m_bookmark->url().toString());

    update();
}

void BookmarksToolbarButton::leaveEvent(QEvent* event)
{
    QPushButton::leaveEvent(event);

    mApp->getWindow()->statusBar()->clearMessage();

    update();
}

void BookmarksToolbarButton::mousePressEvent(QMouseEvent* event)
{
    if (m_bookmark && m_bookmark->isFolder()) {
        if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
            openFolder(m_bookmark);
            return;
        }
    }

    m_dragStartPosition = event->position().toPoint();

    QPushButton::mousePressEvent(event);
}

void BookmarksToolbarButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_bookmark && rect().contains(event->position().toPoint())) {
        Qt::MouseButton button = event->button();
        Qt::KeyboardModifiers modifiers = event->modifiers();

        if (m_bookmark->isUrl()) {
            if (button == Qt::LeftButton && modifiers == Qt::NoModifier) {
                bookmarkActivated(m_bookmark);
            }
            else if (button == Qt::LeftButton && modifiers == Qt::ShiftModifier) {
                bookmarkShiftActivated(m_bookmark);
            }
            else if (button == Qt::MiddleButton || modifiers == Qt::ControlModifier) {
                bookmarkCtrlActivated(m_bookmark);
            }
        }
        else if (m_bookmark->isFolder() && button == Qt::MiddleButton) {
            openFolder(m_bookmark);
        }
    }

    QPushButton::mouseReleaseEvent(event);
}

void BookmarksToolbarButton::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->position().toPoint() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        QPushButton::mouseMoveEvent(event);
        return;
    }

    setDown(false);

    auto *drag = new QDrag(this);
    auto* mime = new BookmarksButtonMimeData;
    mime->setBookmarkItem(m_bookmark);
    drag->setMimeData(mime);
    drag->setPixmap(grab());
    drag->exec();
}

void BookmarksToolbarButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter p(this);

    // Just draw separator
    if (m_bookmark->isSeparator()) {
        QStyleOption opt;
        opt.initFrom(this);
        opt.state |= QStyle::State_Horizontal;
        style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p);
        return;
    }

    QStyleOptionButton option;
    initStyleOption(&option);

    // We are manually drawing the arrow
    option.features &= ~QStyleOptionButton::HasMenu;

    // Draw button base (only under mouse, this is autoraise button)
    if (isDown() || hitButton(mapFromGlobal(QCursor::pos()))) {
        option.state |= QStyle::State_AutoRaise | QStyle::State_Raised;
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &option, &p, this);
    }

    const int shiftX = isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &option, this) : 0;
    const int shiftY = isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &option, this) : 0;

    const int height = option.rect.height();
    const int center = height / 2 + option.rect.top() + shiftY;

    const int iconSize = 16;
    const int iconYPos = center - iconSize / 2;

    int leftPosition = PADDING + shiftX;
    int rightPosition = option.rect.right() - PADDING;

    // Draw icon
    if (!m_showOnlyText) {
        QRect iconRect(leftPosition, iconYPos, iconSize, iconSize);
        p.drawPixmap(QStyle::visualRect(option.direction, option.rect, iconRect), m_bookmark->icon().pixmap(iconSize));
        leftPosition = iconRect.right() + PADDING;
    }

    // Draw menu arrow
    if (!m_showOnlyIcon && menu()) {
        const int arrowSize = 8;
        QStyleOption opt;
        opt.initFrom(this);
        const QRect rect = QRect(rightPosition - 8, center - arrowSize / 2, arrowSize, arrowSize);
        opt.rect = QStyle::visualRect(option.direction, option.rect, rect);
        opt.state &= ~QStyle::State_MouseOver;
        style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, &p, this);
        rightPosition = rect.left() - PADDING;
    }

    // Draw text
    if (!m_showOnlyIcon) {
        const int textWidth = rightPosition - leftPosition;
        const int textYPos = center - fontMetrics().height() / 2;
        const QString txt = fontMetrics().elidedText(m_bookmark->title(), Qt::ElideRight, textWidth);
        QRect textRect(leftPosition, textYPos, textWidth, fontMetrics().height());
        style()->drawItemText(&p, QStyle::visualRect(option.direction, option.rect, textRect),
                              Qt::TextSingleLine | Qt::AlignCenter, option.palette, true, txt);
    }
}

void BookmarksToolbarButton::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mime = event->mimeData();
    if ((mime->hasUrls() && mime->hasText()) || mime->hasFormat(BookmarksButtonMimeData::mimeType())) {
        event->acceptProposedAction();
        setDown(true);
        return;
    }

    QPushButton::dragEnterEvent(event);
}

void BookmarksToolbarButton::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    setDown(false);
}

void BookmarksToolbarButton::dropEvent(QDropEvent *event)
{
    setDown(false);

    const QMimeData* mime = event->mimeData();
    if (!mime->hasUrls() && !mime->hasFormat(BookmarksButtonMimeData::mimeType())) {
        QPushButton::dropEvent(event);
        return;
    }

    BookmarkItem* bookmark = nullptr;

    if (mime->hasFormat(BookmarksButtonMimeData::mimeType())) {
        const auto* bookmarkMime = static_cast<const BookmarksButtonMimeData*>(mime);
        bookmark = bookmarkMime->item();

        if (m_bookmark == bookmark) {
            return;
        }
    } else {
        const QUrl url = mime->urls().at(0);
        const QString title = mime->hasText() ? mime->text() : QString::fromUtf8(url.toEncoded(QUrl::RemoveScheme));

        bookmark = new BookmarkItem(BookmarkItem::Url);
        bookmark->setTitle(title);
        bookmark->setUrl(url);
    }

    mApp->bookmarks()->addBookmark(m_bookmark, bookmark);
}
