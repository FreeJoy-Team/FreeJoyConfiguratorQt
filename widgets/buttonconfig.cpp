#include "buttonconfig.h"
#include "ui_buttonconfig.h"
#include <QTimer>

#include <QDebug>
ButtonConfig::ButtonConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ButtonConfig)
{
    ui->setupUi(this);
    is_shifts_activated_ = false;
    shift1_activated_ = false;
    shift2_activated_ = false;
    shift3_activated_ = false;
    shift4_activated_ = false;
    shift5_activated_ = false;

    // make dynamic spawn?
    for (int i = 0; i < MAX_BUTTONS_NUM; i++)
    {
        ButtonLogical* logical_buttons_widget = new ButtonLogical(i, this);
        ui->layoutV_LogicalButton->addWidget(logical_buttons_widget);
        LogicButtonPtrList_.append(logical_buttons_widget);

        connect(LogicButtonPtrList_[i], SIGNAL(functionIndexChanged(int, int, int)),
                this, SLOT(functionTypeChanged(int, int, int)));
    }

    LogicaButtonslCreator();
}

ButtonConfig::~ButtonConfig()
{
    delete ui;
}

void ButtonConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < LogicButtonPtrList_.size(); ++i) {
        LogicButtonPtrList_[i]->RetranslateUi();
    }
}

// dynamic initialization of widgets. its decrease app startup time
// в идеале надо и создавать виджеты здесь, но возникает проблема - долго открывается вкладка
// и пока не знаю как это решить
void ButtonConfig::LogicaButtonslCreator()
{
    static int tmp = 0;
    if (tmp >= MAX_BUTTONS_NUM){
        if (MAX_BUTTONS_NUM != 128){
            qCritical()<<"buttonconfig.cpp MAX_BUTTONS_NUM != 128";
        }
        qDebug()<<"LogicaButtonslCreator() finished";
        emit logicalButtonsCreated();
        return;
    }
    // как я понял таймер срабатывает после полной загрузки приложения(оно отобразится)
    // т.к. в LogicaButtonslCreator заходит при инициализации, но срабатывает после запуска приложения
    QTimer::singleShot(10, [&]{
        for (int i = 0; i < 8; i++) // MAX_BUTTONS_NUM(128)/8 = 16 ДОЛЖНО ДЕЛИТЬСЯ БЕЗ ОСТАТКА
        {
            LogicButtonPtrList_[tmp]->Initialization();
            tmp++;
        }
        LogicaButtonslCreator();
    });
}

void ButtonConfig::PhysicalButtonsSpawn(int count)
{
    // delete all
    while (!PhysicButtonPtrList_.empty()) {
        QWidget *widget = PhysicButtonPtrList_.takeLast();
        ui->layoutG_PhysicalButton->removeWidget(widget);
        widget->deleteLater();
    }
    // add
    int row = 0;
    int column = 0;
    ui->layoutG_PhysicalButton->setAlignment(Qt::AlignTop);
    for (int i = 0; i < count; i++) {
        if(column >= 8)     // phys buttons column
        {
            row++;
            column = 0;
        }
        ButtonPhysical* physical_button_widget = new ButtonPhysical(i, this);
        ui->layoutG_PhysicalButton->addWidget(physical_button_widget, row, column);
        PhysicButtonPtrList_.append(physical_button_widget);
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
    for (int i = 0; i < LogicButtonPtrList_.size(); ++i) {
        LogicButtonPtrList_[i]->SetMaxPhysButtons(value);
        LogicButtonPtrList_[i]->SetSpinBoxOnOff(value);
    }

    PhysicalButtonsSpawn(value);
}

void ButtonConfig::ButtonStateChanged()
{
    int number = 0;

    // logical buttons state
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            number = j + (i)*8;
            if ((gEnv.pDeviceConfig->gamepad_report.button_data[i] & (1 << (j & 0x07))))
            {
                if (number < LogicButtonPtrList_.size())
                {
                    LogicButtonPtrList_[number]->SetButtonState(true);
                }
            }
            else if ((gEnv.pDeviceConfig->gamepad_report.button_data[i] & (1 << (j & 0x07))) == false)
            {
                if (number < LogicButtonPtrList_.size())
                {
                    LogicButtonPtrList_[number]->SetButtonState(false);
                }
            }
        }
    }

    // physical button state
    for (int i = 1; i < 9; i++)
    {
        for (int j = 0; j < 8; j++)
        {
                number = gEnv.pDeviceConfig->gamepad_report.raw_button_data[0] + j + (i-1)*8;  //number = 64 + j + (i-1)*8;

            if ((gEnv.pDeviceConfig->gamepad_report.raw_button_data[i] & (1 << (j & 0x07))))
            {
                if (number < PhysicButtonPtrList_.size())
                {
                    PhysicButtonPtrList_[number]->SetButtonState(true);
                }
            }
            else if ((gEnv.pDeviceConfig->gamepad_report.raw_button_data[i] & (1 << (j & 0x07))) == false)
            {

                if ( number < PhysicButtonPtrList_.size())
                {
                    PhysicButtonPtrList_[number]->SetButtonState(false);
                }
            }
        }
    }

    // shift state
    for (int i = 0; i < SHIFT_COUNT; ++i)       // выглядит как избыточный код, но так необходимо для оптимизации
    {
        if (gEnv.pDeviceConfig->gamepad_report.shift_button_data & (1 << (i & 0x07)))
        {
            is_shifts_activated_ = true;

            if (i == 0 && shift1_activated_ == false){
                default_shift_style_ = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift1_logicalButton->setStyleSheet(default_shift_style_ + "background-color: rgb(0, 128, 0);");
                //ui->groupBox_Shift1->setStyleSheet("background-color: rgb(0, 128, 0);");
                //ui->spinBox_Shift1->setStyleSheet("background-color: rgb(0, 128, 0);");
                shift1_activated_ = true;
            }
            else if (i == 1 && shift2_activated_ == false){
                default_shift_style_ = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift2_logicalButton->setStyleSheet(default_shift_style_ + "background-color: rgb(0, 128, 0);");
                shift2_activated_ = true;
            }
            else if (i == 2 && shift3_activated_ == false){
                default_shift_style_ = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift3_logicalButton->setStyleSheet(default_shift_style_ + "background-color: rgb(0, 128, 0);");
                shift3_activated_ = true;
            }
            else if (i == 3 && shift4_activated_ == false){
                default_shift_style_ = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift4_logicalButton->setStyleSheet(default_shift_style_ + "background-color: rgb(0, 128, 0);");
                shift4_activated_ = true;
            }
            else if (i == 4 && shift5_activated_ == false){
                default_shift_style_ = ui->text_shift1_logicalButton->styleSheet();
                ui->text_shift5_logicalButton->setStyleSheet(default_shift_style_ + "background-color: rgb(0, 128, 0);");
                shift5_activated_ = true;
            }

        }
        else if (is_shifts_activated_ == true)
        {
            if (i == 0 && shift1_activated_ == true){
                ui->text_shift1_logicalButton->setStyleSheet(default_shift_style_);
                shift1_activated_ = false;
            } else if (i == 0 && shift2_activated_ == true){
                ui->text_shift2_logicalButton->setStyleSheet(default_shift_style_);
                shift2_activated_ = false;
            } else if (i == 0 && shift3_activated_ == true){
                ui->text_shift3_logicalButton->setStyleSheet(default_shift_style_);
                shift3_activated_ = false;
            } else if (i == 0 && shift4_activated_ == true){
                ui->text_shift4_logicalButton->setStyleSheet(default_shift_style_);
                shift4_activated_ = false;
            } else if (i == 0 && shift5_activated_ == true){
                ui->text_shift5_logicalButton->setStyleSheet(default_shift_style_);
                shift5_activated_ = false;
            }

            if (shift1_activated_ == false && shift2_activated_ == false && shift3_activated_ == false &&
                shift4_activated_ == false && shift5_activated_ == false)
            {
                is_shifts_activated_ = false;
            }
        }
    }
}

