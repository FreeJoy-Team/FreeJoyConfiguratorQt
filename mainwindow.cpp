#include <QThread>
#include <QTimer>
#include <QSettings>
#include <QFileDialog>
#include <ctime>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common_types.h"
#include "global.h"
#include "deviceconfig.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    clock_t tmp_clock = clock();
    qDebug()<<"start MainWindow constructor time ms ="<< clock() - *gEnv.pApp_start_time;
    ui->setupUi(this);

    //setWindowFlags( Qt::FramelessWindowHint );

    // версия контроллера
    QString str = QString::number(FIRMWARE_VERSION, 16);
    if (str.size() == 4){
        this->setWindowTitle(tr("FreeJoy Configurator") + " v" + str[0] + "." + str[1] + "." + str[2] + "b" + str[3]);
    }

//    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);

    // load application config
    LoadAppConfig();

    hid_device_worker = new HidDevice();
    thread = new QThread;

    thread_getSend_config = new QThread;

    // add dyn
//    QGridLayout* layout = new QGridLayout;
//    Encoders* qwe = new Encoders;
//    layout->addWidget(qwe);
//    ui->tab_Encoders->setLayout(layout);
    // dynamic widgets spawn
//    QScrollBar* scroll = ui->scrollArea_2->verticalScrollBar();
//    connect( scroll , SIGNAL(valueChanged(int)) ,this , SLOT(addvalues(int)) );

    qDebug()<<"befor load widgets time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    tmp_clock = clock();

                                            //////////////// ADD WIDGETS ////////////////
    // add pin config
    pin_config = new PinConfig(this);
    ui->layoutV_tabPinConfig->addWidget(pin_config);
    qDebug()<<"pin config load time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    tmp_clock = clock();
    // add button config
    button_config = new ButtonConfig(this);     // not need delete. this - parent
    ui->layoutV_tabButtonConfig->addWidget(button_config);
    qDebug()<<"button config load time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    tmp_clock = clock();
    // add axes config
    axes_config = new AxesConfig(this);
    ui->layoutV_tabAxesConfig->addWidget(axes_config);
    qDebug()<<"axes config load time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    tmp_clock = clock();
    // add axes curves config
    axes_curves_config = new AxesCurvesConfig(this);
    ui->layoutV_tabAxesCurvesConfig->addWidget(axes_curves_config);
    qDebug()<<"curves config load time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    tmp_clock = clock();
    // add axes to buttons config
    a2b_config = new AxesToButtonsConfig(this);
    ui->layoutV_tabAxesToButtons->addWidget(a2b_config);
    qDebug()<<"a2b config load time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    tmp_clock = clock();
    // add shift registers config
    shift_reg_config = new ShiftRegistersConfig(this);
    ui->layoutV_tabShiftRegistersConfig->addWidget(shift_reg_config);
    qDebug()<<"shift config load time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    tmp_clock = clock();
    // add encoders config
    encoder_config = new EncodersConfig(this);
    ui->layoutV_tabEncodersConfig->addWidget(encoder_config);
    qDebug()<<"encoder config load time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    tmp_clock = clock();
    // add led config
    led_config = new LedConfig(this);
    ui->layoutV_tabLedConfig->addWidget(led_config);
    qDebug()<<"led config load time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    tmp_clock = clock();


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
    connect(a2b_config, SIGNAL(a2bCountChanged(int)),
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



    hid_device_worker->moveToThread(thread);    //thread.data()

    connect(thread, SIGNAL(started()), hid_device_worker, SLOT(processData()));      //worker.data()

    connect(hid_device_worker, SIGNAL(putGamepadPacket(uint8_t *)),
                          this, SLOT(getGamepadPacket(uint8_t *)));
    connect(hid_device_worker, SIGNAL(putConnectedDeviceInfo()),
                          this, SLOT(showConnectDeviceInfo()));
    connect(hid_device_worker, SIGNAL(putDisconnectedDeviceInfo()),
                          this, SLOT(hideConnectDeviceInfo()));
    connect(hid_device_worker, SIGNAL(hidDeviceList(QStringList*)),
                          this, SLOT(hidDeviceList(QStringList*)));


    // load default config
    ReadFromConfig();

    // set style
    gEnv.pAppSettings->beginGroup("StyleSettings");
    if (gEnv.pAppSettings->value("StyleSheet", "default").toString() == "dark"){
        interfaceStyleChanged(true);
    } else {
        interfaceStyleChanged(false);
    }
    gEnv.pAppSettings->endGroup();


    thread->start();
    qDebug()<<"after widgets MainWindow constructor end time ms ="<< clock() - *gEnv.pApp_start_time - tmp_clock;
    qDebug()<<"start to MainWindow constructor end time ms ="<< clock() - *gEnv.pApp_start_time;
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
    delete thread_getSend_config;       // hz
    delete ui;
}


