#include "axesextended.h"
#include "ui_axesextended.h"

#include "converter.h"

AxesExtended::AxesExtended(int axisNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AxesExtended)
{
    ui->setupUi(this);

    m_axisNumber = axisNumber;

    for (int i = 0; i < m_i2cAddressList.size(); ++i) {
        ui->comboBox_I2cAddress->addItem(m_i2cAddressList[i].guiName);
    }
    for (int i = 0; i < m_functionList.size(); ++i) {
        ui->comboBox_Function->addItem(m_functionList[i].guiName);
    }
    for (int i = 0; i < m_button_1_3_list.size(); ++i) {
        ui->comboBox_Button1->addItem(m_button_1_3_list[i].guiName);
        ui->comboBox_Button3->addItem(m_button_1_3_list[i].guiName);
    }
    for (int i = 0; i < m_button_2_list.size(); ++i) {
        ui->comboBox_Button2->addItem(m_button_2_list[i].guiName);
    }
    for (int i = 0; i < m_axesList.size(); ++i) {
        ui->comboBox_AxisSource2->addItem(m_axesList[i].guiName);
    }

    ui->comboBox_Button1->setCurrentIndex(AXIS_BUTTON_DOWN);
    ui->comboBox_Button2->setCurrentIndex(AXIS_BUTTON_RESET);
    ui->comboBox_Button3->setCurrentIndex(AXIS_BUTTON_UP);
    // set ADS1115_00 selected
    ui->comboBox_I2cAddress->setCurrentIndex(1);

    // set filter text
    ui->label_Filter->setText(tr("Filter") + " " + m_filterList[ui->sliderH_Filter->value()].guiName);

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

void AxesExtended::retranslateUi()
{
    ui->retranslateUi(this);
}

void AxesExtended::setI2CEnabled(bool isEnable)
{
    ui->comboBox_I2cAddress->setEnabled(isEnable);
}

void AxesExtended::filterChanged(int filterLevel)
{
    ui->label_Filter->setText(tr("Filter") + " " + m_filterList[filterLevel].guiName);
}

void AxesExtended::functionIndexChanged(int index)
{
    if (index > 0) {
        ui->comboBox_AxisSource2->setEnabled(true);
    } else {
        ui->comboBox_AxisSource2->setEnabled(false);
    }
}

void AxesExtended::readFromConfig()
{
    axis_config_t *axisCfg = &gEnv.pDeviceConfig->config.axis_config[m_axisNumber];
    // I2C, sources, function
    ui->comboBox_I2cAddress->setCurrentIndex(Converter::EnumToIndex(axisCfg->i2c_address, m_i2cAddressList));
    ui->comboBox_AxisSource2->setCurrentIndex(Converter::EnumToIndex(axisCfg->source_secondary, m_axesList));
    ui->comboBox_Function->setCurrentIndex(Converter::EnumToIndex(axisCfg->function, m_functionList));
    // chanel
    ui->spinBox_ChanelEncoder->setValue(axisCfg->channel);
    // buttons
    ui->comboBox_Button1->setCurrentIndex(Converter::EnumToIndex(axisCfg->button1_type, m_button_1_3_list));
    ui->spinBox_Button1->setValue(axisCfg->button1 + 1);
    ui->comboBox_Button2->setCurrentIndex(Converter::EnumToIndex(axisCfg->button2_type, m_button_2_list));
    ui->spinBox_Button2->setValue(axisCfg->button2 + 1);
    ui->comboBox_Button3->setCurrentIndex(Converter::EnumToIndex(axisCfg->button3_type, m_button_1_3_list));
    ui->spinBox_Button3->setValue(axisCfg->button3 + 1);
    // divider, prescaler
    ui->spinBox_StepDiv->setValue(axisCfg->divider);
    ui->spinBox_Prescaler->setValue(axisCfg->prescaler);
    // resolution, offset
    ui->spinBox_Resolution->setValue(axisCfg->resolution + 1);
    ui->spinBox_Offset->setValue(axisCfg->offset_angle * 15);
    //deadband
    ui->checkBox_DynDeadband->setChecked(axisCfg->is_dynamic_deadband);
    ui->spinBox_Deadband->setValue(axisCfg->deadband_size);
    // filter
    ui->sliderH_Filter->setValue(axisCfg->filter);
}

void AxesExtended::writeToConfig()
{
    axis_config_t *axisCfg = &gEnv.pDeviceConfig->config.axis_config[m_axisNumber];
    // I2C, sources, function
    axisCfg->i2c_address = m_i2cAddressList[ui->comboBox_I2cAddress->currentIndex()].deviceEnumIndex;
    axisCfg->source_secondary = m_axesList[ui->comboBox_AxisSource2->currentIndex()].deviceEnumIndex;
    axisCfg->function = m_functionList[ui->comboBox_Function->currentIndex()].deviceEnumIndex;
    // chanel
    axisCfg->channel = ui->spinBox_ChanelEncoder->value();
    // buttons
    axisCfg->button1_type = m_button_1_3_list[ui->comboBox_Button1->currentIndex()].deviceEnumIndex;
    axisCfg->button1 = ui->spinBox_Button1->value() - 1;
    axisCfg->button2_type = m_button_2_list[ui->comboBox_Button2->currentIndex()].deviceEnumIndex;
    axisCfg->button2 = ui->spinBox_Button2->value() - 1;
    axisCfg->button3_type = m_button_1_3_list[ui->comboBox_Button3->currentIndex()].deviceEnumIndex;
    axisCfg->button3 = ui->spinBox_Button3->value() - 1;
    // divider, prescaler
    axisCfg->divider = ui->spinBox_StepDiv->value();
    axisCfg->prescaler = ui->spinBox_Prescaler->value();
    // resolution, offset
    axisCfg->resolution = ui->spinBox_Resolution->value() - 1;
    axisCfg->offset_angle = ui->spinBox_Offset->value() / 15;
    // deadband
    axisCfg->is_dynamic_deadband = ui->checkBox_DynDeadband->isChecked();
    axisCfg->deadband_size = ui->spinBox_Deadband->value();
    // filter
    axisCfg->filter = ui->sliderH_Filter->value();
}
