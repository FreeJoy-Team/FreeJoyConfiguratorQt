#include "advancedsettings.h"
#include "ui_advancedsettings.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>

#include "global.h"
#include "deviceconfig.h"


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

void AdvancedSettings::on_pushButton_LangEnglish_clicked()
{
    gEnv.pAppSettings->beginGroup("LanguageSettings");
    gEnv.pAppSettings->setValue( "Language", "english" );
    gEnv.pAppSettings->endGroup();

    emit languageChanged("english");
}

void AdvancedSettings::on_pushButton_LangRussian_clicked()  // мб в деструкторе сохранять?
{
    gEnv.pAppSettings->beginGroup("LanguageSettings");
    gEnv.pAppSettings->setValue( "Language", "russian" );
    gEnv.pAppSettings->endGroup();

    emit languageChanged("russian");
}

void AdvancedSettings::on_pushButton_StyleDefault_clicked()
{
        QFile f(":/QSS-master/default.qss");
        if (!f.exists())   {
            printf("Unable to set stylesheet, file not found\n");
        }
        else   {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
        }

        gEnv.pAppSettings->beginGroup("StyleSettings");
        gEnv.pAppSettings->setValue("StyleSheet", "default");
        gEnv.pAppSettings->endGroup();
        emit interfaceStyleChanged(false);
}

void AdvancedSettings::on_pushButton_StyleWhite_clicked()
{
        QFile f(":/qss/css/qss.css");
        if (!f.exists())   {
            printf("Unable to set stylesheet, file not found\n");
        }
        else   {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
        }

        gEnv.pAppSettings->beginGroup("StyleSettings");
        gEnv.pAppSettings->setValue("StyleSheet", "white");
        gEnv.pAppSettings->endGroup();
        emit interfaceStyleChanged(false);
}

void AdvancedSettings::on_pushButton_StyleDark_clicked()
{
        QFile f(":qdarkstyle/style.qss");
        if (!f.exists())   {
            printf("Unable to set stylesheet, file not found\n");
        }
        else   {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
        }

        gEnv.pAppSettings->beginGroup("StyleSettings");
        gEnv.pAppSettings->setValue("StyleSheet", "dark");
        gEnv.pAppSettings->endGroup();
        emit interfaceStyleChanged(true);
}