void MainWindow::ReadFromConfig()
{
    // read pin config
    pin_config->ReadFromConfig();
    // read axes config
    axes_config->ReadFromConfig();
    // read axes curves config
    axes_curves_config->ReadFromConfig();          // вынести в отдельную функцию
    // read axes to buttons config
    a2b_config->ReadFromConfig();
    // read shift registers config
    shift_reg_config->ReadFromConfig();
    // read encoder config
    encoder_config->ReadFromConfig();
    // read LED config
    led_config->ReadFromConfig();
    // read adv.settings config
    ui->widget_2->ReadFromConfig();
    // read button config
    button_config->ReadFromConfig();
}

void MainWindow::WriteToConfig()
{
    // write pin config
    pin_config->WriteToConfig();            // проверить снизу
    // write axes config
    axes_config->WriteToConfig();
    // write axes curves config
    axes_curves_config->WriteToConfig();
    // write axes to buttons config
    a2b_config->WriteToConfig();
    // write shift registers config
    shift_reg_config->WriteToConfig();
    // write encoder config
    encoder_config->WriteToConfig();
    // write LED config
    led_config->WriteToConfig();
    // write adv.settings config
    ui->widget_2->WriteToConfig();
    // write button config
    button_config->WriteToConfig();
}


void MainWindow::LoadAppConfig()
{
    // load language settings
    gEnv.pAppSettings->beginGroup("LanguageSettings");
    if (gEnv.pAppSettings->value("Language", "english").toString() == "russian")
    {
        translator.load(":/FreeJoyQt_ru");
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
        ui->widget_2->RetranslateUi();      // сделать уже нормально
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
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_AxesToButtons),
                                      gEnv.pAppSettings->value("AxesToButtons", ui->tabWidget->indexOf(ui->tab_AxesToButtons)).toInt());
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

