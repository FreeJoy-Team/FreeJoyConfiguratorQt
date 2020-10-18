#include "advancedsettings.h"
#include "ui_advancedsettings.h"

//#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices> // wiki

#include "global.h"
#include "deviceconfig.h"

#include <QDebug>

AdvancedSettings::AdvancedSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);
    
    flasher = new Flasher(this);
    ui->layoutH_Flasher->addWidget(flasher);

    gEnv.pAppSettings->beginGroup("FontSettings");
    ui->spinBox_FontSize->setValue(gEnv.pAppSettings->value("FontSize", "8").toInt());
    gEnv.pAppSettings->endGroup();

    gEnv.pAppSettings->beginGroup("OtherSettings");
    ui->checkBox_LoadDefCfg->setChecked(gEnv.pAppSettings->value("LoadDefCfgOnStartUp", false).toBool());
    gEnv.pAppSettings->endGroup();

}

AdvancedSettings::~AdvancedSettings()
{
    gEnv.pAppSettings->beginGroup("OtherSettings");
    gEnv.pAppSettings->setValue("LoadDefCfgOnStartUp", ui->checkBox_LoadDefCfg->isChecked());
    gEnv.pAppSettings->endGroup();

    delete ui;
}

void AdvancedSettings::RetranslateUi()
{
    ui->retranslateUi(this);
    flasher->RetranslateUi();
}

Flasher* AdvancedSettings::GetFlasher()
{
    return flasher;
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

// default style
void AdvancedSettings::on_pushButton_StyleDefault_clicked()
{
    SetStyle(ui->pushButton_StyleDefault, ":/styles/default.qss", "default", false);
}
// white style
void AdvancedSettings::on_pushButton_StyleWhite_clicked()
{
    SetStyle(ui->pushButton_StyleWhite, ":qss/qss.qss", "white", false);
}
// dark style
void AdvancedSettings::on_pushButton_StyleDark_clicked()
{
    SetStyle(ui->pushButton_StyleDark, ":qdarkstyle/style.qss", "dark", true);
}

void AdvancedSettings::SetStyle(QPushButton* pressed_button, QString file_name, QString style_name,  bool is_dark)
{
    tmp_text = pressed_button->text();
    tmp_style = pressed_button->styleSheet();
    ui->pushButton_StyleDefault->setEnabled(false);
    ui->pushButton_StyleWhite->setEnabled(false);
    ui->pushButton_StyleDark->setEnabled(false);
    pressed_button->setText(tr("Loading... Please wait"));
    pressed_button->setStyleSheet(tmp_style + "color: rgb(230, 230, 230); background-color: rgb(170, 170, 0);");

    // без таймера не успевает отрисовать изменения текста и стиля кнопки, возможно на низкочастотных мониках 10мс не хватит?
    // отрисовка в другом потоке?
    QTimer::singleShot(10, [&, pressed_button, file_name, style_name]{
        QFile f(file_name);
        if (!f.exists())   {
            qDebug()<<"Unable to set stylesheet, file not found\n";
        }
        else   {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
        }

        gEnv.pAppSettings->beginGroup("StyleSettings");
        gEnv.pAppSettings->setValue("StyleSheet", style_name);
        gEnv.pAppSettings->endGroup();
        emit interfaceStyleChanged(is_dark);

        pressed_button->setText(tmp_text);
        pressed_button->setStyleSheet(tmp_style);
        ui->pushButton_StyleDefault->setEnabled(true);
        ui->pushButton_StyleWhite->setEnabled(true);
        ui->pushButton_StyleDark->setEnabled(true);
    });
}


void AdvancedSettings::on_spinBox_FontSize_valueChanged(int font_size)
{
    //gEnv.pAppSettings->beginGroup("FontSettings");
    QApplication::setFont(QFont("MS Shell Dlg 2", font_size));
    emit fontChanged();
    //gEnv.pAppSettings->endGroup();
}



//wiki
void AdvancedSettings::on_pushButton_Wiki_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/FreeJoy-Team/FreeJoyWiki"));
}

// about
void AdvancedSettings::on_pushButton_About_clicked()
{
    QString version("<p align=\"center\">FreeJoyConfiguratorQt v" + *gEnv.pAppVersion);    // тупо, надо в дефайне
    QString source("<br> Source code available on <A HREF=\"https://github.com/FreeJoy-Team/FreeJoyConfiguratorQt\">GutHub</A>");
    QString wiki("<br> Check <A HREF=\"https://github.com/FreeJoy-Team/FreeJoyWiki\">our wiki</A> for detailed instructions.");
    QMessageBox::about(this, "About FreeJoyQt", version + wiki + "<p align=\"center\">Build with Qt " + QString(QT_VERSION_STR) + source);
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
