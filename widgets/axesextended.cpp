#include "axesextended.h"
#include "ui_axesextended.h"

#include "converter.h"

AxesExtended::AxesExtended(int axis_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesExtended)
{
    ui->setupUi(this);

    axis_number_ = axis_number;

    for (uint i = 0; i < i2c_address_list_.size(); ++i) {
        ui->comboBox_I2cAddress->addItem(i2c_address_list_[i].gui_name);
    }
    for (uint i = 0; i < function_list_.size(); ++i) {
        ui->comboBox_Function->addItem(function_list_[i].gui_name);
    }
    for (uint i = 0; i < button_1_3_list_.size(); ++i) {
        ui->comboBox_Button1->addItem(button_1_3_list_[i].gui_name);
        ui->comboBox_Button3->addItem(button_1_3_list_[i].gui_name);
    }
    for (uint i = 0; i < button_2_list_.size(); ++i) {
        ui->comboBox_Button2->addItem(button_2_list_[i].gui_name);
    }
    for (uint i = 0; i < axes_list_.size(); ++i) {
        ui->comboBox_AxisSource2->addItem(axes_list_[i].gui_name);
    }

    ui->comboBox_Button1->setCurrentIndex(AXIS_BUTTON_DOWN);
    ui->comboBox_Button2->setCurrentIndex(AXIS_BUTTON_RESET);
    ui->comboBox_Button3->setCurrentIndex(AXIS_BUTTON_UP);
    // set ADS1115_00 selected
    ui->comboBox_I2cAddress->setCurrentIndex(1);

    // set filter text
    ui->label_Filter->setText(tr("Filter") + " " + filter_list_[ui->sliderH_Filter->value()].gui_name);

    // filter changed
    connect(ui->sliderH_Filter, SIGNAL(valueChanged(int)),
            this, SLOT(filterChanged(int)));
    // function changed
    connect(ui->comboBox_Function, SIGNAL(currentIndexChanged(int)),
            this, SLOT(functionIndexChanged(int)));
}

AxesExtended::~AxesExtended()
{
    delete ui;
}


void AxesExtended::SetI2CEnabled(bool is_enable)
{
    ui->comboBox_I2cAddress->setEnabled(is_enable);
}

void AxesExtended::filterChanged(int filter_level)
{
    ui->label_Filter->setText(tr("Filter") + " " + filter_list_[filter_level].gui_name);
}

void AxesExtended::functionIndexChanged(int index)
{
    if (index > 0){
        ui->comboBox_AxisSource2->setEnabled(true);
    } else {
        ui->comboBox_AxisSource2->setEnabled(false);
    }
}

void AxesExtended::ReadFromConfig()
{
    // I2C, sources, function
    ui->comboBox_I2cAddress->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].i2c_address, i2c_address_list_));
    ui->comboBox_AxisSource2->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].source_secondary, axes_list_));
    ui->comboBox_Function->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].function, function_list_));
    // chanel
    ui->spinBox_ChanelEncoder->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].channel);
    // buttons
    ui->comboBox_Button1->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].button1_type, button_1_3_list_));
    ui->spinBox_Button1->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].button1 + 1);
    ui->comboBox_Button2->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].button2_type, button_2_list_));
    ui->spinBox_Button2->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].button2 + 1);
    ui->comboBox_Button3->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].button3_type, button_1_3_list_));
    ui->spinBox_Button3->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].button3 + 1);
    // divider, prescaler
    ui->spinBox_StepDiv->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].divider);
    ui->spinBox_Prescaler->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].prescaler);
    // resolution, offset
    ui->spinBox_Resolution->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].resolution + 1);
    ui->spinBox_Offset->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].offset_angle);
    //deadband
    ui->checkBox_DynDeadband->setChecked(gEnv.pDeviceConfig->config.axis_config[axis_number_].is_dynamic_deadband);
    ui->spinBox_Deadband->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].deadband_size);
    // filter
    ui->sliderH_Filter->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].filter);
}

void AxesExtended::WriteToConfig()
{
    // I2C, sources, function
    gEnv.pDeviceConfig->config.axis_config[axis_number_].i2c_address = i2c_address_list_[ui->comboBox_I2cAddress->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].source_secondary = axes_list_[ui->comboBox_AxisSource2->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].function = function_list_[ui->comboBox_Function->currentIndex()].device_enum_index;
    // chanel
    gEnv.pDeviceConfig->config.axis_config[axis_number_].channel = ui->spinBox_ChanelEncoder->value();
    // buttons
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button1_type = button_1_3_list_[ui->comboBox_Button1->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button1 = ui->spinBox_Button1->value() - 1;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button2_type = button_2_list_[ui->comboBox_Button2->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button2 = ui->spinBox_Button2->value() - 1;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button3_type = button_1_3_list_[ui->comboBox_Button3->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button3 = ui->spinBox_Button3->value() - 1;
    // divider, prescaler
    gEnv.pDeviceConfig->config.axis_config[axis_number_].divider = ui->spinBox_StepDiv->value();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].prescaler = ui->spinBox_Prescaler->value();
    // resolution, offset
    gEnv.pDeviceConfig->config.axis_config[axis_number_].resolution = ui->spinBox_Resolution->value() - 1;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].offset_angle = ui->spinBox_Offset->value();
    // deadband
    gEnv.pDeviceConfig->config.axis_config[axis_number_].is_dynamic_deadband = ui->checkBox_DynDeadband->isChecked();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].deadband_size = ui->spinBox_Deadband->value();
    // filter
    gEnv.pDeviceConfig->config.axis_config[axis_number_].filter = ui->sliderH_Filter->value();
}