void MainWindow::SaveAppConfig()
{
    // save tab index
    gEnv.pAppSettings->beginGroup("TabIndexSettings");
    gEnv.pAppSettings->setValue("PinConfig",       ui->tabWidget->indexOf(ui->tab_PinConfig));
    gEnv.pAppSettings->setValue("ButtonConfig",    ui->tabWidget->indexOf(ui->tab_ButtonConfig));
    gEnv.pAppSettings->setValue("AxesConfig",      ui->tabWidget->indexOf(ui->tab_AxesConfig));
    gEnv.pAppSettings->setValue("AxesCurves",      ui->tabWidget->indexOf(ui->tab_AxesCurves));
    gEnv.pAppSettings->setValue("AxesToButtons",   ui->tabWidget->indexOf(ui->tab_AxesToButtons));
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



void MainWindow::showConnectDeviceInfo() {
    ui->label_DeviceStatus->setText(tr("Connected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");
    //ui->label_DeviceStatus->setVisible(true);
    ui->pushButton_ReadConfig->setEnabled(true);
    ui->pushButton_WriteConfig->setEnabled(true);
    ui->widget_2->DeviceConnected(true);
    qDebug()<<"start time ms ="<< clock() - *gEnv.pApp_start_time;
}


void MainWindow::hideConnectDeviceInfo() {
    //ui->label_DeviceStatus->setVisible(false);
    ui->label_DeviceStatus->setText(tr("Disconnected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(160, 0, 0);");
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);
    ui->widget_2->DeviceConnected(false);
    axes_curves_config->DeviceStatus(false);
}

// slot interface style changed
void MainWindow::interfaceStyleChanged(bool is_dark)
{
    axes_curves_config->SetDarkInterface(is_dark);
}

// slot change language
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
    a2b_config->RetranslateUi();
    ui->widget_2->RetranslateUi();
}


// попробовать вынести в отдельный поток и повесить дилей
int asd = 0;
void MainWindow::getGamepadPacket(uint8_t * buff)            // НЕ В ЯДРЕ ВОРКЕРА
{
    ui->lineEdit->setText(QString::number(asd));
    report_convert.GamepadReport(buff);

    static clock_t timer;
    static bool change = false;

    if (!change)
    {
        timer = clock();
        change = true;
    }
    else if (change && clock() - timer > 17)    // обновление раз в 17мс, мб сделать дефайн в герцах
    {
        // optimization
        if(ui->tab_ButtonConfig->isVisible() == true){
            button_config->ButtonStateChanged();
        }
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

    asd++;
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
void MainWindow::on_pushButton_ReadConfig_clicked()        // херня? mb QtConcurrent::run()
{
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);
//    //ui->button_GetConfig->setEnabled(false);
    QEventLoop loop;
    QObject context;
    context.moveToThread(thread_getSend_config);
    connect(thread_getSend_config, &QThread::started, &context, [&]() {
        qDebug()<<"read start";
        emit getConfigDone(hid_device_worker->GetConfigFromDevice());
        qDebug()<<"read finish";
        loop.quit();
    });
    thread_getSend_config->start();
    loop.exec();
    thread_getSend_config->quit();
    thread_getSend_config->wait();
    ui->label_DebDeviceName->setText(gEnv.pDeviceConfig->config.device_name);
    ui->label_VID->setText(QString::number(gEnv.pDeviceConfig->config.vid));
}

// write config to device
void MainWindow::on_pushButton_WriteConfig_clicked()        // херня? mb QtConcurrent::run()
{
    ui->pushButton_WriteConfig->setEnabled(false);
    ui->pushButton_ReadConfig->setEnabled(false);

    WriteToConfig();

//    ui->button_SendConfig->setEnabled(false);
    QEventLoop loop;
    QObject context;
    context.moveToThread(thread_getSend_config);
    connect(thread_getSend_config, &QThread::started, &context, [&]() {
        qDebug()<<"write start";
        emit sendConfigDone(hid_device_worker->SendConfigToDevice());
        qDebug()<<"write finish";
        loop.quit();
    });
    thread_getSend_config->start();
    loop.exec();
    thread_getSend_config->quit();
    thread_getSend_config->wait();
}

void MainWindow::deviceFlasherController(bool is_start_flash)        // херня? mb QtConcurrent::run()
{
    QEventLoop loop;
    QObject context;
    context.moveToThread(thread_getSend_config);
    connect(thread_getSend_config, &QThread::started, &context, [&]() {
        if (is_start_flash == true){
            hid_device_worker->FlashFirmware(ui->widget_2->GetFileArray());
        } else {
            //ui->widget_2->DeviceConnected(hid_device_worker->EnterToFlashMode());
            hid_device_worker->EnterToFlashMode();
        }

        loop.quit();
    });
    thread_getSend_config->start();
    loop.exec();
    thread_getSend_config->quit();
    thread_getSend_config->wait();
}

// slot after receiving the config
void MainWindow::configReceived(bool success)        // повторное наатие
{
    button_default_style_ = ui->pushButton_ReadConfig->styleSheet();
    static QString button_default_text = ui->pushButton_ReadConfig->text();

    if (success == true){

        ReadFromConfig();

        // curves
        axes_curves_config->DeviceStatus(true);

        // set firmware version     // label_FirmwareVersion
        QString str = QString::number(gEnv.pDeviceConfig->config.firmware_version, 16);
        if (str.size() == 4){
            ui->label_DeviceStatus->setText(tr("Device firmware") + " v" + str[0] + "." + str[1] + "." + str[2] + "b" + str[3]);
        }

        ui->pushButton_ReadConfig->setText(tr("Received"));
        ui->pushButton_ReadConfig->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");

        QTimer::singleShot(1000, [&]{
            ui->pushButton_ReadConfig->setStyleSheet(button_default_style_);
            ui->pushButton_ReadConfig->setText(button_default_text);
            ui->pushButton_ReadConfig->setEnabled(true);
            ui->pushButton_WriteConfig->setEnabled(true);
        });
    } else {
        ui->pushButton_ReadConfig->setText(tr("Error"));
        ui->pushButton_ReadConfig->setStyleSheet("color: white; background-color: rgb(200, 0, 0);");

        QTimer::singleShot(1000, [&]{
            ui->pushButton_ReadConfig->setStyleSheet(button_default_style_);
            ui->pushButton_ReadConfig->setText(button_default_text);
            ui->pushButton_ReadConfig->setEnabled(true);
            ui->pushButton_WriteConfig->setEnabled(true);
        });
    }
}

// slot after sending the config
void MainWindow::configSent(bool success)
{
    button_default_style_ = ui->pushButton_ReadConfig->styleSheet();
    static QString button_default_text = ui->pushButton_WriteConfig->text();
    axes_curves_config->DeviceStatus(true);

    if (success == true){
        ui->pushButton_WriteConfig->setText(tr("Sent"));
        ui->pushButton_WriteConfig->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");

        QTimer::singleShot(1000, [&]{
            ui->pushButton_WriteConfig->setStyleSheet(button_default_style_);
            ui->pushButton_WriteConfig->setText(button_default_text);
            ui->pushButton_WriteConfig->setEnabled(true);
            ui->pushButton_ReadConfig->setEnabled(true);
        });
    } else {
        ui->pushButton_WriteConfig->setText(tr("Error"));
        ui->pushButton_WriteConfig->setStyleSheet("color: white; background-color: rgb(200, 0, 0);");

        QTimer::singleShot(1000, [&]{
            ui->pushButton_WriteConfig->setStyleSheet(button_default_style_);
            ui->pushButton_WriteConfig->setText(button_default_text);
            ui->pushButton_WriteConfig->setEnabled(true);
            ui->pushButton_ReadConfig->setEnabled(true);
        });
    }
}

void MainWindow::LoadDeviceConfigFromFile(QSettings* appS)
{
    // уменьшение текста
    dev_config_t* devc = &gEnv.pDeviceConfig->config;
    bool tmp;

    // load Device USB config from file
    appS->beginGroup("DeviceUsbConfig");

    devc->firmware_version = QString::number(appS->value("FirmwareVersion", devc->firmware_version).toInt()).toUShort(&tmp ,16);
    std::string tmp_string = appS->value("DeviceName", devc->device_name).toString().toStdString();
    for (uint i = 0; i < sizeof(devc->device_name); i++) {
        if (i < tmp_string.size()){
            devc->device_name[i] = tmp_string[i];
        } else {
            devc->device_name[i] = '\0';
        }
    }
    //devc->vid = QString::number(appS->value("Pid", devc->vid).toInt()).toShort(&tmp ,16);
    devc->pid = QString::number(appS->value("Pid", devc->pid).toInt()).toUShort(&tmp ,16);
    devc->is_dynamic_config = appS->value("DynamicHID", devc->is_dynamic_config).toInt();
    devc->exchange_period_ms = appS->value("USBExchange", devc->exchange_period_ms).toInt();
    appS->endGroup();

    // load Pins config from file
    appS->beginGroup("PinsConfig");

    devc->pins[0] = (int8_t)appS->value("A0", devc->pins[0]).toInt();   // вроде и без (int8_t) работает
    devc->pins[1] = (int8_t)appS->value("A1", devc->pins[1]).toInt();
    devc->pins[2] = (int8_t)appS->value("A2", devc->pins[2]).toInt();
    devc->pins[3] = (int8_t)appS->value("A3", devc->pins[3]).toInt();
    devc->pins[4] = (int8_t)appS->value("A4", devc->pins[4]).toInt();
    devc->pins[5] = (int8_t)appS->value("A5", devc->pins[5]).toInt();
    devc->pins[6] = (int8_t)appS->value("A6", devc->pins[6]).toInt();
    devc->pins[7] = (int8_t)appS->value("A7", devc->pins[7]).toInt();
    devc->pins[8] = (int8_t)appS->value("A8", devc->pins[8]).toInt();
    devc->pins[9] = (int8_t)appS->value("A9", devc->pins[9]).toInt();
    devc->pins[10] = (int8_t)appS->value("A10", devc->pins[10]).toInt();
    devc->pins[11] = (int8_t)appS->value("A15", devc->pins[11]).toInt();
    devc->pins[12] = (int8_t)appS->value("B0", devc->pins[12]).toInt();
    devc->pins[13] = (int8_t)appS->value("B1", devc->pins[13]).toInt();
    devc->pins[14] = (int8_t)appS->value("B3", devc->pins[14]).toInt();
    devc->pins[15] = (int8_t)appS->value("B4", devc->pins[15]).toInt();
    devc->pins[16] = (int8_t)appS->value("B5", devc->pins[16]).toInt();
    devc->pins[17] = (int8_t)appS->value("B6", devc->pins[17]).toInt();
    devc->pins[18] = (int8_t)appS->value("B7", devc->pins[18]).toInt();
    devc->pins[19] = (int8_t)appS->value("B8", devc->pins[19]).toInt();
    devc->pins[20] = (int8_t)appS->value("B9", devc->pins[20]).toInt();
    devc->pins[21] = (int8_t)appS->value("B10", devc->pins[21]).toInt();
    devc->pins[22] = (int8_t)appS->value("B11", devc->pins[22]).toInt();
    devc->pins[23] = (int8_t)appS->value("B12", devc->pins[23]).toInt();
    devc->pins[24] = (int8_t)appS->value("B13", devc->pins[24]).toInt();
    devc->pins[25] = (int8_t)appS->value("B14", devc->pins[25]).toInt();
    devc->pins[26] = (int8_t)appS->value("B15", devc->pins[26]).toInt();
    devc->pins[27] = (int8_t)appS->value("C13", devc->pins[27]).toInt();
    devc->pins[28] = (int8_t)appS->value("C14", devc->pins[28]).toInt();
    devc->pins[29] = (int8_t)appS->value("C15", devc->pins[29]).toInt();
    appS->endGroup();

    // load Shift config from file
    appS->beginGroup("ShiftConfig");
    for (int i = 0; i < SHIFT_COUNT - 1; ++i) { // -1 "No shift"    (SHIFT_COUNT = shift_count + "No shift")
        devc->shift_config[i].button = (int8_t)appS->value("Shift", devc->shift_config[i].button).toInt();
    }
    appS->endGroup();

    // load Timer config from file
    appS->beginGroup("TimersConfig");

    devc->button_timer1_ms = (uint16_t)appS->value("Timer1", devc->button_timer1_ms).toInt();   // вроде и без (int16_t) работает
    devc->button_timer2_ms = (uint16_t)appS->value("Timer2", devc->button_timer2_ms).toInt();
    devc->button_timer3_ms = (uint16_t)appS->value("Timer3", devc->button_timer3_ms).toInt();
    devc->button_debounce_ms = (uint16_t)appS->value("Debounce", devc->button_debounce_ms).toInt();
    devc->encoder_press_time_ms = (uint8_t)appS->value("EncoderPress", devc->encoder_press_time_ms).toInt();
    appS->endGroup();

    // load Buttons config from file
    for (int i = 0; i < MAX_BUTTONS_NUM; ++i) {
        appS->beginGroup("ButtonsConfig_" + QString::number(i));

        devc->buttons[i].physical_num = appS->value("ButtonPhysicNumber", devc->buttons[i].physical_num).toInt();
        devc->buttons[i].type = appS->value("ButtonType", devc->buttons[i].type).toInt();
        devc->buttons[i].shift_modificator = appS->value("ShiftMod", devc->buttons[i].shift_modificator).toInt();
        devc->buttons[i].is_inverted = appS->value("Inverted", devc->buttons[i].is_inverted).toInt();
        devc->buttons[i].is_disabled = appS->value("Disabled", devc->buttons[i].is_disabled).toInt();
        devc->buttons[i].delay_timer = appS->value("DelayTimer", devc->buttons[i].delay_timer).toInt();
        devc->buttons[i].press_timer = appS->value("PressTimer", devc->buttons[i].press_timer).toInt();
        appS->endGroup();
    }

    // load Axes config from file
    for (int i = 0; i < MAX_AXIS_NUM; ++i)
    {
        appS->beginGroup("AxesConfig_" + QString::number(i));

        devc->axis_config[i].calib_min = appS->value("CalibMin", devc->axis_config[i].calib_min).toInt();
        devc->axis_config[i].calib_center = appS->value("CalibCenter", devc->axis_config[i].calib_center).toInt();
        devc->axis_config[i].calib_max = appS->value("CalibMax", devc->axis_config[i].calib_max).toInt();

        devc->axis_config[i].out_enabled = appS->value("OutEnabled", devc->axis_config[i].out_enabled).toInt();
        devc->axis_config[i].inverted = appS->value("Inverted", devc->axis_config[i].inverted).toInt();
        devc->axis_config[i].function = appS->value("Function", devc->axis_config[i].function).toInt();
        devc->axis_config[i].filter = appS->value("Filter", devc->axis_config[i].filter).toInt();

        devc->axis_config[i].resolution = appS->value("Resolution", devc->axis_config[i].resolution).toInt();
        devc->axis_config[i].channel = appS->value("Channel", devc->axis_config[i].channel).toInt();
        devc->axis_config[i].deadband_size = appS->value("DeadbandSize", devc->axis_config[i].deadband_size).toInt();
        devc->axis_config[i].is_dynamic_deadband = appS->value("DynDeadbandEnabled", devc->axis_config[i].is_dynamic_deadband).toInt();

        devc->axis_config[i].source_main = appS->value("SourceMain", devc->axis_config[i].source_main).toInt();
        devc->axis_config[i].source_secondary = appS->value("SourceSecond", devc->axis_config[i].source_secondary).toInt();
        devc->axis_config[i].offset_angle = appS->value("OffsetAngle", devc->axis_config[i].offset_angle).toInt();

        devc->axis_config[i].button1 = appS->value("Button1", devc->axis_config[i].button1).toInt();
        devc->axis_config[i].button2 = appS->value("Button2", devc->axis_config[i].button2).toInt();
        devc->axis_config[i].button3 = appS->value("Button3", devc->axis_config[i].button3).toInt();
        devc->axis_config[i].divider = appS->value("Divider", devc->axis_config[i].divider).toInt();
        devc->axis_config[i].i2c_address = appS->value("I2cAddress", devc->axis_config[i].i2c_address).toInt();
        devc->axis_config[i].button1_type = appS->value("Button1Type", devc->axis_config[i].button1_type).toInt();
        devc->axis_config[i].button2_type = appS->value("Button2Type", devc->axis_config[i].button2_type).toInt();
        devc->axis_config[i].button3_type = appS->value("Button3Type", devc->axis_config[i].button3_type).toInt();
        devc->axis_config[i].prescaler = appS->value("Prescaler", devc->axis_config[i].prescaler).toInt();
        appS->endGroup();
        // axes curves
        appS->beginGroup("AxesCurvesConfig_" + QString::number(i));
        for (int j = 0; j < 11; ++j) {      // 11 - axis curve points count
            devc->axis_config[i].curve_shape[j] = appS->value("Point_" + QString::number(j), devc->axis_config[i].curve_shape[j]).toInt();
        }
        appS->endGroup();
    }

    // load Axes To Buttons config from file
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        appS->beginGroup("Axes2bConfig_" + QString::number(i));

        devc->axes_to_buttons[i].buttons_cnt = appS->value("ButtonsCount", devc->axes_to_buttons[i].buttons_cnt).toInt();
        devc->axes_to_buttons[i].is_enabled = appS->value("Enabled", devc->axes_to_buttons[i].is_enabled).toInt();
        for (int j = 0; j < MAX_A2B_BUTTONS + 1; ++j) {
            devc->axes_to_buttons[i].points[j] = appS->value("Point_" + QString::number(j), devc->axes_to_buttons[i].points[j]).toInt();
        }
        appS->endGroup();
    }

    // load Shift Registers config from file
    for (int i = 0; i < MAX_SHIFT_REG_NUM; ++i) {
        appS->beginGroup("ShiftRegsConfig_" + QString::number(i));

        devc->shift_registers[i].type = appS->value("ShiftType", devc->shift_registers[i].type).toInt();
        devc->shift_registers[i].button_cnt = appS->value("ButtonCnt", devc->shift_registers[i].button_cnt).toInt();
        appS->endGroup();
    }

    // load Encoders config from file
    for (int i = 0; i < MAX_ENCODERS_NUM; ++i) {
        appS->beginGroup("EncodersConfig_" + QString::number(i));

        devc->encoders[i] = appS->value("EncType", devc->encoders[i]).toInt();
        appS->endGroup();
    }

    // load LEDs config from file
    appS->beginGroup("LedsPWMConfig");

    devc->led_pwm_config.duty_cycle[0] = appS->value("PinPB0", devc->led_pwm_config.duty_cycle[0]).toInt();
    devc->led_pwm_config.duty_cycle[1] = appS->value("PinPB1", devc->led_pwm_config.duty_cycle[1]).toInt();
    devc->led_pwm_config.duty_cycle[2] = appS->value("PinPB4", devc->led_pwm_config.duty_cycle[2]).toInt();
    appS->endGroup();

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        appS->beginGroup("LedsConfig_" + QString::number(i));

        devc->leds[i].input_num = appS->value("InputNum", devc->leds[i].input_num).toInt();
        devc->leds[i].type = appS->value("LedType", devc->leds[i].type).toInt();
        appS->endGroup();
    }

    ReadFromConfig();
}

