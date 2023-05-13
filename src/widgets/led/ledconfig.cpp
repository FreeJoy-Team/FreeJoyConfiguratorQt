#include "ledconfig.h"
#include "ui_ledconfig.h"

//#include <time.h>
#include "axes.h"
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
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        ui->comboBox_axisConPA8->addItem(axesList()[i].guiName);
        ui->comboBox_axisConPB0->addItem(axesList()[i].guiName);
        ui->comboBox_axisConPB1->addItem(axesList()[i].guiName);
        ui->comboBox_axisConPB4->addItem(axesList()[i].guiName);
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
    if (ledCount > MAX_LEDS_NUM) return;

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

            if ((gEnv.pDeviceConfig->paramsReport.log_button_data[index] & (1 << (bit & 0x07)))) {
                m_ledPtrList[i]->setLedState(true);
            } else if ((gEnv.pDeviceConfig->paramsReport.log_button_data[index] & (1 << (bit & 0x07))) == false) {
                m_ledPtrList[i]->setLedState(false);
            }
        }
    }
}

void LedConfig::readFromConfig()
{
    led_pwm_config_t *pwm = gEnv.pDeviceConfig->config.led_pwm_config;
    ui->spinBox_LedPA8->setValue(pwm[0].duty_cycle);
    ui->spinBox_LedPB0->setValue(pwm[1].duty_cycle);
    ui->spinBox_LedPB1->setValue(pwm[2].duty_cycle);
    ui->spinBox_LedPB4->setValue(pwm[3].duty_cycle);

    ui->checkBox_connectedPA8->setChecked(pwm[0].is_axis);
    ui->checkBox_connectedPB0->setChecked(pwm[1].is_axis);
    ui->checkBox_connectedPB1->setChecked(pwm[2].is_axis);
    ui->checkBox_connectedPB4->setChecked(pwm[3].is_axis);

    ui->comboBox_axisConPA8->setCurrentIndex(pwm[0].axis_num);
    ui->comboBox_axisConPB0->setCurrentIndex(pwm[1].axis_num);
    ui->comboBox_axisConPB1->setCurrentIndex(pwm[2].axis_num);
    ui->comboBox_axisConPB4->setCurrentIndex(pwm[3].axis_num);

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        m_ledPtrList[i]->readFromConfig();
    }
}

void LedConfig::writeToConfig()
{
    led_pwm_config_t *pwm = gEnv.pDeviceConfig->config.led_pwm_config;
    pwm[0].duty_cycle = ui->spinBox_LedPA8->value();
    pwm[1].duty_cycle = ui->spinBox_LedPB0->value();
    pwm[2].duty_cycle = ui->spinBox_LedPB1->value();
    pwm[3].duty_cycle = ui->spinBox_LedPB4->value();

    pwm[0].is_axis = ui->checkBox_connectedPA8->isChecked();
    pwm[1].is_axis = ui->checkBox_connectedPB0->isChecked();
    pwm[2].is_axis = ui->checkBox_connectedPB1->isChecked();
    pwm[3].is_axis = ui->checkBox_connectedPB4->isChecked();

    pwm[0].axis_num = ui->comboBox_axisConPA8->currentIndex();
    pwm[1].axis_num = ui->comboBox_axisConPB0->currentIndex();
    pwm[2].axis_num = ui->comboBox_axisConPB1->currentIndex();
    pwm[3].axis_num = ui->comboBox_axisConPB4->currentIndex();

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        if (m_ledPtrList[i]->isHidden()) {
            break;
        }
        m_ledPtrList[i]->writeToConfig();
    }
}
