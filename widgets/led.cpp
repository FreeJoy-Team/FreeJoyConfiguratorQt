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

void LED::RetranslateUi()
{
    ui->retranslateUi(this);
}

int LED::CurrentButtonSelected() const
{
    return ui->spinBox_InputNumber->value() - 1;
}

void LED::LEDStateChanged(bool set_state)
{
//    static QPalette default_palette;
    static QString default_style;

    if (set_state != current_state_)
    {
//        this->setAutoFillBackground(true);

        if (set_state){
//            default_palette = this->palette();
            default_style = ui->label_LEDNumber->styleSheet();

//            this->setPalette(QPalette(QPalette::Window, QColor(0, 128, 0)));
            ui->label_LEDNumber->setStyleSheet(default_style + "background-color: rgb(0, 128, 0);");
        } else {
//            this->setPalette(default_palette);
            ui->label_LEDNumber->setStyleSheet(default_style);
        }
        current_state_ = set_state;
    }
}


void LED::ReadFromConfig()
{
    ui->spinBox_InputNumber->setValue(gEnv.pDeviceConfig->config.leds[led_number_].input_num + 1);
    ui->comboBox_Function->setCurrentIndex(gEnv.pDeviceConfig->config.leds[led_number_].type);
}

void LED::WriteToConfig()
{
    gEnv.pDeviceConfig->config.leds[led_number_].input_num = ui->spinBox_InputNumber->value() - 1;
    gEnv.pDeviceConfig->config.leds[led_number_].type = ui->comboBox_Function->currentIndex();
}
