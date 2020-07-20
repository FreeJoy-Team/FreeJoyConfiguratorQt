#include "advancedsettings.h"
#include "ui_advancedsettings.h"

#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QTimer>

#include "global.h"
#include "deviceconfig.h"


///// NEED FLASH MODE
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
    ui->pushButton_StyleDefault->setText("Loading... Please wait");
    ui->pushButton_StyleDefault->setStyleSheet("background-color: rgb(170, 170, 0);");

    // без таймера не успевает отрисовать изменения текста и стиля, возможно на низкочастотных мониках 10мс не хватит?
    // или ? мб ещё блочить кнопку
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
    });
}

void AdvancedSettings::on_pushButton_StyleWhite_clicked()
{
    tmp_text = ui->pushButton_StyleWhite->text();
    tmp_style = ui->pushButton_StyleWhite->styleSheet();
    ui->pushButton_StyleWhite->setText("Loading... Please wait");
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
    });
}

void AdvancedSettings::on_pushButton_StyleDark_clicked()
{
    tmp_text = ui->pushButton_StyleDark->text();
    tmp_style = ui->pushButton_StyleDark->styleSheet();
    ui->pushButton_StyleDark->setText("Loading... Please wait");
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
    });
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
        if (i < tmp_string.size()){
            gEnv.pDeviceConfig->config.device_name[i] = tmp_string[i];
        } else {
            gEnv.pDeviceConfig->config.device_name[i] = '\0';
        }
    }
    // usb exchange period
    gEnv.pDeviceConfig->config.exchange_period_ms = ui->spinBox_USBExchangePeriod->value();
}

void AdvancedSettings::on_spinBox_FontSize_valueChanged(int font_size)
{
    //gEnv.pAppSettings->beginGroup("FontSettings");
    QApplication::setFont(QFont("MS Shell Dlg 2", font_size));
    //gEnv.pAppSettings->endGroup();
}
