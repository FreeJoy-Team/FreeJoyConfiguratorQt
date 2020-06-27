#include "led.h"
#include "ui_led.h"

LED::LED(int led_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LED)
{
    ui->setupUi(this);
    led_number_ = led_number;
    ui->label_LEDNumber->setNum(led_number + 1);

    for (int i = 0; i < LED_FUNCTION_COUNT; ++i) {
        ui->comboBox_Function->addItem(led_list_[i].gui_name);
    }
}

LED::~LED()
{
    delete ui;
}

void LED::ReadFromConfig()
{
    ui->spinBox_InputNumber->setValue(gEnv.pDeviceConfig->config.leds[led_number_].input_num);
    ui->comboBox_Function->setCurrentIndex(gEnv.pDeviceConfig->config.leds[led_number_].type);
}

void LED::WriteToConfig()
{
    gEnv.pDeviceConfig->config.leds[led_number_].input_num = ui->spinBox_InputNumber->value();
    gEnv.pDeviceConfig->config.leds[led_number_].type = ui->comboBox_Function->currentIndex();
}
