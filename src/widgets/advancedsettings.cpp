#include "advancedsettings.h"
#include "ui_advancedsettings.h"

//#include <QFile>
#include <QComboBox>
#include <QDesktopServices> // wiki
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QTimer>

#include "deviceconfig.h"
#include "global.h"

#include <QDebug>

AdvancedSettings::AdvancedSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);

    m_flasher = new Flasher(this);
    ui->layoutH_Flasher->addWidget(m_flasher);

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

void AdvancedSettings::retranslateUi()
{
    ui->retranslateUi(this);
    m_flasher->retranslateUi();
}

Flasher *AdvancedSettings::flasher() const
{
    return m_flasher;
}

void AdvancedSettings::on_pushButton_LangEnglish_clicked()
{
    gEnv.pAppSettings->beginGroup("LanguageSettings");
    gEnv.pAppSettings->setValue("Language", "english");
    gEnv.pAppSettings->endGroup();

    emit languageChanged("english");
}

void AdvancedSettings::on_pushButton_LangRussian_clicked() // мб в деструкторе сохранять?
{
    gEnv.pAppSettings->beginGroup("LanguageSettings");
    gEnv.pAppSettings->setValue("Language", "russian");
    gEnv.pAppSettings->endGroup();

    emit languageChanged("russian");
}

// default style
void AdvancedSettings::on_pushButton_StyleDefault_clicked()
{
    setStyle(ui->pushButton_StyleDefault, ":/styles/default.qss", "default", false);
}
// white style
void AdvancedSettings::on_pushButton_StyleWhite_clicked()
{
    setStyle(ui->pushButton_StyleWhite, ":qss/qss.qss", "white", false);
}
// dark style
void AdvancedSettings::on_pushButton_StyleDark_clicked()
{
    setStyle(ui->pushButton_StyleDark, ":qdarkstyle/style.qss", "dark", true);
}
//#include <QStyle>
//#include <QThread>
//#include <QStyleFactory>
//#include <QApplication>
//#include "widgets/buttonlogical.h"
//#include "widgets/buttonconfig.h"
void AdvancedSettings::setStyle(QPushButton *pressedButton, QString filePath, QString styleName, bool isDark)
{
    m_default_text = pressedButton->text();
    m_default_style = pressedButton->styleSheet();
    ui->pushButton_StyleDefault->setEnabled(false);
    ui->pushButton_StyleWhite->setEnabled(false);
    ui->pushButton_StyleDark->setEnabled(false);
    pressedButton->setText(tr("Loading... Please wait"));
    pressedButton->setStyleSheet(m_default_style
                                 + QStringLiteral("color: rgb(230, 230, 230); background-color: rgb(170, 170, 0);"));

    // без таймера не успевает отрисовать изменения текста и стиля кнопки
    QTimer::singleShot(10, [&, pressedButton, filePath, styleName, isDark] {
        QFile f(filePath);
        if (!f.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "Unable to set stylesheet, file not found\n";
        } else {
            //            qtbase/src/widgets/kernel/qapplication.cpp        SOURCE
            //            void QApplication::setStyleSheet(const QString& styleSheet)
            //            {
            //                QApplicationPrivate::styleSheet = styleSheet;
            //                QStyleSheetStyle *styleSheetStyle = qt_styleSheet(QApplicationPrivate::app_style);
            //                if (styleSheet.isEmpty()) { // application style sheet removed
            //                    if (!styleSheetStyle)                                 // со старта приложения есть стиль
            //                        return; // there was no stylesheet before         // как мне его очистить не трогая сёрсы?
            //                    setStyle(styleSheetStyle->base);                      // тут проблема с первым переключением?
            //                } else if (styleSheetStyle) { // style sheet update, just repolish
            //                    styleSheetStyle->repolish(qApp);
            //                } else { // stylesheet set the first time
            //                    QStyleSheetStyle *newStyleSheetStyle = new QStyleSheetStyle(QApplicationPrivate::app_style);
            //                    QApplicationPrivate::app_style->setParent(newStyleSheetStyle);
            //                    setStyle(newStyleSheetStyle);
            //                }
            //            }

            //f.open(QFile::ReadOnly | QFile::Text);
            //QTextStream ts(&f);
            //QString str = ts.readAll();
            //QElapsedTimer timer;
            //timer.start();
            //qApp->setStyleSheet(QString::fromLatin1(f.readAll()));
            //qApp->setStyleSheet(QLatin1String(f.readAll()));
            //qDebug()<<timer.restart();
            //qApp->style()->unpolish(qApp);

            //window()->setStyleSheet(QLatin1String(f.readAll()));
            //            const QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
            //QStyleSheetStyle *styleSheetStyle =  qobject_cast<QStyleSheetStyle *>(QApplication::style());

            QElapsedTimer timer;
            timer.start();
            //window()->setStyleSheet(QLatin1String(f.readAll()));
            //            qDebug()<<timer.restart();

            for (auto &&child : window()->findChildren<QComboBox *>()) { // пиздец
                child->hide();
            }
            for (auto &&child : window()->findChildren<QCheckBox *>()) { // пиздец Х2
                child->hide();
            }
            for (auto &&child : window()->findChildren<QSpinBox *>()) { // ...
                child->hide();
            }

            window()->setStyleSheet(QLatin1String(f.readAll())); // повторный вызовQLatin1String(f.readAll()) == ""
            // очень медленно, не знаю как ускорить
            // хз чё делать
            qApp->setStyleSheet(QLatin1String(f.readAll())); // а тут QLatin1String(f.readAll()) = "" пустота
            // и почему вызов qApp->setStyleSheet после window()->setStyleSheet исправляет ситуацию с кривым стилем
            // и какого хера если стиль не дефолтный переключение на другой не дефолтный первый раз занимает дохера
            // а повторно мгновенно, где оно кешируется и какого хера при старте приложения оно грузится в 10 раз быстрее
            // чем применение стиля, если уже установлен стиль

            for (auto &&child : window()->findChildren<QComboBox *>()) { // какого хуя такое
                child->show();
            }
            for (auto &&child : window()->findChildren<QCheckBox *>()) { // дерьмо
                child->show();
            }
            for (auto &&child : window()->findChildren<QSpinBox *>()) { // работает быстрее всего
                child->show();
            }

            qDebug() << timer.elapsed();
            f.close();
        }

        gEnv.pAppSettings->beginGroup("StyleSettings");
        gEnv.pAppSettings->setValue("StyleSheet", styleName);
        gEnv.pAppSettings->endGroup();
        emit interfaceStyleChanged(isDark);

        pressedButton->setText(m_default_text);
        pressedButton->setStyleSheet(m_default_style); // косяк же tmp_style_ старый стиль
        ui->pushButton_StyleDefault->setEnabled(true);
        ui->pushButton_StyleWhite->setEnabled(true);
        ui->pushButton_StyleDark->setEnabled(true);
    });
}