void MainWindow::SaveDeviceConfigToFile(QSettings* appS)
{
    WriteToConfig();

    // save Device USB config to file
    appS->beginGroup("DeviceUsbConfig");
    appS->setValue("FirmwareVersion", QString::number(gEnv.pDeviceConfig->config.firmware_version, 16));
    appS->setValue("DeviceName", gEnv.pDeviceConfig->config.device_name);
    //appS->setValue("Vid", QString::number(gEnv.pDeviceConfig->config.vid, 16));
    appS->setValue("Pid", QString::number(gEnv.pDeviceConfig->config.pid, 16));
    appS->setValue("DynamicHID", gEnv.pDeviceConfig->config.is_dynamic_config);
    appS->setValue("USBExchange", gEnv.pDeviceConfig->config.exchange_period_ms);
    appS->endGroup();
    // save Pins config to file
    appS->beginGroup("PinsConfig");        // can for x3
    appS->setValue("A0", gEnv.pDeviceConfig->config.pins[0]);
    appS->setValue("A1", gEnv.pDeviceConfig->config.pins[1]);
    appS->setValue("A2", gEnv.pDeviceConfig->config.pins[2]);
    appS->setValue("A3", gEnv.pDeviceConfig->config.pins[3]);
    appS->setValue("A4", gEnv.pDeviceConfig->config.pins[4]);
    appS->setValue("A5", gEnv.pDeviceConfig->config.pins[5]);
    appS->setValue("A6", gEnv.pDeviceConfig->config.pins[6]);
    appS->setValue("A7", gEnv.pDeviceConfig->config.pins[7]);
    appS->setValue("A8", gEnv.pDeviceConfig->config.pins[8]);
    appS->setValue("A9", gEnv.pDeviceConfig->config.pins[9]);
    appS->setValue("A10", gEnv.pDeviceConfig->config.pins[10]);
    appS->setValue("A15", gEnv.pDeviceConfig->config.pins[11]);
    appS->setValue("B0", gEnv.pDeviceConfig->config.pins[12]);
    appS->setValue("B1", gEnv.pDeviceConfig->config.pins[13]);
    appS->setValue("B3", gEnv.pDeviceConfig->config.pins[14]);
    appS->setValue("B4", gEnv.pDeviceConfig->config.pins[15]);
    appS->setValue("B5", gEnv.pDeviceConfig->config.pins[16]);
    appS->setValue("B6", gEnv.pDeviceConfig->config.pins[17]);
    appS->setValue("B7", gEnv.pDeviceConfig->config.pins[18]);
    appS->setValue("B8", gEnv.pDeviceConfig->config.pins[19]);
    appS->setValue("B9", gEnv.pDeviceConfig->config.pins[20]);
    appS->setValue("B10", gEnv.pDeviceConfig->config.pins[21]);
    appS->setValue("B11", gEnv.pDeviceConfig->config.pins[22]);
    appS->setValue("B12", gEnv.pDeviceConfig->config.pins[23]);
    appS->setValue("B13", gEnv.pDeviceConfig->config.pins[24]);
    appS->setValue("B14", gEnv.pDeviceConfig->config.pins[25]);
    appS->setValue("B15", gEnv.pDeviceConfig->config.pins[26]);
    appS->setValue("C13", gEnv.pDeviceConfig->config.pins[27]);
    appS->setValue("C14", gEnv.pDeviceConfig->config.pins[28]);
    appS->setValue("C15", gEnv.pDeviceConfig->config.pins[29]);
    appS->endGroup();

    // save Shift config to file
    appS->beginGroup("ShiftConfig");
    for (int i = 0; i < SHIFT_COUNT - 1; ++i) { // -1 "No shift"    (SHIFT_COUNT = shift_count + "No shift")
        appS->setValue("Shift" + QString::number(i), gEnv.pDeviceConfig->config.shift_config[i].button);
    }
    appS->endGroup();

    // save Timer config to file
    appS->beginGroup("TimersConfig");

    appS->setValue("Timer1", gEnv.pDeviceConfig->config.button_timer1_ms);
    appS->setValue("Timer2", gEnv.pDeviceConfig->config.button_timer2_ms);
    appS->setValue("Timer3", gEnv.pDeviceConfig->config.button_timer3_ms);
    appS->setValue("Debounce", gEnv.pDeviceConfig->config.button_debounce_ms);
    appS->setValue("EncoderPress", gEnv.pDeviceConfig->config.encoder_press_time_ms);
    appS->endGroup();

    // save Buttons config to file
    for (int i = 0; i < MAX_BUTTONS_NUM; ++i) {
        appS->beginGroup("ButtonsConfig_" + QString::number(i));

        appS->setValue("ButtonPhysicNumber", gEnv.pDeviceConfig->config.buttons[i].physical_num);
        appS->setValue("ButtonType", gEnv.pDeviceConfig->config.buttons[i].type);
        appS->setValue("ShiftMod", gEnv.pDeviceConfig->config.buttons[i].shift_modificator);
        appS->setValue("Inverted", gEnv.pDeviceConfig->config.buttons[i].is_inverted);
        appS->setValue("Disabled", gEnv.pDeviceConfig->config.buttons[i].is_disabled);
        appS->setValue("DelayTimer", gEnv.pDeviceConfig->config.buttons[i].delay_timer);
        appS->setValue("PressTimer", gEnv.pDeviceConfig->config.buttons[i].press_timer);
        appS->endGroup();
    }

    // save Axes config to file
    for (int i = 0; i < MAX_AXIS_NUM; ++i)
    {
        appS->beginGroup("AxesConfig_" + QString::number(i));

        appS->setValue("CalibMin", gEnv.pDeviceConfig->config.axis_config[i].calib_min);
        appS->setValue("CalibCenter", gEnv.pDeviceConfig->config.axis_config[i].calib_center);
        appS->setValue("CalibMax", gEnv.pDeviceConfig->config.axis_config[i].calib_max);

        appS->setValue("OutEnabled", gEnv.pDeviceConfig->config.axis_config[i].out_enabled);
        appS->setValue("Inverted", gEnv.pDeviceConfig->config.axis_config[i].inverted);
        appS->setValue("Function", gEnv.pDeviceConfig->config.axis_config[i].function);
        appS->setValue("Filter", gEnv.pDeviceConfig->config.axis_config[i].filter);

        appS->setValue("Resolution", gEnv.pDeviceConfig->config.axis_config[i].resolution);
        appS->setValue("Channel", gEnv.pDeviceConfig->config.axis_config[i].channel);
        appS->setValue("DeadbandSize", gEnv.pDeviceConfig->config.axis_config[i].deadband_size);
        appS->setValue("DynDeadbandEnabled", gEnv.pDeviceConfig->config.axis_config[i].is_dynamic_deadband);

        appS->setValue("SourceMain", gEnv.pDeviceConfig->config.axis_config[i].source_main);
        appS->setValue("SourceSecond", gEnv.pDeviceConfig->config.axis_config[i].source_secondary);
        appS->setValue("OffsetAngle", gEnv.pDeviceConfig->config.axis_config[i].offset_angle);

        appS->setValue("Button1", gEnv.pDeviceConfig->config.axis_config[i].button1);
        appS->setValue("Button2", gEnv.pDeviceConfig->config.axis_config[i].button2);
        appS->setValue("Button3", gEnv.pDeviceConfig->config.axis_config[i].button3);
        appS->setValue("Divider", gEnv.pDeviceConfig->config.axis_config[i].divider);
        appS->setValue("I2cAddress", gEnv.pDeviceConfig->config.axis_config[i].i2c_address);
        appS->setValue("Button1Type", gEnv.pDeviceConfig->config.axis_config[i].button1_type);
        appS->setValue("Button2Type", gEnv.pDeviceConfig->config.axis_config[i].button2_type);
        appS->setValue("Button3Type", gEnv.pDeviceConfig->config.axis_config[i].button3_type);
        appS->setValue("Prescaler", gEnv.pDeviceConfig->config.axis_config[i].prescaler);
        appS->endGroup();
        // axes curves
        appS->beginGroup("AxesCurvesConfig_" + QString::number(i));
        for (int j = 0; j < 11; ++j) {      // 11 - axis curve points count
            appS->setValue("Point_" + QString::number(j), gEnv.pDeviceConfig->config.axis_config[i].curve_shape[j]);
        }
        appS->endGroup();
    }

    // save Axes To Buttons config to file
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        appS->beginGroup("Axes2bConfig_" + QString::number(i));

        appS->setValue("ButtonsCount", gEnv.pDeviceConfig->config.axes_to_buttons[i].buttons_cnt);
        appS->setValue("Enabled", gEnv.pDeviceConfig->config.axes_to_buttons[i].is_enabled);
        for (int j = 0; j < MAX_A2B_BUTTONS + 1; ++j) {
            appS->setValue("Point_" + QString::number(j), gEnv.pDeviceConfig->config.axes_to_buttons[i].points[j]);
        }
        appS->endGroup();
    }

    // save Shift Registers config to file
    for (int i = 0; i < MAX_SHIFT_REG_NUM; ++i) {
        appS->beginGroup("ShiftRegsConfig_" + QString::number(i));

        appS->setValue("ShiftType", gEnv.pDeviceConfig->config.shift_registers[i].type);
        appS->setValue("ButtonCnt", gEnv.pDeviceConfig->config.shift_registers[i].button_cnt);
        appS->endGroup();
    }

    // save Encoders config to file
    for (int i = 0; i < MAX_ENCODERS_NUM; ++i) {
        appS->beginGroup("EncodersConfig_" + QString::number(i));

        appS->setValue("EncType", gEnv.pDeviceConfig->config.encoders[i]);
        appS->endGroup();
    }

    // save LEDs config to file
    appS->beginGroup("LedsPWMConfig");
    appS->setValue("PinPB0", gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[0]);
    appS->setValue("PinPB1", gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[1]);
    appS->setValue("PinPB4", gEnv.pDeviceConfig->config.led_pwm_config.duty_cycle[2]);
    appS->endGroup();

    for (int i = 0; i < MAX_LEDS_NUM; ++i) {
        appS->beginGroup("LedsConfig_" + QString::number(i));

        appS->setValue("InputNum", gEnv.pDeviceConfig->config.leds[i].input_num);
        appS->setValue("LedType", gEnv.pDeviceConfig->config.leds[i].type);
        appS->endGroup();
    }
}



