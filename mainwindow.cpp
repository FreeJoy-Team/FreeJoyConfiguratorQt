#include <QThread>
#include <QTimer>
#include <QSettings>
#include <QFileDialog>
#include <ctime>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mousewheelguard.h"
#include <QSpinBox>

#include "common_types.h"
#include "global.h"
#include "deviceconfig.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QElapsedTimer timer;
    timer.start();

    qDebug()<<"after main start MainWindow constructor time ms ="<< gEnv.pApp_start_time->elapsed();
    ui->setupUi(this);

    QMainWindow::setWindowIcon(QIcon(":/Images/icon-32.png"));
    //setWindowFlags( Qt::FramelessWindowHint );

    // версия контроллера
    QString str = QString::number(FIRMWARE_VERSION, 16);
    if (str.size() == 4){
        this->setWindowTitle(tr("FreeJoy Configurator") + " v" + str[0] + "." + str[1] + "." + str[2]);// + "b" + str[3]);
    }

    // load application config
    LoadAppConfig();

    hid_device_worker = new HidDevice();
    thread = new QThread;

    thread_getSend_config = new QThread;


    qDebug()<<"befor load widgets time ms ="<< timer.restart();
                                            //////////////// ADD WIDGETS ////////////////
    // add pin config
    pin_config = new PinConfig(this);
    ui->layoutV_tabPinConfig->addWidget(pin_config);
    qDebug()<<"pin config load time ms ="<< timer.restart();
    // add button config
    button_config = new ButtonConfig(this);     // not need delete. this - parent
    ui->layoutV_tabButtonConfig->addWidget(button_config);
    qDebug()<<"button config load time ms ="<< timer.restart();
    // add axes config
    axes_config = new AxesConfig(this);
    ui->layoutV_tabAxesConfig->addWidget(axes_config);
    qDebug()<<"axes config load time ms ="<< timer.restart();
    // add axes curves config
    axes_curves_config = new AxesCurvesConfig(this);
    ui->layoutV_tabAxesCurvesConfig->addWidget(axes_curves_config);
    qDebug()<<"curves config load time ms ="<< timer.restart();
    // add shift registers config
    shift_reg_config = new ShiftRegistersConfig(this);
    ui->layoutV_tabShiftRegistersConfig->addWidget(shift_reg_config);
    qDebug()<<"shift config load time ms ="<< timer.restart();
    // add encoders config
    encoder_config = new EncodersConfig(this);
    ui->layoutV_tabEncodersConfig->addWidget(encoder_config);
    qDebug()<<"encoder config load time ms ="<< timer.restart();
    // add led config
    led_config = new LedConfig(this);
    ui->layoutV_tabLedConfig->addWidget(led_config);
    qDebug()<<"led config load time ms ="<< timer.restart();


    // strong focus for mouse wheel
    for (auto&& child: this->findChildren<QSpinBox *>())
    {
        child->setFocusPolicy(Qt::StrongFocus);
        child->installEventFilter(new MouseWheelGuard(child));
    }

    for (auto&& child: this->findChildren<QComboBox *>())
    {
        child->setFocusPolicy(Qt::StrongFocus);
        child->installEventFilter(new MouseWheelGuard(child));
    }
    // это наверно неправильно, но я пока не сообразил как сверху исключить, мб через object_cast?
    for (auto&& child: pin_config->findChildren<PinComboBox *>())
    {
        for (auto&& comBox: child->findChildren<QComboBox *>())
        {
            comBox->setFocusPolicy(Qt::WheelFocus);
        }
    }
    for (auto&& comBox: axes_curves_config->findChildren<QComboBox *>())
    {
        comBox->setFocusPolicy(Qt::WheelFocus);
    }

                                            //////////////// SIGNASL-SLOTS ////////////////
            ///////// GET / SEND     CONFIG ///////
    connect(this, SIGNAL(getConfigDone(bool)),
            this, SLOT(configReceived(bool)));
    connect(this, SIGNAL(sendConfigDone(bool)),
            this, SLOT(configSent(bool)));



            /////////      /////////////
    // buttons pin changed
    connect(pin_config, SIGNAL(totalButtonsValueChanged(int)),
                button_config, SLOT(setUiOnOff(int)));
    // LEDs changed
    connect(pin_config, SIGNAL(totalLEDsValueChanged(int)),
                led_config, SLOT(SpawnLEDs(int)));
    // encoder changed
    connect(button_config, SIGNAL(encoderInputChanged(int, int)),
            encoder_config, SLOT(encoderInputChanged(int, int)));
    // fast encoder
    connect(pin_config, SIGNAL(fastEncoderSelected(QString, bool)),
            encoder_config, SLOT(fastEncoderSelected(QString, bool)));
    // shift registers
    connect(pin_config, SIGNAL(shiftRegSelected(int, int, QString)),
            shift_reg_config, SLOT(shiftRegSelected(int, int, QString)));
    // a2b count
    connect(axes_config, SIGNAL(a2bCountChanged(int)),
            pin_config, SLOT(a2bCountChanged(int)));
    // shift reg buttons count shiftRegsButtonsCount
    connect(shift_reg_config, SIGNAL(shiftRegButtonsCountChanged(int)),
            pin_config, SLOT(shiftRegButtonsCountChanged(int)));
    // axes source changed//axesSourceChanged
    connect(pin_config, SIGNAL(axesSourceChanged(int, bool)),
            axes_config, SLOT(addOrDeleteMainSource(int, bool)));
    // language changed
    connect(ui->widget_2, SIGNAL(languageChanged(QString)),
            this, SLOT(languageChanged(QString)));
    // interface changed
    connect(ui->widget_2, SIGNAL(interfaceStyleChanged(bool)),
            this, SLOT(interfaceStyleChanged(bool)));
    // font changed
    connect(ui->widget_2, SIGNAL(fontChanged()),
            this, SLOT(setFont()));


    // enter flash mode clicked
    connect(ui->widget_2, SIGNAL(flashModeClicked(bool)),
            this, SLOT(deviceFlasherController(bool)));
    // flasher found
    connect(hid_device_worker, SIGNAL(flasherFound(bool)),
            ui->widget_2, SLOT(flasherFound(bool)));
    // start flash
    connect(ui->widget_2, SIGNAL(startFlash(bool)),              // thread  flashStatus
            this, SLOT(deviceFlasherController(bool)));
    // flash status
    connect(hid_device_worker, SIGNAL(flashStatus(int, int)),              // thread  flashStatus
            ui->widget_2, SLOT(flashStatus(int, int)));
    // set selected hid device
    connect(ui->comboBox_HidDeviceList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(hidDeviceListChanged(int)));



    hid_device_worker->moveToThread(thread);

    connect(thread, SIGNAL(started()), hid_device_worker, SLOT(processData()));

    connect(hid_device_worker, SIGNAL(putGamepadPacket(uint8_t *)),
                          this, SLOT(getGamepadPacket(uint8_t *)));
    connect(hid_device_worker, SIGNAL(putConnectedDeviceInfo()),
                          this, SLOT(showConnectDeviceInfo()));
    connect(hid_device_worker, SIGNAL(putDisconnectedDeviceInfo()),
                          this, SLOT(hideConnectDeviceInfo()));
    connect(hid_device_worker, SIGNAL(hidDeviceList(QStringList*)),
                          this, SLOT(hidDeviceList(QStringList*)));


    // read config from device
    connect(hid_device_worker, SIGNAL(configReceived(bool)),
            this, SLOT(configReceived(bool)));
    // write config to device
    connect(hid_device_worker, SIGNAL(configSent(bool)),
            this, SLOT(configSent(bool)));


    // load default config
    ReadFromConfig();
    gEnv.pAppSettings->beginGroup("OtherSettings");
    if (gEnv.pAppSettings->value("LoadDefCfgOnStartUp", false).toBool() == true)
    {
        gEnv.pAppSettings->endGroup();
        on_pushButton_LoadDefaultConfig_clicked();
    } else {
        gEnv.pAppSettings->endGroup();
    }


    // set style
    gEnv.pAppSettings->beginGroup("StyleSettings");
    if (gEnv.pAppSettings->value("StyleSheet", "default").toString() == "dark"){
        interfaceStyleChanged(true);
    } else {
        interfaceStyleChanged(false);
    }
    gEnv.pAppSettings->endGroup();


    thread->start();

    qDebug()<<"after widgets MainWindow constructor end time ms ="<< timer.elapsed();
    qDebug()<<"from start app to MainWindow constructor end time ms ="<< gEnv.pApp_start_time->elapsed();
}

