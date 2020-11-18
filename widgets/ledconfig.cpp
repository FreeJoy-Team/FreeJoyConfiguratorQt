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
    for (int i = 0; i < MAX_LEDS_NUM; i++)
    {
        LED * led = new LED(i, this);
        ui->layoutV_LED->addWidget(led);
        LEDPtrList_.append(led);
        led->hide();
    }
}

LedConfig::~LedConfig()
{
    delete ui;
}

void LedConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < LEDPtrList_.size(); ++i) {
        LEDPtrList_[i]->RetranslateUi();
    }
}

void LedConfig::SpawnLEDs(int led_count)
{
    for (int i = 0; i < MAX_LEDS_NUM; i++)  // или проверка на скрытие и break; ?
    {
        LEDPtrList_[i]->hide();
    }
    for (int i = 0; i < led_count; i++)
    {
        LEDPtrList_[i]->show();
    }
}

void LedConfig::LedStateChanged()
{

    for (int i = 0; gEnv.pDeviceConfig->config.leds[i].input_num > -1; ++i) // нахуевертил побыстрому, позже допилю
    {
        if (i >= LEDPtrList_.size()){
            break;
        }
        if (LEDPtrList_[i]->CurrentButtonSelected() == gEnv.pDeviceConfig->config.leds[i].input_num) {
            // logical buttons state
            int index = gEnv.pDeviceConfig->config.leds[i].input_num / 8;
            int bit = gEnv.pDeviceConfig->config.leds[i].input_num - index * 8;

            if ((gEnv.pDeviceConfig->gamepad_report.button_data[index] & (1 << (bit & 0x07))))
            {
                LEDPtrList_[i]->LEDStateChanged(true);
            }
            else if ((gEnv.pDeviceConfig->gamepad_report.button_data[index] & (1 << (bit & 0x07))) == false)
            {
                LEDPtrList_[i]->LEDStateChanged(false);
            }
        }
    }
}

void LedConfig::ReadFromConfig()
{
    ui->spinBox_LedPB0->setValue(gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[0]);
    ui->spinBox_LedPB1->setValue(gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[1]);
    ui->spinBox_LedPB4->setValue(gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[2]);

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        LEDPtrList_[i]->ReadFromConfig();
    }
}

void LedConfig::WriteToConfig()
{
    gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[0] = ui->spinBox_LedPB0->value();
    gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[1] = ui->spinBox_LedPB1->value();
    gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[2] = ui->spinBox_LedPB4->value();

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        if (LEDPtrList_[i]->isHidden()){
            break;
        }
        LEDPtrList_[i]->WriteToConfig();
    }
}