void AdvancedSettings::on_spinBox_FontSize_valueChanged(int fontSize)
{
    //gEnv.pAppSettings->beginGroup("FontSettings");
    QApplication::setFont(QFont("MS Shell Dlg 2", fontSize));
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
    const QString version("<p align=\"center\">FreeJoyConfiguratorQt v" + *gEnv.pAppVersion + " "); // тупо, надо в дефайне?
    const QString source = tr("<br>Built with Qt %1 (%2)<br>"
                              R"(Source code available under GPLv3 on <a style="color: #03A9F4; text-decoration:none;" href="https://github.com/FreeJoy-Team/FreeJoyConfiguratorQt">Github</a><br>)")
                               .arg(QT_VERSION_STR, QSysInfo::buildCpuArchitecture()); //"Copyright © %3 Reksotiv and contributors"//, "2020"

    const QString wiki(tr(R"(<br>Check <a style="color: #03A9F4; text-decoration:none;" href="https://github.com/FreeJoy-Team/FreeJoyWiki">our wiki </a>)"
        "for detailed instructions."));
    QMessageBox::about(this, tr("About FreeJoyQt"), version + source + wiki);
}

void AdvancedSettings::readFromConfig()
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

void AdvancedSettings::writeToConfig()
{
    // PID
    gEnv.pDeviceConfig->config.pid = ui->lineEdit_PID->text().toInt(nullptr, 16);
    // dynamic conf.
    gEnv.pDeviceConfig->config.is_dynamic_config = ui->checkBox_DynamicHIDConfig->isChecked();
    // device name
    std::string tmp_string = ui->lineEdit_DeviceUSBName->text().toStdString();
    for (uint i = 0; i < sizeof(gEnv.pDeviceConfig->config.device_name); i++) { // ...
        if (i < tmp_string.size()) {
            gEnv.pDeviceConfig->config.device_name[i] = tmp_string[i];
        } else {
            gEnv.pDeviceConfig->config.device_name[i] = '\0';
        }
    }
    // usb exchange period
    gEnv.pDeviceConfig->config.exchange_period_ms = ui->spinBox_USBExchangePeriod->value();
}
