#include "led.h"
#include "ui_led.h"

LED::LED(int ledNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LED)
{
    ui->setupUi(this);
    m_ledNumber = ledNumber;
    ui->label_LEDNumber->setNum(ledNumber + 1);

    for (int i = 0; i < LED_FUNCTION_COUNT; ++i) {
        ui->comboBox_Function->addItem(m_ledList[i].guiName);
    }
}

LED::~LED()
{
    delete ui;
}

void LED::retranslateUi()
{
    ui->retranslateUi(this);
}

int LED::currentButtonSelected() const
{
    return ui->spinBox_InputNumber->value() - 1;
}

void LED::setLedState(bool state)
{
    static QString default_style;

    if (state != m_currentState) {
        if (state) {
            default_style = ui->label_LEDNumber->styleSheet();
            ui->label_LEDNumber->setStyleSheet(default_style + QStringLiteral("background-color: rgb(0, 128, 0);"));
        } else {
            ui->label_LEDNumber->setStyleSheet(default_style);
        }
        m_currentState = state;
    }
}

void LED::readFromConfig()
{
    ui->spinBox_InputNumber->setValue(gEnv.pDeviceConfig->config.leds[m_ledNumber].input_num + 1);
    ui->comboBox_Function->setCurrentIndex(gEnv.pDeviceConfig->config.leds[m_ledNumber].type);
}

void LED::writeToConfig()
{
    gEnv.pDeviceConfig->config.leds[m_ledNumber].input_num = ui->spinBox_InputNumber->value() - 1;
    gEnv.pDeviceConfig->config.leds[m_ledNumber].type = ui->comboBox_Function->currentIndex();
}