void ButtonConfig::ReadFromConfig()
{
    // logical buttons
    for (int i = 0; i < LogicButtonPtrList_.size(); i++) {
        LogicButtonPtrList_[i]->ReadFromConfig();
    }
    // other
    ui->spinBox_Shift1->setValue(gEnv.pDeviceConfig->config.shift_config[0].button + 1);
    ui->spinBox_Shift2->setValue(gEnv.pDeviceConfig->config.shift_config[1].button + 1);
    ui->spinBox_Shift3->setValue(gEnv.pDeviceConfig->config.shift_config[2].button + 1);
    ui->spinBox_Shift4->setValue(gEnv.pDeviceConfig->config.shift_config[3].button + 1);
    ui->spinBox_Shift5->setValue(gEnv.pDeviceConfig->config.shift_config[4].button + 1);

    ui->spinBox_Timer1->setValue(gEnv.pDeviceConfig->config.button_timer1_ms);
    ui->spinBox_Timer2->setValue(gEnv.pDeviceConfig->config.button_timer2_ms);
    ui->spinBox_Timer3->setValue(gEnv.pDeviceConfig->config.button_timer3_ms);

    ui->spinBox_DebounceTimer->setValue(gEnv.pDeviceConfig->config.button_debounce_ms);
    ui->spinBox_A2bDebounce->setValue(gEnv.pDeviceConfig->config.a2b_debounce_ms);

    ui->spinBox_EncoderPressTimer->setValue(gEnv.pDeviceConfig->config.encoder_press_time_ms);
}

void ButtonConfig::WriteToConfig()
{
    gEnv.pDeviceConfig->config.shift_config[0].button = ui->spinBox_Shift1->value() - 1;
    gEnv.pDeviceConfig->config.shift_config[1].button = ui->spinBox_Shift2->value() - 1;
    gEnv.pDeviceConfig->config.shift_config[2].button = ui->spinBox_Shift3->value() - 1;
    gEnv.pDeviceConfig->config.shift_config[3].button = ui->spinBox_Shift4->value() - 1;
    gEnv.pDeviceConfig->config.shift_config[4].button = ui->spinBox_Shift5->value() - 1;

    gEnv.pDeviceConfig->config.button_timer1_ms = ui->spinBox_Timer1->value();
    gEnv.pDeviceConfig->config.button_timer2_ms = ui->spinBox_Timer2->value();
    gEnv.pDeviceConfig->config.button_timer3_ms = ui->spinBox_Timer3->value();

    gEnv.pDeviceConfig->config.button_debounce_ms = ui->spinBox_DebounceTimer->value();
    gEnv.pDeviceConfig->config.a2b_debounce_ms = ui->spinBox_A2bDebounce->value();

    gEnv.pDeviceConfig->config.encoder_press_time_ms = ui->spinBox_EncoderPressTimer->value();

    // logical buttons
    for (int i = 0; i < LogicButtonPtrList_.size(); ++i) {
        LogicButtonPtrList_[i]->WriteToConfig();
    }
}
