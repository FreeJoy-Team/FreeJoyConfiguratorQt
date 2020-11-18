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
    , thread_{new QThread}
    , hid_device_worker_{new HidDevice()}
    , thread_getSend_config_{new QThread}
    , pin_config_{new PinConfig(this)}// not need delete. this - parent
    , button_config_{new ButtonConfig(this)}
    , led_config_{new LedConfig(this)}
    , encoder_config_{new EncodersConfig(this)}
    , shift_reg_config_{new ShiftRegistersConfig(this)}
    , axes_config_{new AxesConfig(this)}
    , axes_curves_config_{new AxesCurvesConfig(this)}
    , adv_settings_{new AdvancedSettings(this)}
{
    qDebug()<<"main + member initialize time ="<< gEnv.pApp_start_time->elapsed() << "ms";
    QElapsedTimer timer;
    timer.start();

    ui->setupUi(this);

    QMainWindow::setWindowIcon(QIcon(":/Images/icon-32.png"));
    //setWindowFlags( Qt::FramelessWindowHint );

    // firmware version
    QString str = QString::number(FIRMWARE_VERSION, 16);
    if (str.size() >= 4){
        setWindowTitle(tr("FreeJoy Configurator") + " v" + str[0] + "." + str[1] + "." + str[2] + " (" + gEnv.pAppVersion + ")");// + "b" + str[3]);
    }

    // load application config
    // debug window // ну так се - дебаг=нуллптр до LoadAppConfig конфига, но не жрёт ресурсов если дебаг выкл
    //debug_window_ = nullptr;
    LoadAppConfig();

//    hid_device_worker_ = new HidDevice();
//    thread_ = new QThread;
//    thread_getSend_config_ = new QThread;

//    index_device_before_write_ = 0;

    qDebug()<<"before add widgets ="<< timer.restart() << "ms";
                                            //////////////// ADD WIDGETS ////////////////
    // add pin widget
    ui->layoutV_tabPinConfig->addWidget(pin_config_);
    qDebug()<<"pin config load time ="<< timer.restart() << "ms";
    // add button widget
    ui->layoutV_tabButtonConfig->addWidget(button_config_);
    qDebug()<<"button config load time ="<< timer.restart() << "ms";
    // add axes widget
    ui->layoutV_tabAxesConfig->addWidget(axes_config_);
    qDebug()<<"axes config load time ="<< timer.restart() << "ms";
    // add axes curves widget
    ui->layoutV_tabAxesCurvesConfig->addWidget(axes_curves_config_);
    qDebug()<<"curves config load time ="<< timer.restart() << "ms";
    // add shift registers widget
    ui->layoutV_tabShiftRegistersConfig->addWidget(shift_reg_config_);
    qDebug()<<"shift config load time ="<< timer.restart() << "ms";
    // add encoders widget
    ui->layoutV_tabEncodersConfig->addWidget(encoder_config_);
    qDebug()<<"encoder config load time ="<< timer.restart() << "ms";
    // add led widget
    ui->layoutV_tabLedConfig->addWidget(led_config_);
    qDebug()<<"led config load time ="<< timer.restart() << "ms";
    // add advanced settings widget
    ui->layoutV_tabAdvSettings->addWidget(adv_settings_);
    qDebug()<<"advanced settings load time ="<< timer.restart() << "ms";



    // strong focus for mouse wheel
    // без протекта можно при прокручивании страницы случайно навести на комбобокс и изменить его колесом мыши
    // при установке setFocusPolicy(Qt::StrongFocus) и протекта на комбобокс придётся нажать, для прокручивания колесом
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
    // хз так или сверху исключать?
    for (auto&& child: pin_config_->findChildren<PinComboBox *>())
    {
        for (auto&& comBox: child->findChildren<QComboBox *>())
        {
            comBox->setFocusPolicy(Qt::WheelFocus);
        }
    }
    for (auto&& comBox: axes_curves_config_->findChildren<QComboBox *>())
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
    connect(pin_config_, SIGNAL(totalButtonsValueChanged(int)),
                button_config_, SLOT(setUiOnOff(int)));
    // LEDs changed
    connect(pin_config_, SIGNAL(totalLEDsValueChanged(int)),
                led_config_, SLOT(SpawnLEDs(int)));
    // encoder changed
    connect(button_config_, SIGNAL(encoderInputChanged(int, int)),
            encoder_config_, SLOT(encoderInputChanged(int, int)));
    // fast encoder
    connect(pin_config_, SIGNAL(fastEncoderSelected(QString, bool)),
            encoder_config_, SLOT(fastEncoderSelected(QString, bool)));
    // shift registers
    connect(pin_config_, SIGNAL(shiftRegSelected(int, int, QString)),
            shift_reg_config_, SLOT(shiftRegSelected(int, int, QString)));
    // a2b count
    connect(axes_config_, SIGNAL(a2bCountChanged(int)),
            pin_config_, SLOT(a2bCountChanged(int)));
    // shift reg buttons count shiftRegsButtonsCount
    connect(shift_reg_config_, SIGNAL(shiftRegButtonsCountChanged(int)),
            pin_config_, SLOT(shiftRegButtonsCountChanged(int)));
    // axes source changed//axesSourceChanged
    connect(pin_config_, SIGNAL(axesSourceChanged(int, bool)),
            axes_config_, SLOT(addOrDeleteMainSource(int, bool)));
    // language changed
    connect(adv_settings_, SIGNAL(languageChanged(QString)),
            this, SLOT(languageChanged(QString)));
    // interface changed
    connect(adv_settings_, SIGNAL(interfaceStyleChanged(bool)),
            this, SLOT(interfaceStyleChanged(bool)));
    // font changed
    connect(adv_settings_, SIGNAL(fontChanged()),
            this, SLOT(setFont()));


    // enter flash mode clicked
    connect(adv_settings_->GetFlasher(), SIGNAL(flashModeClicked(bool)),
            this, SLOT(deviceFlasherController(bool)));
    // flasher found
    connect(hid_device_worker_, SIGNAL(flasherFound(bool)),
            adv_settings_->GetFlasher(), SLOT(flasherFound(bool)));
    // start flash
    connect(adv_settings_->GetFlasher(), SIGNAL(startFlash(bool)),              // thread  flashStatus
            this, SLOT(deviceFlasherController(bool)));
    // flash status
    connect(hid_device_worker_, SIGNAL(flashStatus(int, int)),              // thread  flashStatus
            adv_settings_->GetFlasher(), SLOT(flashStatus(int, int)));
    // set selected hid device
    connect(ui->comboBox_HidDeviceList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(hidDeviceListChanged(int)));


    // HID worker
    hid_device_worker_->moveToThread(thread_);

    connect(thread_, SIGNAL(started()), hid_device_worker_, SLOT(processData()));

    connect(hid_device_worker_, SIGNAL(putGamepadPacket(uint8_t *)),
                          this, SLOT(getGamepadPacket(uint8_t *)));
    connect(hid_device_worker_, SIGNAL(putConnectedDeviceInfo()),
                          this, SLOT(showConnectDeviceInfo()));
    connect(hid_device_worker_, SIGNAL(putDisconnectedDeviceInfo()),
                          this, SLOT(hideConnectDeviceInfo()));
    connect(hid_device_worker_, SIGNAL(hidDeviceList(QStringList*)),
                          this, SLOT(hidDeviceList(QStringList*)));


    // read config from device
    connect(hid_device_worker_, SIGNAL(configReceived(bool)),
            this, SLOT(configReceived(bool)));
    // write config to device
    connect(hid_device_worker_, SIGNAL(configSent(bool)),
            this, SLOT(configSent(bool)));


    // load default config // loading will occur after loading buttons config
    // комбобоксы у кнопок заполняются после старта приложения и конфиг должен
    // запускаться сигналом от кнопок
    connect(button_config_, SIGNAL(logicalButtonsCreated()),
            this, SLOT(loadDefaultConfig()));


    // set style
    gEnv.pAppSettings->beginGroup("StyleSettings");
    if (gEnv.pAppSettings->value("StyleSheet", "default").toString() == "dark"){
        interfaceStyleChanged(true);
    } else {
        interfaceStyleChanged(false);
    }
    gEnv.pAppSettings->endGroup();


    thread_->start();

    qDebug()<<"after widgets load time ="<< timer.elapsed() << "ms";
    qDebug()<<"Application Start Time ="<< gEnv.pApp_start_time->elapsed() << "ms";
}

