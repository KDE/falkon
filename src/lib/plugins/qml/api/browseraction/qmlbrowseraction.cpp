#include "qmlbrowseraction.h"
#include "mainapplication.h"
#include <QQuickWindow>

QmlBrowserAction::QmlBrowserAction(QObject *parent)
    : AbstractButtonInterface(parent)
    , m_popup(nullptr)
{
    connect(this, &AbstractButtonInterface::clicked, this, &QmlBrowserAction::clicked);
}

QString QmlBrowserAction::id() const
{
    return m_id;
}

void QmlBrowserAction::setId(const QString &id)
{
    m_id = id;
}

QString QmlBrowserAction::name() const
{
    return m_name;
}

void QmlBrowserAction::setName(const QString &name)
{
    m_name = name;
}

QString QmlBrowserAction::iconUrl() const
{
    return m_iconUrl;
}

void QmlBrowserAction::setIconUrl(const QString &iconUrl)
{
    m_iconUrl = iconUrl;
    QString fileName = QUrl(m_iconUrl.toUtf8()).toLocalFile();
    setIcon(QIcon(fileName));
}

QQmlComponent* QmlBrowserAction::popup() const
{
    return m_popup;
}

void QmlBrowserAction::setPopup(QQmlComponent* popup)
{
    m_popup = popup;
}

QmlBrowserAction::LocationFlags QmlBrowserAction::location() const
{
    return m_displayFlags;
}

void QmlBrowserAction::setLocation(const LocationFlags &locationFlags)
{
    m_displayFlags = locationFlags;
    emit locationChanged();
}

void QmlBrowserAction::clicked(ClickController *clickController)
{
    if (!m_popup) {
        qWarning() << "No popup to show";
        return;
    }

    QQuickWindow *quickWindow = dynamic_cast<QQuickWindow*>(m_popup->create());
    if (!quickWindow) {
        qWarning() << "Cannot create QQuickWindow from popup";
        return;
    }
    quickWindow->setFlags(Qt::Popup);
    quickWindow->setPosition(clickController->callPopupPosition(quickWindow->size()));

    connect(quickWindow, &QQuickWindow::activeChanged, this, [quickWindow, clickController]{
        if (!quickWindow->isActive()) {
            quickWindow->destroy();
            clickController->callPopupClosed();
        }
    });

    quickWindow->show();
    quickWindow->requestActivate();
}
