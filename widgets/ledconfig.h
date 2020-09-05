#ifndef LEDCONFIG_H
#define LEDCONFIG_H

#include <QWidget>
#include "led.h"

namespace Ui {
class LedConfig;
}

class LedConfig : public QWidget
{
    Q_OBJECT

public:
    explicit LedConfig(QWidget *parent = nullptr);
    ~LedConfig();

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

    //void ButtonLEDStateChanged();     // future

public slots:
    void SpawnLEDs(int led_count);

private:
    Ui::LedConfig *ui;

    QList<LED*> LEDAdrList;
    int current_led_count_;
};

#endif // LEDCONFIG_H