//                                                                       // set text in centr
//    QList<PinComboBox *> allPButtons = this->findChildren<PinComboBox *>();
//    for (int i = 0; i < allPButtons.size(); i++)
//    {
//        int index = allPButtons[i]->PinComboBox::GetIndex();
//        QString name = gEnv.pAppConfig->pin_list[index].gui_name;

//        int width = allPButtons[i]->width();
//        width = width/2 - name.length()*3;

//        QString padding = QString::number(width);
//        QString styleSheet = "QComboBox { padding-left: %1px;}";
//        allPButtons[i]->setStyleSheet( styleSheet.arg(padding) );
//    }
void MainWindow::resizeEvent(QResizeEvent* event)                       // SLOW
{
   QMainWindow::resizeEvent(event);

//   static clock_t time2;
//   static bool change = false;
//   if (!change)
//   {
//       time2 = clock();
//       change = true;
//   }
//   else if (change && clock() - time2 > 50)
//   {
//       QList<PinComboBox *> allPButtons = this->findChildren<PinComboBox *>();
//       for (int i = 0; i < allPButtons.size(); i++)
//       {
//           int index = allPButtons[i]->PinComboBox::GetIndex();
//           QString name = gEnv.pAppConfig->pin_list[index].gui_name;

//           int width = allPButtons[i]->width();
//           width = width/2 - name.length()*3;

//           QString padding = QString::number(width);
//           QString styleSheet = "QComboBox { padding-left: %1px;}";
//           allPButtons[i]->setStyleSheet( styleSheet.arg(padding) );
//       }
//       change = false;
//   }
}