MainWindow::~MainWindow()
{
    SaveAppConfig();
    hid_device_worker->SetIsFinish(true);
    thread->quit();
    thread->deleteLater();
    thread->wait();
    delete hid_device_worker;
    delete thread;              // не уверен в нужности, если есть thread->deleteLater();
    //delete thread_getSend_config;       // hz
    delete ui;
}




                                                        ///////////////////// device reports /////////////////////
// device connected
void MainWindow::showConnectDeviceInfo() {
    ui->label_DeviceStatus->setText(tr("Connected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");
    ui->pushButton_ReadConfig->setEnabled(true);
    ui->pushButton_WriteConfig->setEnabled(true);
    ui->widget_2->DeviceConnected(true);
}
// device disconnected
void MainWindow::hideConnectDeviceInfo() {
    ui->label_DeviceStatus->setText(tr("Disconnected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(160, 0, 0);");
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);
    ui->widget_2->DeviceConnected(false);
    axes_curves_config->DeviceStatus(false);
}
// add/delete hid devices to combobox
void MainWindow::hidDeviceList(QStringList* device_list)
{
    if (device_list->size() == 0 && ui->comboBox_HidDeviceList->count() > 0)
    {
        ui->comboBox_HidDeviceList->clear();
        return;
    } else if (device_list->size() > 0)
    {
        ui->comboBox_HidDeviceList->clear();
        ui->comboBox_HidDeviceList->addItems(*device_list);
    }
}
// received device report
void MainWindow::getGamepadPacket(uint8_t * buff)
{
    report_convert.GamepadReport(buff);

    // update button state without delay. fix gamepad_report.raw_button_data[0]
    // из-за задержки может не ловить изменения первых физических 64 кнопок или оставшихся. Например, может подряд попасться gamepad_report.raw_button_data[0] = 0
    // и не видеть оставшиеся физические 64 кнопки. ленивый и неоптимизированный фикс
    if(ui->tab_ButtonConfig->isVisible() == true){
        button_config->ButtonStateChanged();
    }

    static QElapsedTimer timer;
    static bool change = false;

    if (!change)
    {
        timer.start();
        change = true;
    }
    else if (timer.elapsed() > 17)    // обновление раз в 17мс, мб сделать дефайн в герцах
    {
        // optimization
//        if(ui->tab_ButtonConfig->isVisible() == true){
//            button_config->ButtonStateChanged();
//        }
        // optimization
        if(ui->tab_AxesConfig->isVisible() == true){
            axes_config->AxesValueChanged();
        }
        // optimization
        if(ui->tab_AxesCurves->isVisible() == true){
            axes_curves_config->UpdateAxesCurves();
        }
        change = false;
    }
    // debug tab
#ifdef QT_DEBUG
    static int asd = 0;
    ui->lineEdit->setText(QString::number(++asd));
#endif
}

// Flasher controller
void MainWindow::deviceFlasherController(bool is_start_flash)
{        // херня? mb QtConcurrent::run()
    // убрать всё в один поток, как read/write
    QEventLoop loop;
    QObject context;
    context.moveToThread(thread_getSend_config);
    connect(thread_getSend_config, &QThread::started, &context, [&]() {
        if (is_start_flash == true){
            hid_device_worker->FlashFirmware(ui->widget_2->GetFileArray());
        } else {
            hid_device_worker->EnterToFlashMode();
        }

        loop.quit();
    });
    thread_getSend_config->start();
    loop.exec();
    thread_getSend_config->quit();
    thread_getSend_config->wait();
}




                                            /////////////////////                 /////////////////////

// slot interface style changed
void MainWindow::interfaceStyleChanged(bool is_dark)
{
    axes_curves_config->SetDarkInterface(is_dark);
}

// slot language change
void MainWindow::languageChanged(QString language)        // QSignalBlocker blocker(ui->comboBox);
{
    if (language == "russian")
    {
        translator.load(":/FreeJoyQt_ru");// + QString("ru_RU"));//QLocale::system().name();//QString("ru_RU"));//QLocale::name());
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
    }
    else if (language == "english")
    {
        translator.load(":/FreeJoyQt_en");
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
    } else {
        return;
    }
    pin_config->RetranslateUi();
    button_config->RetranslateUi();
    led_config->RetranslateUi();
    encoder_config->RetranslateUi();
    shift_reg_config->RetranslateUi();
    axes_config->RetranslateUi();
    axes_curves_config->RetranslateUi();
    ui->widget_2->RetranslateUi();
}
// set font
void MainWindow::setFont()
{
    for (QWidget *widget : QApplication::allWidgets()) {
        widget->setFont(QApplication::font());
        widget->update();
    }
}

// load app config
void MainWindow::LoadAppConfig()
{
    // load language settings
    gEnv.pAppSettings->beginGroup("LanguageSettings");
    if (gEnv.pAppSettings->value("Language", "english").toString() == "russian")
    {
        translator.load(":/FreeJoyQt_ru");
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
        ui->widget_2->RetranslateUi();
    }
    gEnv.pAppSettings->endGroup();

    // load window settings
    gEnv.pAppSettings->beginGroup("WindowSettings");
    this->restoreGeometry(gEnv.pAppSettings->value("Geometry").toByteArray());
    gEnv.pAppSettings->endGroup();

    // load tab index
    gEnv.pAppSettings->beginGroup("TabIndexSettings");
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_PinConfig),
                                      gEnv.pAppSettings->value("PinConfig", ui->tabWidget->indexOf(ui->tab_PinConfig)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_ButtonConfig),
                                      gEnv.pAppSettings->value("ButtonConfig", ui->tabWidget->indexOf(ui->tab_ButtonConfig)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_AxesConfig),
                                      gEnv.pAppSettings->value("AxesConfig", ui->tabWidget->indexOf(ui->tab_AxesConfig)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_AxesCurves),
                                      gEnv.pAppSettings->value("AxesCurves", ui->tabWidget->indexOf(ui->tab_AxesCurves)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_ShiftRegisters),
                                      gEnv.pAppSettings->value("ShiftRegs", ui->tabWidget->indexOf(ui->tab_ShiftRegisters)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_Encoders),
                                      gEnv.pAppSettings->value("Encoders", ui->tabWidget->indexOf(ui->tab_Encoders)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_LED),
                                      gEnv.pAppSettings->value("LED", ui->tabWidget->indexOf(ui->tab_LED)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_AdvancedSettings),
                                      gEnv.pAppSettings->value("AdvSettings", ui->tabWidget->indexOf(ui->tab_AdvancedSettings)).toInt());
    ui->tabWidget->setCurrentIndex(gEnv.pAppSettings->value("CurrentIndex", 0).toInt());
    gEnv.pAppSettings->endGroup();

    #ifdef QT_DEBUG
    #else
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tab_Debug));
    #endif
}
// save app config
void MainWindow::SaveAppConfig()
{
    // save tab index
    gEnv.pAppSettings->beginGroup("TabIndexSettings");
    gEnv.pAppSettings->setValue("PinConfig",       ui->tabWidget->indexOf(ui->tab_PinConfig));
    gEnv.pAppSettings->setValue("ButtonConfig",    ui->tabWidget->indexOf(ui->tab_ButtonConfig));
    gEnv.pAppSettings->setValue("AxesConfig",      ui->tabWidget->indexOf(ui->tab_AxesConfig));
    gEnv.pAppSettings->setValue("AxesCurves",      ui->tabWidget->indexOf(ui->tab_AxesCurves));
    gEnv.pAppSettings->setValue("ShiftRegs",       ui->tabWidget->indexOf(ui->tab_ShiftRegisters));
    gEnv.pAppSettings->setValue("Encoders",        ui->tabWidget->indexOf(ui->tab_Encoders));
    gEnv.pAppSettings->setValue("LED",             ui->tabWidget->indexOf(ui->tab_LED));
    gEnv.pAppSettings->setValue("AdvSettings",     ui->tabWidget->indexOf(ui->tab_AdvancedSettings));
    gEnv.pAppSettings->setValue("CurrentIndex",    ui->tabWidget->currentIndex());
    gEnv.pAppSettings->endGroup();
    // save window settings
    gEnv.pAppSettings->beginGroup("WindowSettings");
    gEnv.pAppSettings->setValue("Geometry",   this->saveGeometry());
    gEnv.pAppSettings->endGroup();
    // save font settings
    gEnv.pAppSettings->beginGroup("FontSettings");
    gEnv.pAppSettings->setValue("FontSize", QApplication::font().pointSize());
    gEnv.pAppSettings->endGroup();
}


                                                            ////////////////// buttons //////////////////
