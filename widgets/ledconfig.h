#ifndef LEDCONFIG_H
#define LEDCONFIG_H

#include <QWidget>
#include <QDebug>
#include "common_types.h"

#include "global.h"
#include "deviceconfig.h"

namespace Ui {
class LedConfig;
}

class LedConfig : public QWidget
{
    Q_OBJECT

public:
    explicit LedConfig(QWidget *parent = nullptr);
    ~LedConfig();

//    signals:
//    void valueChanged(int led_count);

//private slots:
//    void SpawnLEDs(int led_count);


private:
    Ui::LedConfig *ui;
};

#endif // LEDCONFIG_H