// dynamic widgets spawn
void MainWindow::addvalues(int value)
{
    Q_UNUSED(value)
//    if (value > 90 && LogicButtonAdrList2.size()< 512)
//    {
//        for (int i = 0; i< 5; i++) {
//            ButtonConfig* qwe = new ButtonConfig;
//            ui->verticalLayout_3->addWidget(qwe, i);
//            LogicButtonAdrList2.append(qwe);
//        }
////        QList<PinComboBox *> allPButtons = this->findChildren<PinComboBox *>();
////        for (int i = 0; i< allPButtons.size(); i++) {
////            allPButtons[i]->PinComboBox::GenText();
////        }
//    }
}

void MainWindow::on_pushButton_LoadFromFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Config"), QDir::currentPath() + "/", tr("Config Files (*.cfg)"));

    QSettings device_settings( fileName, QSettings::IniFormat );
    LoadDeviceConfigFromFile(&device_settings);
}

void MainWindow::on_pushButton_SaveToFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Config"), QDir::currentPath() + "/" + gEnv.pDeviceConfig->config.device_name, tr("Config Files (*.cfg)"));


    QSettings device_settings( fileName, QSettings::IniFormat );
    SaveDeviceConfigToFile(&device_settings);
}


void MainWindow::on_pushButton_SetDefaultConfig_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Config"), QDir::currentPath() + "/", tr("Config Files (*.cfg)"));

    gEnv.pAppSettings->beginGroup("DefaultConfig");
    gEnv.pAppSettings->setValue("FileName", fileName);
    gEnv.pAppSettings->endGroup();
}

void MainWindow::on_pushButton_LoadDefaultConfig_clicked()
{
    // load default config file LoadDeviceConfigFromFile
    gEnv.pAppSettings->beginGroup("DefaultConfig");

    if (gEnv.pAppSettings->value("FileName", "none") != "none"){
        QSettings app_settings( gEnv.pAppSettings->value("FileName", "none").toString(), QSettings::IniFormat );
        LoadDeviceConfigFromFile(&app_settings);
    }
    QSettings app_settings( "FreeJoySettings.conf", QSettings::IniFormat );
    gEnv.pAppSettings->endGroup();
}

void MainWindow::on_pushButton_TestButton_clicked()
{

}