MainWindow::~MainWindow()
{
    SaveAppConfig();
    hid_device_worker_->SetIsFinish(true);
    thread_->quit();
    thread_->deleteLater();
    thread_->wait();
    thread_getSend_config_->quit();
    thread_getSend_config_->deleteLater();
    thread_getSend_config_->wait();
    delete hid_device_worker_;
    delete thread_;              // не уверен в нужности, если есть thread->deleteLater();
    delete thread_getSend_config_;
    //delete thread_getSend_config;       // hz
    delete ui;
}




                                                        ///////////////////// device reports /////////////////////
// device connected
void MainWindow::showConnectDeviceInfo() {
    //qDebug()<<"Device connected";
    ui->label_DeviceStatus->setText(tr("Connected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");
    ui->pushButton_ReadConfig->setEnabled(true);
    ui->pushButton_WriteConfig->setEnabled(true);
    adv_settings_->GetFlasher()->DeviceConnected(true);
}
// device disconnected
void MainWindow::hideConnectDeviceInfo() {
    //qDebug()<<"Device disconected";
    ui->label_DeviceStatus->setText(tr("Disconnected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(160, 0, 0);");
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);
    adv_settings_->GetFlasher()->DeviceConnected(false);
    if (debug_window_){
        debug_window_->ResetPacketsCount();
    }

    // disable curve point
    QTimer::singleShot(3000, [&]{   // не лучший способ
        if (ui->pushButton_ReadConfig->isEnabled() == false){
            axes_curves_config_->DeviceStatus(false);
        }
    });
    //axes_curves_config->DeviceStatus(false);
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
//        if (ui->comboBox_HidDeviceList->count() >= index_device_before_write_){
//            ui->comboBox_HidDeviceList->setCurrentIndex(index_device_before_write_);
//        }
    }
}

