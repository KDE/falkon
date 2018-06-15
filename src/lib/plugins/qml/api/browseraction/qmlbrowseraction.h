#pragma once
#include "abstractbuttoninterface.h"
#include <QQmlComponent>

class QmlBrowserAction : public AbstractButtonInterface
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ id WRITE setId)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip)
    Q_PROPERTY(QString icon READ iconUrl WRITE setIconUrl)
    Q_PROPERTY(QString badgeText READ badgeText WRITE setBadgeText)
    Q_PROPERTY(QQmlComponent* popup READ popup WRITE setPopup)
    Q_PROPERTY(LocationFlags location READ location WRITE setLocation NOTIFY locationChanged)

public:
    enum Location {
        NavigationToolBar = 0x1,
        StatusBar = 0x2
    };
    Q_DECLARE_FLAGS(LocationFlags, Location)
    Q_ENUMS(LocationFlags)

    explicit QmlBrowserAction(QObject *parent = nullptr);
    QString id() const;
    void setId(const QString &id);
    QString name() const;
    void setName(const QString &name);
    QString iconUrl() const;
    void setIconUrl(const QString &iconUrl);
    QQmlComponent* popup() const;
    void setPopup(QQmlComponent* popup);
    LocationFlags location() const;
    void setLocation(const LocationFlags &locationFlags);

Q_SIGNALS:
    void locationChanged();

private:
    QString m_id;
    QString m_name;
    QString m_iconUrl;
    QQmlComponent* m_popup;
    LocationFlags m_displayFlags;

    void clicked(ClickController *clickController);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QmlBrowserAction::LocationFlags)
