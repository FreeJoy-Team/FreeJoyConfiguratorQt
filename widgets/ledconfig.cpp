#include "ledconfig.h"
#include "ui_ledconfig.h"

//#include <time.h>

#include "common_types.h"
#include "global.h"
#include "deviceconfig.h"

#include <QDebug>
LedConfig::LedConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LedConfig)
{
    ui->setupUi(this);
    ui->layoutV_LED->setAlignment(Qt::AlignTop);

    // LEDs spawn and hide
    //clock_t start = clock();
    for (int i = 0; i < MAX_LEDS_NUM; i++)
    {
        LED * led = new LED(i, this);       // мб нах new?
        ui->layoutV_LED->addWidget(led);
        LEDAdrList.append(led);
        led->hide();
    }
    //clock_t stop = clock();
    //qDebug()<< "LEDs create ms= " << stop - start;
}

LedConfig::~LedConfig()
{
    delete ui;
}

void LedConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < LEDAdrList.size(); ++i) {
        LEDAdrList[i]->RetranslateUi();
    }
}

void LedConfig::SpawnLEDs(int led_count)
{
    for (int i = 0; i < MAX_LEDS_NUM; i++)  // или проверка на скрытие и break; ?
    {
        LEDAdrList[i]->hide();
    }
    for (int i = 0; i < led_count; i++)
    {
        LEDAdrList[i]->show();
    }
}

//void LedConfig::ButtonLEDStateChanged()
//{
//    if (LEDAdrList.size() > 0){

//    }
//}

void LedConfig::ReadFromConfig()
{
    ui->spinBox_LedPB0->setValue(gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[0]);
    ui->spinBox_LedPB1->setValue(gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[1]);
    ui->spinBox_LedPB4->setValue(gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[2]);

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        LEDAdrList[i]->ReadFromConfig();
    }
}

void LedConfig::WriteToConfig()
{
    gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[0] = ui->spinBox_LedPB0->value();
    gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[1] = ui->spinBox_LedPB1->value();
    gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[2] = ui->spinBox_LedPB4->value();

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        if (LEDAdrList[i]->isHidden()){
            break;
        }
        LEDAdrList[i]->WriteToConfig();
    }
}