// comboBox selected device
void MainWindow::hidDeviceListChanged(int index)
{
    hid_device_worker->SetSelectedDevice(index);
}
// reset all pins
void MainWindow::on_pushButton_ResetAllPins_clicked()
{
    gEnv.pDeviceConfig->ResetConfig();

    ReadFromConfig();

    pin_config->ResetAllPins();
}
// read config from device
void MainWindow::on_pushButton_ReadConfig_clicked()
{
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);

    hid_device_worker->GetConfigFromDevice();
}
// write config to device
void MainWindow::on_pushButton_WriteConfig_clicked()
{
    ui->pushButton_WriteConfig->setEnabled(false);
    ui->pushButton_ReadConfig->setEnabled(false);

    WriteToConfig();
    hid_device_worker->SendConfigToDevice();
}
// load from file
void MainWindow::on_pushButton_LoadFromFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Config"), QDir::currentPath() + "/", tr("Config Files (*.cfg)"));

    QSettings device_settings( fileName, QSettings::IniFormat );
    LoadDeviceConfigFromFile(&device_settings);
}
// save to file
void MainWindow::on_pushButton_SaveToFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Config"), QDir::currentPath() + "/" + gEnv.pDeviceConfig->config.device_name, tr("Config Files (*.cfg)"));


    QSettings device_settings( fileName, QSettings::IniFormat );
    SaveDeviceConfigToFile(&device_settings);
}
//set default config
void MainWindow::on_pushButton_SetDefaultConfig_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Config"), QDir::currentPath() + "/", tr("Config Files (*.cfg)"));

    gEnv.pAppSettings->beginGroup("DefaultConfig");
    gEnv.pAppSettings->setValue("FileName", fileName);
    gEnv.pAppSettings->endGroup();
}
// load default config file
void MainWindow::on_pushButton_LoadDefaultConfig_clicked()
{
    gEnv.pAppSettings->beginGroup("DefaultConfig");

    if (gEnv.pAppSettings->value("FileName", "none") != "none"){
        QSettings app_settings( gEnv.pAppSettings->value("FileName", "none").toString(), QSettings::IniFormat );
        LoadDeviceConfigFromFile(&app_settings);
    }
    QSettings app_settings( "FreeJoySettings.conf", QSettings::IniFormat );
    gEnv.pAppSettings->endGroup();
}
// debug button
void MainWindow::on_pushButton_TestButton_clicked()
{

}




// add dyn // constructor
//    QGridLayout* layout = new QGridLayout;
//    Encoders* qwe = new Encoders;
//    layout->addWidget(qwe);
//    ui->tab_Encoders->setLayout(layout);
// dynamic widgets spawn
//    QScrollBar* scroll = ui->scrollArea_2->verticalScrollBar();
//    connect( scroll , SIGNAL(valueChanged(int)) ,this , SLOT(addvalues(int)) );

// dynamic widgets spawn
//void MainWindow::addvalues(int value)
//{
//    Q_UNUSED(value)
////    if (value > 90 && LogicButtonAdrList2.size()< 512)
////    {
////        for (int i = 0; i< 5; i++) {
////            ButtonConfig* qwe = new ButtonConfig;
////            ui->verticalLayout_3->addWidget(qwe, i);
////            LogicButtonAdrList2.append(qwe);
////        }
//////        QList<PinComboBox *> allPButtons = this->findChildren<PinComboBox *>();
//////        for (int i = 0; i< allPButtons.size(); i++) {
//////            allPButtons[i]->PinComboBox::GenText();
//////        }
////    }
//}
