#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include <QWidget>
#include "deviceconfig.h"

namespace Ui {
class AdvancedSettings;
}

class AdvancedSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedSettings(QWidget *parent = nullptr);
    ~AdvancedSettings();

private:
    Ui::AdvancedSettings *ui;

public slots:   // private?
    void ReadFromConfig();
    void WriteToConfig();
};

#endif // ADVANCEDSETTINGS_H
