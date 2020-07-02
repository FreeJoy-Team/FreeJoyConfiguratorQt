#include "axes.h"
#include "ui_axes.h"

Axes::Axes(int axis_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Axes)
{
    ui->setupUi(this);

    //pDev_config = &gEnv.pDeviceConfig->config;        // чуть короче но надо?

    axis_number_ = axis_number;
    ui->groupBox_AxixName->setTitle(axes_list_[axis_number_].gui_name);

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

    connect(ui->checkBox_Output, SIGNAL(toggled(bool)),
            this, SLOT(outputValueChanged(bool)));
    connect(ui->sliderV_Filter, SIGNAL(valueChanged(int)),
            this, SLOT(filterChanged(int)));
}

Axes::~Axes()
{
    delete ui;
}

void Axes::filterChanged(int filter_level)
{
    ui->label_Filter->setText(filter_list_[filter_level].gui_name);
}

void Axes::UpdateAxisRaw()
{
    ui->progressBar_Raw->setValue(gEnv.pDeviceConfig->gamepad_report.axis_data[axis_number_]);
}

void Axes::UpdateAxisOut()
{
    ui->progressBar_Out->setValue(gEnv.pDeviceConfig->gamepad_report.raw_axis_data[axis_number_]);
}

void Axes::ReadFromConfig()     // Converter::EnumToIndex(device_enum, list)                // add source_main
{
    // output, inverted
    ui->checkBox_Output->setChecked(gEnv.pDeviceConfig->config.axis_config[axis_number_].out_enabled);
    ui->checkBox_Inverted->setChecked(gEnv.pDeviceConfig->config.axis_config[axis_number_].inverted);
    // I2C, sources, function
    ui->comboBox_I2cAddress->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].i2c_address, i2c_address_list_));
    //ui->comboBox_AxisSource1->setCurrentIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].source_main);
    ui->comboBox_AxisSource2->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].source_secondary, axes_list_));
    ui->comboBox_Function->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].function, function_list_));
    // chanel
    ui->spinBox_ChanelEncoder->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].channel);
    // buttons
    ui->comboBox_Button1->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].button1_type, button_1_3_list_));
    ui->spinBox_Button1->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].button1);
    ui->comboBox_Button2->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].button2_type, button_2_list_));
    ui->spinBox_Button2->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].button2);
    ui->comboBox_Button3->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].button3_type, button_1_3_list_));
    ui->spinBox_Button3->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].button3);
    // divider, prescaler
    ui->spinBox_StepDiv->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].divider);
    ui->spinBox_Prescaler->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].prescaler);
    // resolution, offset
    ui->spinBox_Resolution->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].resolution);
    ui->spinBox_Offset->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].offset_angle);
    //deadband
    ui->checkBox_DynDeadband->setChecked(gEnv.pDeviceConfig->config.axis_config[axis_number_].is_dynamic_deadband);
    ui->spinBox_Deadband->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].deadband_size);
    // calibration
    ui->spinBox_CalibMin->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_min);
    ui->spinBox_CalibCenter->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_center);
    ui->spinBox_CalibMax->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_max);
    // filter
    ui->sliderV_Filter->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].filter);
}

void Axes::WriteToConfig()                // add source_main
{   // output, inverted
    gEnv.pDeviceConfig->config.axis_config[axis_number_].out_enabled = ui->checkBox_Output->isChecked();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].inverted = ui->checkBox_Inverted->isChecked();
    // I2C, sources, function
    gEnv.pDeviceConfig->config.axis_config[axis_number_].i2c_address = i2c_address_list_[ui->comboBox_I2cAddress->currentIndex()].device_enum_index;
    //gEnv.pDeviceConfig->config.axis_config[axis_number_].source_main =
    gEnv.pDeviceConfig->config.axis_config[axis_number_].source_secondary = axes_list_[ui->comboBox_AxisSource2->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].function = function_list_[ui->comboBox_Function->currentIndex()].device_enum_index;
    // chanel
    gEnv.pDeviceConfig->config.axis_config[axis_number_].channel = ui->spinBox_ChanelEncoder->value();
    // buttons
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button1_type = button_1_3_list_[ui->comboBox_Button1->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button1 = ui->spinBox_Button1->value();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button2_type = button_2_list_[ui->comboBox_Button2->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button2 = ui->spinBox_Button2->value();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button3_type = button_1_3_list_[ui->comboBox_Button3->currentIndex()].device_enum_index;
    gEnv.pDeviceConfig->config.axis_config[axis_number_].button3 = ui->spinBox_Button3->value();
    // divider, prescaler
    gEnv.pDeviceConfig->config.axis_config[axis_number_].divider = ui->spinBox_StepDiv->value();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].prescaler = ui->spinBox_Prescaler->value();
    // resolution, offset
    gEnv.pDeviceConfig->config.axis_config[axis_number_].resolution = ui->spinBox_Resolution->value();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].offset_angle = ui->spinBox_Offset->value();
    // deadband
    gEnv.pDeviceConfig->config.axis_config[axis_number_].is_dynamic_deadband = ui->checkBox_DynDeadband->isChecked();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].deadband_size = ui->spinBox_Deadband->value();
    // calibration
    gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_min = ui->spinBox_CalibMin->value();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_center = ui->spinBox_CalibCenter->value();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_max = ui->spinBox_CalibMax->value();
    // filter
    gEnv.pDeviceConfig->config.axis_config[axis_number_].filter = ui->sliderV_Filter->value();
}
#include <QDebug>
void Axes::outputValueChanged(bool is_checked)
{
    qDebug()<<is_checked <<"state";
    if (is_checked == true){
        ui->progressBar_Out->setEnabled(true);
        ui->progressBar_Raw->setEnabled(true);
        qDebug()<<is_checked <<"state";
    } else {
        ui->progressBar_Out->setEnabled(false);
        ui->progressBar_Raw->setEnabled(false);
    }
}
