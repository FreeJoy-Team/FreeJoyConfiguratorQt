#include "ledconfig.h"
#include "ui_ledconfig.h"

//#include <time.h>

#include "common_types.h"
#include "deviceconfig.h"
#include "global.h"

#include <QDebug>
LedConfig::LedConfig(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LedConfig)
{
    ui->setupUi(this);
    ui->layoutV_LED->setAlignment(Qt::AlignTop);

    // LEDs spawn and hide
    for (int i = 0; i < MAX_LEDS_NUM; i++) {
        LED *led = new LED(i, this);
        ui->layoutV_LED->addWidget(led);
        m_ledPtrList.append(led);
        led->hide();
    }
}

LedConfig::~LedConfig()
{
    delete ui;
}

void LedConfig::retranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < m_ledPtrList.size(); ++i) {
        m_ledPtrList[i]->retranslateUi();
    }
}

void LedConfig::spawnLeds(int ledCount)
{
    for (int i = 0; i < MAX_LEDS_NUM; i++) // или проверка на скрытие и break; ?
    {
        m_ledPtrList[i]->hide();
    }
    for (int i = 0; i < ledCount; i++) {
        m_ledPtrList[i]->show();
    }
}

void LedConfig::setLedsState()
{
    for (int i = 0; gEnv.pDeviceConfig->config.leds[i].input_num > -1; ++i) // можно улучшить
    {
        if (i >= m_ledPtrList.size()) {
            break;
        }
        if (m_ledPtrList[i]->currentButtonSelected() == gEnv.pDeviceConfig->config.leds[i].input_num) {
            // logical buttons state
            int index = gEnv.pDeviceConfig->config.leds[i].input_num / 8;
            int bit = gEnv.pDeviceConfig->config.leds[i].input_num - index * 8;

            if ((gEnv.pDeviceConfig->gamepadReport.button_data[index] & (1 << (bit & 0x07)))) {
                m_ledPtrList[i]->setLedState(true);
            } else if ((gEnv.pDeviceConfig->gamepadReport.button_data[index] & (1 << (bit & 0x07))) == false) {
                m_ledPtrList[i]->setLedState(false);
            }
        }
    }
}

void LedConfig::readFromConfig()
{
    ui->spinBox_LedPB0->setValue(gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[0]);
    ui->spinBox_LedPB1->setValue(gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[1]);
    ui->spinBox_LedPB4->setValue(gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[2]);

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        m_ledPtrList[i]->readFromConfig();
    }
}

void LedConfig::writeToConfig()
{
    gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[0] = ui->spinBox_LedPB0->value();
    gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[1] = ui->spinBox_LedPB1->value();
    gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[2] = ui->spinBox_LedPB4->value();

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        if (m_ledPtrList[i]->isHidden()) {
            break;
        }
        m_ledPtrList[i]->writeToConfig();
    }
}
