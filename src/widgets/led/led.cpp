#include "led.h"
#include "ui_led.h"

LED::LED(int ledNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LED)
{
    ui->setupUi(this);
    m_ledNumber = ledNumber;
    ui->label_LEDNumber->setNum(ledNumber + 1);

    for (uint i = 0; i < std::size(m_ledList); ++i) {
        ui->comboBox_Function->addItem(m_ledList[i].guiName);
    }

    for (uint i = 0; i < std::size(m_TimerList); ++i) {
        ui->comboBox_Timer->addItem(m_TimerList[i].guiName);
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
    led_config_t *led = &gEnv.pDeviceConfig->config.leds[m_ledNumber];
    ui->spinBox_InputNumber->setValue(led->input_num + 1);
    ui->comboBox_Function->setCurrentIndex(led->type);

    ui->comboBox_Timer->setCurrentIndex(led->timer + 1); // +1 because first element in m_TimerList = -1
}

void LED::writeToConfig()
{
    led_config_t *led = &gEnv.pDeviceConfig->config.leds[m_ledNumber];
    led->input_num = ui->spinBox_InputNumber->value() - 1;
    led->type = ui->comboBox_Function->currentIndex();

    led->timer = ui->comboBox_Timer->currentIndex() - 1; // -1 because first element in m_TimerList = -1
}
