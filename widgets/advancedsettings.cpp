#include "advancedsettings.h"
#include "ui_advancedsettings.h"

#include "global.h"
#include "deviceconfig.h"
#include <QDebug>
#include <QString>



///// NEED INITIALIZATION
///// NEED FLASH MODE
AdvancedSettings::AdvancedSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);

}

AdvancedSettings::~AdvancedSettings()
{
    delete ui;
}

void AdvancedSettings::RetranslateUi()
{
    ui->retranslateUi(this);
}

void AdvancedSettings::ReadFromConfig()
{
    // PID
    ui->lineEdit_PID->setInputMask("HHHH");
    ui->lineEdit_PID->setText(QString::number(gEnv.pDeviceConfig->config.pid, 16));
    // dynamic conf.
    ui->checkBox_DynamicHIDConfig->setChecked(gEnv.pDeviceConfig->config.is_dynamic_config);
    // device name
    ui->lineEdit_DeviceUSBName->setText(gEnv.pDeviceConfig->config.device_name);
    // usb exchange period
    ui->spinBox_USBExchangePeriod->setValue(gEnv.pDeviceConfig->config.exchange_period_ms);
}

void AdvancedSettings::WriteToConfig()
{
    qDebug()<<"Go ACTIVATED";
    // PID
    gEnv.pDeviceConfig->config.pid = ui->lineEdit_PID->text().toInt(nullptr,16);
    // dynamic conf.
    gEnv.pDeviceConfig->config.is_dynamic_config = ui->checkBox_DynamicHIDConfig->isChecked();
    // device name
    std::string tmp_string = ui->lineEdit_DeviceUSBName->text().toStdString();
    for (uint i = 0; i < sizeof(gEnv.pDeviceConfig->config.device_name); i++) {             // ...
        gEnv.pDeviceConfig->config.device_name[i] = tmp_string[i];
    }
    // usb exchange period
    gEnv.pDeviceConfig->config.exchange_period_ms = ui->spinBox_USBExchangePeriod->value();
}
