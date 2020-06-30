#include "buttonconfig.h"
#include "ui_buttonconfig.h"

//#include "widgets/buttonlogical.h"

#include <QDebug>

ButtonConfig::ButtonConfig(QWidget *parent) :               // сделать спавн физических при изменении пина
    QWidget(parent),
    ui(new Ui::ButtonConfig)
{
    ui->setupUi(this);


    // make dynamic spawn
    for (int i = 0; i < MAX_BUTTONS_NUM; i++)       // logical_buttons_widget объявлен в хедере? убрать?
    {
        logical_buttons_widget = new ButtonLogical(i, this);
        ui->layoutV_LogicalButton->addWidget(logical_buttons_widget);
        LogicButtonAdrList.append(logical_buttons_widget);
    }

    for (int i = 0; i < LogicButtonAdrList.size(); ++i) {     // можно и наверх закинуть, если без динамик спавна
        connect(LogicButtonAdrList[i], SIGNAL(functionIndexChanged(int, int, int)),
                this, SLOT(functionTypeChanged(int, int, int)));
    }
}

ButtonConfig::~ButtonConfig()
{
    delete ui;
}

void ButtonConfig::PhysicalButtonsSpawn(int count)
{
    // delete all
    while (PhysicButtonAdrList.size() > 0) {
        QWidget *widget = PhysicButtonAdrList.takeLast();
        ui->layoutG_PhysicalButton->removeWidget(widget);
        widget->deleteLater();
    }
    // add
    int row = 0;
    int column = 0;
    ui->layoutG_PhysicalButton->setAlignment(Qt::AlignTop);
    for (int i = 0; i < count; i++) {
        if(column >= 10)
        {
            row++;
            column = 0;
        }
        physical_button_widget = new ButtonPhysical(i, this);
        ui->layoutG_PhysicalButton->addWidget(physical_button_widget, row, column);
        PhysicButtonAdrList.append(physical_button_widget);
        column++;
    }
}

void ButtonConfig::functionTypeChanged(int index, int function_previous_index, int button_number)
{
    if (index == ENCODER_INPUT_A){
        emit encoderInputChanged(button_number + 1, 0);
    } else if (index == ENCODER_INPUT_B){
        emit encoderInputChanged(0, button_number + 1);
    }

    if (function_previous_index == ENCODER_INPUT_A){
        emit encoderInputChanged((button_number + 1) * -1, 0);  // send negative number
    } else if (function_previous_index == ENCODER_INPUT_B){
        emit encoderInputChanged(0, (button_number + 1) * -1);
    }
}

void ButtonConfig::setUiOnOff(int value)
{
    if (value > 0){
        ui->spinBox_Shift1->setEnabled(true);
        ui->spinBox_Shift2->setEnabled(true);
        ui->spinBox_Shift3->setEnabled(true);
        ui->spinBox_Shift4->setEnabled(true);
        ui->spinBox_Shift5->setEnabled(true);
    } else {
        ui->spinBox_Shift1->setEnabled(false);
        ui->spinBox_Shift2->setEnabled(false);
        ui->spinBox_Shift3->setEnabled(false);
        ui->spinBox_Shift4->setEnabled(false);
        ui->spinBox_Shift5->setEnabled(false);
    }
    for (int i = 0; i < LogicButtonAdrList.size(); ++i) {
        LogicButtonAdrList[i]->SetMaxPhysButtons(value);
        LogicButtonAdrList[i]->SetSpinBoxOnOff(value);
    }

    PhysicalButtonsSpawn(value);
}

