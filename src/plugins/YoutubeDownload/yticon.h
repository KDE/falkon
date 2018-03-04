#ifndef YTICON_H
#define YTICON_H

#include <abstractbuttoninterface.h>

class YtIcon : public AbstractButtonInterface
{
    Q_OBJECT

public:
    explicit YtIcon(QObject *parent = nullptr);

    QString id() const override;
    QString name() const override;

private:
    void updateState();
    void clicked(ClickController *controller);
};

#endif //YTICON_H