// received device report
void MainWindow::getGamepadPacket(uint8_t * buff)
{
    report_convert_.GamepadReport(buff);

    // update button state without delay. fix gamepad_report.raw_button_data[0]
    // из-за задержки может не ловить изменения первых физических 64 кнопок или оставшихся. Например, может подряд попасться gamepad_report.raw_button_data[0] = 0
    // и не видеть оставшиеся физические 64 кнопки. ленивый и неоптимизированный фикс
    if(ui->tab_ButtonConfig->isVisible() == true || debug_window_){
        button_config_->ButtonStateChanged();
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
        if(ui->tab_LED->isVisible() == true){
            led_config_->LedStateChanged();
        }
        if(ui->tab_AxesConfig->isVisible() == true){
            axes_config_->AxesValueChanged();
        }
        if(ui->tab_AxesCurves->isVisible() == true){
            axes_curves_config_->UpdateAxesCurves();
        }
        change = false;
    }
    // debug info
    if (debug_window_){
        debug_window_->DevicePacketReceived();
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
    // так оставить или как read/write а bool через сигнал?
    QEventLoop loop;
    QObject context;
    context.moveToThread(thread_getSend_config_);
    connect(thread_getSend_config_, &QThread::started, &context, [&]() {
        qDebug()<<"Start flasher controller";
        if (is_start_flash == true){
            qDebug()<<"Start flash";
            hid_device_worker_->FlashFirmware(adv_settings_->GetFlasher()->GetFileArray());
        } else {
            qDebug()<<"Enter to flash mode";
            hid_device_worker_->EnterToFlashMode();
        }
        qDebug()<<"Flasher controller finished";
        loop.quit();
    });
    thread_getSend_config_->start();
    loop.exec();
    thread_getSend_config_->quit();
    thread_getSend_config_->wait();
}




                                            /////////////////////                 /////////////////////

// slot interface style changed
void MainWindow::interfaceStyleChanged(bool is_dark)
{
    axes_curves_config_->SetDarkInterface(is_dark);
}

// slot language change
void MainWindow::languageChanged(QString language)        // QSignalBlocker blocker(ui->comboBox);
{
    qDebug()<<"Retranslate UI";
    if (language == "russian")
    {
        translator_.load(":/FreeJoyQt_ru");// + QString("ru_RU"));//QLocale::system().name();//QString("ru_RU"));//QLocale::name());
        qApp->installTranslator(&translator_);
        ui->retranslateUi(this);
    }
    else if (language == "english")
    {
        translator_.load(":/FreeJoyQt_en");
        qApp->installTranslator(&translator_);
        ui->retranslateUi(this);
    } else {
        return;
    }
    pin_config_->RetranslateUi();
    button_config_->RetranslateUi();
    led_config_->RetranslateUi();
    encoder_config_->RetranslateUi();
    shift_reg_config_->RetranslateUi();
    axes_config_->RetranslateUi();
    axes_curves_config_->RetranslateUi();
    adv_settings_->RetranslateUi();
    if(debug_window_){
        debug_window_->RetranslateUi();
        ui->pushButton_ShowDebug->setText(tr("Hide debug"));
    }
    qDebug()<<"done";
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
    qDebug()<<"Loading application config";
    // load language settings
    gEnv.pAppSettings->beginGroup("LanguageSettings");
    if (gEnv.pAppSettings->value("Language", "english").toString() == "russian")
    {
        translator_.load(":/FreeJoyQt_ru");
        qApp->installTranslator(&translator_);
        ui->retranslateUi(this);
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

    // load debug window
    gEnv.pAppSettings->beginGroup("OtherSettings");
    debug_is_enable_ = gEnv.pAppSettings->value("DebugEnable", "false").toBool();
    if (debug_is_enable_){
        on_pushButton_ShowDebug_clicked();
        if (this->isMaximized() == false){
            resize(width(), height() - 120 - ui->layoutG_MainLayout->verticalSpacing());
        }
    }
    gEnv.pAppSettings->endGroup();

    //debug tab, only for debug build
    #ifdef QT_DEBUG
    #else
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tab_Debug));
    #endif
    qDebug()<<"Finished loading application config";
}
// save app config
void MainWindow::SaveAppConfig()    // перенести сюда сохранение профилей кривых
{
    qDebug()<<"Saving application config";
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
    // save debug
    gEnv.pAppSettings->beginGroup("OtherSettings");
    gEnv.pAppSettings->setValue("DebugEnable", debug_is_enable_);
    gEnv.pAppSettings->endGroup();
    qDebug()<<"done";
}


                                                            ////////////////// buttons //////////////////
