#include "shiftregisters.h"
#include "ui_shiftregisters.h"
#include <math.h>

ShiftRegisters::ShiftRegisters(int shift_reg_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShiftRegisters)
{
    ui->setupUi(this);

    shift_reg_number_ = shift_reg_number;
    ui->label_ShiftIndex->setNum(shift_reg_number + 1);

    for (int i = 0; i < SHIFT_REG_TYPES; ++i) {
        ui->comboBox_ShiftRegType->addItem(shift_registers_list_[i].gui_name);
        ui->label_DataPin->setText(not_defined_);
        ui->label_LatchPin->setText(not_defined_);
    }

    connect(ui->spinBox_ButtonCount, SIGNAL(valueChanged(int)),
            this, SLOT(calcRegistersCount(int)));
}

ShiftRegisters::~ShiftRegisters()
{
    delete ui;
}

void ShiftRegisters::calcRegistersCount(int button_count)
{
    ui->label_RegistersCount->setNum(ceil(button_count/8.0));
    emit buttonCountChanged(button_count, shift_reg_number_);
}

//int ShiftRegisters::GetLatchPin()
//{
//    return latch_pin_;
//}

//int ShiftRegisters::GetDataPin()
//{
//    return data_pin_;
//}

//QString ShiftRegisters::GetLatchString()
//{
//    return ui->label_LatchPin->text();
//}

//QString ShiftRegisters::GetDataString()
//{
//    return ui->label_DataPin->text();
//}

void ShiftRegisters::SetLatchPin(int latch_pin, QString pin_gui_name)
{
    if (latch_pin != 0){
        latch_pin_ = latch_pin;
        ui->label_LatchPin->setText(pin_gui_name);
    } else {
        latch_pin_ = 0;
        ui->label_LatchPin->setText(not_defined_);
    }
    SetUiOnOff();
}

void ShiftRegisters::SetDataPin(int data_pin, QString pin_gui_name)
{
    if (data_pin != 0){
        data_pin_ = data_pin;
        ui->label_DataPin->setText(pin_gui_name);
    } else {
        data_pin_ = 0;
        ui->label_DataPin->setText(not_defined_);
    }
    SetUiOnOff();
}

void ShiftRegisters::SetUiOnOff()
{
    if (latch_pin_ > 0 && data_pin_ > 0){
        for(auto&& child:this->findChildren<QWidget *>()){
        child->setEnabled(true);
        }
    } else {
        for(auto&& child:this->findChildren<QWidget *>()){
        child->setEnabled(false);
        }
    }
}

void ShiftRegisters::ReadFromConfig()
{
    ui->comboBox_ShiftRegType->setCurrentIndex(gEnv.pDeviceConfig->config.shift_registers[shift_reg_number_].type);
    ui->spinBox_ButtonCount->setValue(gEnv.pDeviceConfig->config.shift_registers[shift_reg_number_].button_cnt);
}

void ShiftRegisters::WriteToConfig()
{
    gEnv.pDeviceConfig->config.shift_registers[shift_reg_number_].type = ui->comboBox_ShiftRegType->currentIndex();
    gEnv.pDeviceConfig->config.shift_registers[shift_reg_number_].button_cnt = ui->spinBox_ButtonCount->value();
}
