#include "advancedsettings.h"
#include "ui_advancedsettings.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QTimer>
#include <QFileDialog>

#include "global.h"
#include "deviceconfig.h"


AdvancedSettings::AdvancedSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);

    gEnv.pAppSettings->beginGroup("FontSettings");
    ui->spinBox_FontSize->setValue(gEnv.pAppSettings->value("FontSize", "8").toInt());
    gEnv.pAppSettings->endGroup();

//    connect(this, SIGNAL(styleChanged()),
//            this, SLOT(styleButtonWaiting()));
}

AdvancedSettings::~AdvancedSettings()
{
    delete ui;
}

void AdvancedSettings::RetranslateUi()
{
    ui->retranslateUi(this);
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

//void AdvancedSettings::styleButtonWaiting() // не успевает отрисоваться, таймер ставить или забить
//{
////    ui->pushButton_StyleDefault->setText("WAITING");
////    ui->pushButton_StyleWhite->setStyleSheet("background-color: yellow;");    //rgb(0, 128, 0)
////    ui->pushButton_StyleDark->setStyleSheet("background-color: yellow;");    //rgb(0, 128, 0)
//}

void AdvancedSettings::on_pushButton_StyleDefault_clicked()
{
    //emit styleChanged();
    tmp_text = ui->pushButton_StyleDefault->text();
    tmp_style = ui->pushButton_StyleDefault->styleSheet();
    ui->pushButton_StyleDefault->setEnabled(false);
    ui->pushButton_StyleWhite->setEnabled(false);
    ui->pushButton_StyleDark->setEnabled(false);
    ui->pushButton_StyleDefault->setText(tr("Loading... Please wait"));
    ui->pushButton_StyleDefault->setStyleSheet("background-color: rgb(170, 170, 0);");

    // без таймера не успевает отрисовать изменения текста и стиля, возможно на низкочастотных мониках 10мс не хватит?
    // отрисовка в другом потоке?
    QTimer::singleShot(10, [&]{
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

        ui->pushButton_StyleDefault->setText(tmp_text);
        ui->pushButton_StyleDefault->setStyleSheet(tmp_style);
        ui->pushButton_StyleDefault->setEnabled(true);
        ui->pushButton_StyleWhite->setEnabled(true);
        ui->pushButton_StyleDark->setEnabled(true);
    });
}

void AdvancedSettings::on_pushButton_StyleWhite_clicked()
{
    tmp_text = ui->pushButton_StyleWhite->text();
    tmp_style = ui->pushButton_StyleWhite->styleSheet();
    ui->pushButton_StyleDefault->setEnabled(false);
    ui->pushButton_StyleWhite->setEnabled(false);
    ui->pushButton_StyleDark->setEnabled(false);
    ui->pushButton_StyleWhite->setText(tr("Loading... Please wait"));
    ui->pushButton_StyleWhite->setStyleSheet("background-color: rgb(170, 170, 0);");

    QTimer::singleShot(10, [&]{
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

        ui->pushButton_StyleWhite->setText(tmp_text);
        ui->pushButton_StyleWhite->setStyleSheet(tmp_style);
        ui->pushButton_StyleDefault->setEnabled(true);
        ui->pushButton_StyleWhite->setEnabled(true);
        ui->pushButton_StyleDark->setEnabled(true);
    });
}

void AdvancedSettings::on_pushButton_StyleDark_clicked()
{
    tmp_text = ui->pushButton_StyleDark->text();
    tmp_style = ui->pushButton_StyleDark->styleSheet();
    ui->pushButton_StyleDefault->setEnabled(false);
    ui->pushButton_StyleWhite->setEnabled(false);
    ui->pushButton_StyleDark->setEnabled(false);
    ui->pushButton_StyleDark->setText(tr("Loading... Please wait"));
    ui->pushButton_StyleDark->setStyleSheet("background-color: rgb(170, 170, 0);");

    QTimer::singleShot(10, [&]{
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

        ui->pushButton_StyleDark->setText(tmp_text);
        ui->pushButton_StyleDark->setStyleSheet(tmp_style);
        ui->pushButton_StyleDefault->setEnabled(true);
        ui->pushButton_StyleWhite->setEnabled(true);
        ui->pushButton_StyleDark->setEnabled(true);
    });
}


void AdvancedSettings::on_spinBox_FontSize_valueChanged(int font_size)
{
    //gEnv.pAppSettings->beginGroup("FontSettings");
    QApplication::setFont(QFont("MS Shell Dlg 2", font_size));
    //gEnv.pAppSettings->endGroup();
}



void AdvancedSettings::DeviceConnected(bool is_connect)
{
    if (is_connect == true){        // disable enter to flash button if flasher connected
        ui->pushButton_FlasherMode->setEnabled(true);
    } else {
        ui->pushButton_FlasherMode->setEnabled(false);
    }
}

void AdvancedSettings::flasherFound(bool is_found)
{
    ui->pushButton_FlashFirmware->setEnabled(is_found);
    // disable enter to flash button if flasher connected
    if (is_found == true){
        ui->pushButton_FlasherMode->setEnabled(false);
        ui->pushButton_FlasherMode->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");
        ui->pushButton_FlasherMode->setText(tr("Ready to flash"));
    } else {
        ui->pushButton_FlasherMode->setEnabled(true);
        ui->pushButton_FlasherMode->setStyleSheet(default_style_);
        ui->pushButton_FlasherMode->setText(enter_to_flash_btn_text_);
    }
}

void AdvancedSettings::on_pushButton_FlasherMode_clicked()
{
    enter_to_flash_btn_text_ = ui->pushButton_FlasherMode->text();
    emit flashModeClicked(false);
}
#include <QDebug>
void AdvancedSettings::on_pushButton_FlashFirmware_clicked()
{
    QFile file(QFileDialog::getOpenFileName(this, tr("Open Config"), QDir::currentPath() + "/", tr("Binary files (.bin) (*.bin)")));

    if (file.open(QIODevice::ReadWrite)){
        ui->pushButton_FlashFirmware->setEnabled(false);
        flash_button_text_ = ui->pushButton_FlashFirmware->text();
        default_style_ = ui->pushButton_FlashFirmware->styleSheet();
        file_array_ = file.readAll();
        qDebug()<<"file_array_.size()"<<file_array_.size();
        //emit startFlash(&file_array_);
        emit startFlash(true);
    } else {
        qDebug()<<"cant open file";
    }
}

const QByteArray* AdvancedSettings::GetFileArray()
{
    return &file_array_;
}

void AdvancedSettings::flashStatus(int status, int percent)
{
    ui->progressBar_Flash->setValue(percent);

    if (percent == 1){
        ui->pushButton_FlasherMode->setText(tr("Firmware flashing.."));
    }

    if (status == FINISHED){
        ui->pushButton_FlashFirmware->setText(tr("Finished"));
        ui->pushButton_FlashFirmware->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");
        FlashDone();
    }
    else if (status == SIZE_ERROR){
        ui->pushButton_FlashFirmware->setText(tr("SIZE ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet("color: white; background-color: rgb(200, 0, 0);");
        FlashDone();
    }
    else if (status == CRC_ERROR){
        ui->pushButton_FlashFirmware->setText(tr("CRC ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet("color: white; background-color: rgb(200, 0, 0);");
        FlashDone();
    }
    else if (status == ERASE_ERROR){
        ui->pushButton_FlashFirmware->setText(tr("ERASE ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet("color: white; background-color: rgb(200, 128, 0);");
        FlashDone();
    }
}

void AdvancedSettings::FlashDone()
{
    QTimer::singleShot(1000, [&]{
        ui->pushButton_FlashFirmware->setStyleSheet(default_style_);
        ui->pushButton_FlashFirmware->setEnabled(false);
        ui->pushButton_FlashFirmware->setText(flash_button_text_);
        ui->pushButton_FlasherMode->setEnabled(true);
        ui->pushButton_FlasherMode->setStyleSheet(default_style_);
        ui->pushButton_FlasherMode->setText(enter_to_flash_btn_text_);
        file_array_.clear();
        file_array_.shrink_to_fit();
    });
}


void AdvancedSettings::ReadFromConfig()
{
    // PID
    //ui->lineEdit_PID->setInputMask("HHHH");
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
        if (i < tmp_string.size()){
            gEnv.pDeviceConfig->config.device_name[i] = tmp_string[i];
        } else {
            gEnv.pDeviceConfig->config.device_name[i] = '\0';
        }
    }
    // usb exchange period
    gEnv.pDeviceConfig->config.exchange_period_ms = ui->spinBox_USBExchangePeriod->value();
}