// comboBox selected device
void MainWindow::hidDeviceListChanged(int index)
{
    hid_device_worker_->SetSelectedDevice(index);
}
// reset all pins
void MainWindow::on_pushButton_ResetAllPins_clicked()
{
    qDebug()<<"Reset all started";
    gEnv.pDeviceConfig->ResetConfig();

    ReadFromConfig();

    pin_config_->ResetAllPins();
    qDebug()<<"done";
}
// read config from device
void MainWindow::on_pushButton_ReadConfig_clicked()
{
    qDebug()<<"Read config started";
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);

    hid_device_worker_->GetConfigFromDevice();
    //qDebug()<<"done";
}
// write config to device
void MainWindow::on_pushButton_WriteConfig_clicked()
{
    qDebug()<<"Write config started";
    ui->pushButton_WriteConfig->setEnabled(false);
    ui->pushButton_ReadConfig->setEnabled(false);
//    index_device_before_write_ = ui->comboBox_HidDeviceList->currentIndex();

    WriteToConfig();
    hid_device_worker_->SendConfigToDevice();
    //qDebug()<<"done";
}
// load from file
void MainWindow::on_pushButton_LoadFromFile_clicked()
{
    qDebug()<<"Load from file started";
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Config"), QDir::currentPath() + "/", tr("Config Files (*.cfg)"));

    QSettings device_settings( fileName, QSettings::IniFormat );
    LoadDeviceConfigFromFile(&device_settings);
    qDebug()<<"done";
}
// save to file
void MainWindow::on_pushButton_SaveToFile_clicked()
{
    qDebug()<<"Save to file started";
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Config"), QDir::currentPath() + "/" + gEnv.pDeviceConfig->config.device_name, tr("Config Files (*.cfg)"));


    QSettings device_settings( fileName, QSettings::IniFormat );
    SaveDeviceConfigToFile(&device_settings);
    qDebug()<<"done";
}
//set default config
void MainWindow::on_pushButton_SetDefaultConfig_clicked()
{
    qDebug()<<"Set default config started";
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Config"), QDir::currentPath() + "/", tr("Config Files (*.cfg)"));

    gEnv.pAppSettings->beginGroup("DefaultConfig");
    gEnv.pAppSettings->setValue("FileName", fileName);
    gEnv.pAppSettings->endGroup();
    qDebug()<<"done";
}
// load default config file
void MainWindow::on_pushButton_LoadDefaultConfig_clicked()
{
    qDebug()<<"Load default config started";
    gEnv.pAppSettings->beginGroup("DefaultConfig");

    if (gEnv.pAppSettings->value("FileName", "none") != "none"){
        QSettings app_settings( gEnv.pAppSettings->value("FileName", "none").toString(), QSettings::IniFormat );
        LoadDeviceConfigFromFile(&app_settings);
    }
    QSettings app_settings( "FreeJoySettings.conf", QSettings::IniFormat );
    gEnv.pAppSettings->endGroup();
    qDebug()<<"Load default config finished";
}
// Show debug widget
void MainWindow::on_pushButton_ShowDebug_clicked()
{
    if (debug_window_ == nullptr)
    {
        debug_window_ = new DebugWindow(this);       // хз мб всё же стоит создать дебаг в конструкторе и оставить скрытым и не экономить пару кб
        gEnv.pDebugWindow = debug_window_;
        ui->layoutV_DebugWindow->addWidget(debug_window_);
        debug_window_->hide();
    }

    if (debug_window_->isVisible() == false)
    {
        debug_window_->setMinimumHeight(120);
        if (this->isMaximized() == false){
            resize(width(), height() + 120 + ui->layoutG_MainLayout->verticalSpacing());
        }
        debug_window_->setVisible(true);
        debug_is_enable_ = true;
        ui->pushButton_ShowDebug->setText(tr("Hide debug"));    // in retranslate
    } else {
        debug_window_->setVisible(false);
        debug_window_->setMinimumHeight(0);
        if (this->isMaximized() == false){
            resize(width(), height() - 120 - ui->layoutG_MainLayout->verticalSpacing());    // и в LoadAppConfig()
        }
        debug_is_enable_ = false;
        ui->pushButton_ShowDebug->setText(tr("Show debug"));
    }
}


// test button in debug tab
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