void ButtonConfig::ButtonStateChanged()
{

//qDebug()<<gEnv.pDeviceConfig->gamepad_report.raw_button_data[0];
//////////  PHYSICAL STATE
    for (int i = 1; i < 9; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int number = j + (i-1)*8;
            if (gEnv.pDeviceConfig->gamepad_report.raw_button_data[0] == 0){        // сделать ДЛЯ 64

                if ((gEnv.pDeviceConfig->gamepad_report.raw_button_data[i] & (1 << (j & 0x07))))
                {
                    if (PhysicButtonAdrList.size() > number && PhysicButtonAdrList[number]->is_activated_ == false)
                    {
                        PhysicButtonAdrList[number]->ButtonState(true);
                        //qDebug()<< "j = " <<j <<"i = "<<i;
                    }
                    //qDebug()<< "j = " <<j <<"i = "<<i;
                }
                else if ((gEnv.pDeviceConfig->gamepad_report.raw_button_data[i] & (1 << (j & 0x07))) == false)
                {

                    if (PhysicButtonAdrList.size() > number && PhysicButtonAdrList[number]->is_activated_ == true)
                    {
                        PhysicButtonAdrList[number]->ButtonState(false);
                    }
                }
            }
        }
    }

//////////  LOGICAL STATE
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int number = j + (i)*8;

                if ((gEnv.pDeviceConfig->gamepad_report.button_data[i] & (1 << (j & 0x07))))
                {
                    if (LogicButtonAdrList.size() > number && LogicButtonAdrList[number]->is_activated_ == false)
                    {
                        LogicButtonAdrList[number]->ButtonState(true);
                    }
                }
                else if ((gEnv.pDeviceConfig->gamepad_report.button_data[i] & (1 << (j & 0x07))) == false)
                {

                    if (LogicButtonAdrList.size() > number && LogicButtonAdrList[number]->is_activated_ == true)
                    {
                        LogicButtonAdrList[number]->ButtonState(false);
                    }
                }
        }
    }
}

void ButtonConfig::ReadFromConfig()
{
    // logical buttons
    for (int i = 0; i < LogicButtonAdrList.size(); i++) {
        LogicButtonAdrList[i]->ReadFromConfig();
    }
    // other
    ui->spinBox_Shift1->setValue(gEnv.pDeviceConfig->config.shift_config[0].button);
    ui->spinBox_Shift2->setValue(gEnv.pDeviceConfig->config.shift_config[1].button);
    ui->spinBox_Shift3->setValue(gEnv.pDeviceConfig->config.shift_config[2].button);
    ui->spinBox_Shift4->setValue(gEnv.pDeviceConfig->config.shift_config[3].button);
    ui->spinBox_Shift5->setValue(gEnv.pDeviceConfig->config.shift_config[4].button);

    ui->spinBox_Timer1->setValue(gEnv.pDeviceConfig->config.button_timer1_ms);
    ui->spinBox_Timer2->setValue(gEnv.pDeviceConfig->config.button_timer2_ms);
    ui->spinBox_Timer3->setValue(gEnv.pDeviceConfig->config.button_timer3_ms);

    ui->spinBox_DebounceTimer->setValue(gEnv.pDeviceConfig->config.button_debounce_ms);

    ui->spinBox_EncoderPressTimer->setValue(gEnv.pDeviceConfig->config.exchange_period_ms);
}

void ButtonConfig::WriteToConfig()
{
    gEnv.pDeviceConfig->config.shift_config[0].button = ui->spinBox_Shift1->value();
    gEnv.pDeviceConfig->config.shift_config[1].button = ui->spinBox_Shift2->value();
    gEnv.pDeviceConfig->config.shift_config[2].button = ui->spinBox_Shift3->value();
    gEnv.pDeviceConfig->config.shift_config[3].button = ui->spinBox_Shift4->value();
    gEnv.pDeviceConfig->config.shift_config[4].button = ui->spinBox_Shift5->value();

    gEnv.pDeviceConfig->config.button_timer1_ms = ui->spinBox_Timer1->value();
    gEnv.pDeviceConfig->config.button_timer2_ms = ui->spinBox_Timer2->value();
    gEnv.pDeviceConfig->config.button_timer3_ms = ui->spinBox_Timer3->value();

    gEnv.pDeviceConfig->config.button_debounce_ms = ui->spinBox_DebounceTimer->value();

    gEnv.pDeviceConfig->config.exchange_period_ms = ui->spinBox_EncoderPressTimer->value();

    // logical buttons
    for (int i = 0; i < LogicButtonAdrList.size(); ++i) {
        LogicButtonAdrList[i]->WriteToConfig();
    }
}
