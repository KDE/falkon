#ifndef TABMANAGERSETTINGS_H
#define TABMANAGERSETTINGS_H

#include <QDialog>

namespace Ui {
class TabManagerSettings;
}
class TabManagerPlugin;

class TabManagerSettings : public QDialog
{
    Q_OBJECT

public:
    explicit TabManagerSettings(TabManagerPlugin* plugin, QWidget *parent = 0);
    ~TabManagerSettings() override;

public Q_SLOTS:
    void accept() override;

private:
    Ui::TabManagerSettings* ui;
    TabManagerPlugin* m_plugin;
};

#endif // TABMANAGERSETTINGS_H
