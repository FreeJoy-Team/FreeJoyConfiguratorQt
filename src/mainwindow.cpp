#include <QThread>
#include <QTimer>
#include <QSettings>
#include <QFileDialog>
#include <QDesktopServices>
#include <ctime>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mousewheelguard.h"
#include <QSpinBox>

#include "common_types.h"
#include "global.h"
#include "deviceconfig.h"
#include "version.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
//    , m_thread{new QThread}
//    , m_hidDeviceWorker{new HidDevice()}
//    , m_threadGetSendConfig{new QThread}
//    , m_pinConfig{new PinConfig(this)}// not need delete. this - parent
//    , m_buttonConfig{new ButtonConfig(this)}
//    , m_ledConfig{new LedConfig(this)}
//    , m_encoderConfig{new EncodersConfig(this)}
//    , m_shiftRegConfig{new ShiftRegistersConfig(this)}
//    , m_axesConfig{new AxesConfig(this)}
//    , m_axesCurvesConfig{new AxesCurvesConfig(this)}
//    , m_advSettings{new AdvancedSettings(this)}
{
    qDebug()<<"main + member initialize time ="<< gEnv.pApp_start_time->elapsed() << "ms";
    QElapsedTimer timer;
    timer.start();

    ui->setupUi(this);

    QMainWindow::setWindowIcon(QIcon(":/Images/icon-32.png"));

    // firmware version
    setWindowTitle(tr("FreeJoy Configurator") + " v" + APP_VERSION);

    // load application config
    loadAppConfig();

    m_thread = new QThread;
    m_hidDeviceWorker = new HidDevice();
    m_threadGetSendConfig = new QThread;

    qDebug()<<"before add widgets ="<< timer.restart() << "ms";
                                            //////////////// ADD WIDGETS ////////////////
    // add pin widget
    m_pinConfig = new PinConfig(this);
    ui->layoutV_tabPinConfig->addWidget(m_pinConfig);
    qDebug()<<"pin config load time ="<< timer.restart() << "ms";
    // add button widget
    m_buttonConfig = new ButtonConfig(this);
    ui->layoutV_tabButtonConfig->addWidget(m_buttonConfig);
    qDebug()<<"button config load time ="<< timer.restart() << "ms";
    // add axes widget
    m_axesConfig = new AxesConfig(this);
    ui->layoutV_tabAxesConfig->addWidget(m_axesConfig);
    qDebug()<<"axes config load time ="<< timer.restart() << "ms";
    // add axes curves widget
    m_axesCurvesConfig = new AxesCurvesConfig(this);
    ui->layoutV_tabAxesCurvesConfig->addWidget(m_axesCurvesConfig);
    qDebug()<<"curves config load time ="<< timer.restart() << "ms";
    // add shift registers widget
    m_shiftRegConfig = new ShiftRegistersConfig(this);
    ui->layoutV_tabShiftRegistersConfig->addWidget(m_shiftRegConfig);
    qDebug()<<"shift config load time ="<< timer.restart() << "ms";
    // add encoders widget
    m_encoderConfig = new EncodersConfig(this);
    ui->layoutV_tabEncodersConfig->addWidget(m_encoderConfig);
    qDebug()<<"encoder config load time ="<< timer.restart() << "ms";
    // add led widget
    m_ledConfig = new LedConfig(this);
    ui->layoutV_tabLedConfig->addWidget(m_ledConfig);
    qDebug()<<"led config load time ="<< timer.restart() << "ms";
    // add advanced settings widget
    m_advSettings = new AdvancedSettings(this);
    ui->layoutV_tabAdvSettings->addWidget(m_advSettings);
    qDebug()<<"advanced settings load time ="<< timer.restart() << "ms";


    // strong focus for mouse wheel
    // без протекта можно при прокручивании страницы случайно навести на комбобокс и изменить его колесом мыши
    // при установке setFocusPolicy(Qt::StrongFocus) и протекта на комбобокс придётся нажать, для прокручивания колесом
    for (auto &&child: this->findChildren<QSpinBox *>())
    {
        child->setFocusPolicy(Qt::StrongFocus);
        child->installEventFilter(new MouseWheelGuard(child));
    }

    for (auto &&child: this->findChildren<QComboBox *>())
    {
        child->setFocusPolicy(Qt::StrongFocus);
        child->installEventFilter(new MouseWheelGuard(child));
    }
    // хз так или сверху исключать?
    ui->comboBox_HidDeviceList->setFocusPolicy(Qt::WheelFocus);
    for (auto &&comBox: m_pinConfig->findChildren<QComboBox *>())
    {
            comBox->setFocusPolicy(Qt::WheelFocus);
    }
    for (auto &&comBox: m_axesCurvesConfig->findChildren<QComboBox *>())
    {
        comBox->setFocusPolicy(Qt::WheelFocus);
    }
    for (auto &&comBox: m_ledConfig->findChildren<QComboBox *>())
    {
        comBox->setFocusPolicy(Qt::WheelFocus);
    }

                                            //////////////// SIGNASL-SLOTS ////////////////
    // get/send config
    connect(this, &MainWindow::getConfigDone, this, &MainWindow::configReceived);
    connect(this, &MainWindow::sendConfigDone, this, &MainWindow::configSent);


    // buttons pin changed
    connect(m_pinConfig, &PinConfig::totalButtonsValueChanged, m_buttonConfig, &ButtonConfig::setUiOnOff);
    // LEDs changed
    connect(m_pinConfig, &PinConfig::totalLEDsValueChanged, m_ledConfig, &LedConfig::spawnLeds);
    // encoder changed
    connect(m_buttonConfig, &ButtonConfig::encoderInputChanged, m_encoderConfig, &EncodersConfig::encoderInputChanged);
    // fast encoder
    connect(m_pinConfig, &PinConfig::fastEncoderSelected, m_encoderConfig, &EncodersConfig::fastEncoderSelected);
    // shift registers
    connect(m_pinConfig, &PinConfig::shiftRegSelected, m_shiftRegConfig, &ShiftRegistersConfig::shiftRegSelected);
    // a2b count
    connect(m_axesConfig, &AxesConfig::a2bCountChanged, m_pinConfig, &PinConfig::a2bCountChanged);
    // shift reg buttons count shiftRegsButtonsCount
    connect(m_shiftRegConfig, &ShiftRegistersConfig::shiftRegButtonsCountChanged,
            m_pinConfig, &PinConfig::shiftRegButtonsCountChanged);
    // axes source changed//axesSourceChanged
    connect(m_pinConfig, &PinConfig::axesSourceChanged, m_axesConfig, &AxesConfig::addOrDeleteMainSource);
    // language changed
    connect(m_advSettings, &AdvancedSettings::languageChanged, this, &MainWindow::languageChanged);
    // interface changed
    connect(m_advSettings, &AdvancedSettings::interfaceStyleChanged, this, &MainWindow::interfaceStyleChanged);
    // font changed
    connect(m_advSettings, &AdvancedSettings::fontChanged, this, &MainWindow::setFont);
    // style switch
    connect(ui->widget_StyleSwitch, &SwitchButton::currentIndexChanged, this, &MainWindow::styleSwitched);


    // enter flash mode clicked
    connect(m_advSettings->flasher(), &Flasher::flashModeClicked, this, &MainWindow::deviceFlasherController);
    // flasher found
    connect(m_hidDeviceWorker, &HidDevice::flasherFound, m_advSettings->flasher(), &Flasher::flasherFound);
    // start flash
    connect(m_advSettings->flasher(), &Flasher::startFlash, this, &MainWindow::deviceFlasherController);
    // flash status
    connect(m_hidDeviceWorker, &HidDevice::flashStatus, m_advSettings->flasher(), &Flasher::flashStatus);
    // set selected hid device
    connect(ui->comboBox_HidDeviceList, SIGNAL(currentIndexChanged(int)),
                this, SLOT(hidDeviceListChanged(int)));


    // HID worker
    m_hidDeviceWorker->moveToThread(m_thread);
    connect(m_thread, &QThread::started, m_hidDeviceWorker, &HidDevice::processData);

    connect(m_hidDeviceWorker, &HidDevice::paramsPacketReceived, this, &MainWindow::getParamsPacket);
    connect(m_hidDeviceWorker, &HidDevice::deviceConnected, this, &MainWindow::showConnectDeviceInfo);
    connect(m_hidDeviceWorker, &HidDevice::deviceDisconnected, this, &MainWindow::hideConnectDeviceInfo);
    connect(m_hidDeviceWorker, &HidDevice::flasherConnected, this, &MainWindow::flasherConnected);
    connect(m_hidDeviceWorker, &HidDevice::hidDeviceList, this, &MainWindow::hidDeviceList);


    // read config from device
    connect(m_hidDeviceWorker, &HidDevice::configReceived, this, &MainWindow::configReceived);
    // write config to device
    connect(m_hidDeviceWorker, &HidDevice::configSent, this, &MainWindow::configSent);


    // load default config // loading will occur after loading buttons config
    // комбобоксы у кнопок заполняются после старта приложения и конфиг должен
    // запускаться сигналом от кнопок
    connect(m_buttonConfig, &ButtonConfig::logicalButtonsCreated, this, &MainWindow::loadDefaultConfig);


    // set style for some widgets
    gEnv.pAppSettings->beginGroup("StyleSettings");
    if (gEnv.pAppSettings->value("StyleSheet", "default").toString() == "dark"){
        interfaceStyleChanged(true);
    } else {
        interfaceStyleChanged(false);
    }
    gEnv.pAppSettings->endGroup();


    m_thread->start();

    qDebug()<<"after widgets load time ="<< timer.elapsed() << "ms";
    qDebug()<<"without style startup time ="<< gEnv.pApp_start_time->elapsed() << "ms";
}

MainWindow::~MainWindow()
{
    saveAppConfig();
    m_hidDeviceWorker->setIsFinish(true);
    m_hidDeviceWorker->deleteLater();
    m_thread->quit();
    m_thread->deleteLater();
    m_thread->wait();
    m_threadGetSendConfig->quit();
    m_threadGetSendConfig->deleteLater();
    m_threadGetSendConfig->wait();
    delete ui;
}




                                                        ///////////////////// device reports /////////////////////
// device connected
void MainWindow::showConnectDeviceInfo()
{
    if (ui->comboBox_HidDeviceList->count() > 1) {
        ui->label_DeviceStatus->setText(QString::number(ui->comboBox_HidDeviceList->count()) + " " + tr("devices connected"));
    } else {
        ui->label_DeviceStatus->setText(QString::number(ui->comboBox_HidDeviceList->count()) + " " + tr("device connected"));
    }
    if (ui->comboBox_HidDeviceList->itemData(ui->comboBox_HidDeviceList->currentIndex()).toInt() != 1) {
        ui->pushButton_WriteConfig->setEnabled(true);
        ui->pushButton_ReadConfig->setEnabled(true);
        ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");
    } else {
        // for old firmware
        ui->pushButton_WriteConfig->setEnabled(false);
        ui->pushButton_ReadConfig->setEnabled(false);
        ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(168, 168, 0);");
    }
    m_advSettings->flasher()->deviceConnected(true);
}

// device disconnected
void MainWindow::hideConnectDeviceInfo()
{
    ui->label_DeviceStatus->setText(tr("Disconnected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(160, 0, 0);");
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);
    m_advSettings->flasher()->deviceConnected(false);
//    // reset paramsReport
//    memset(&gEnv.pDeviceConfig->paramsReport, 0, sizeof(gEnv.pDeviceConfig->paramsReport));
//    uint8_t tmp[64]{};        // todo
//    getParamsPacket(tmp);
    // debug window
    if (m_debugWindow) {
        m_debugWindow->resetPacketsCount();
    }
    // disable curve point
    QTimer::singleShot(3000, this, [&] {   // не лучший способ
        if (ui->pushButton_ReadConfig->isEnabled() == false) {
            m_axesCurvesConfig->deviceStatus(false);
        }
    });
}

// flasher connected
void MainWindow::flasherConnected()
{
    ui->label_DeviceStatus->setText(tr("Connected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);
    m_advSettings->flasher()->deviceConnected(false);
    if (m_debugWindow) {
        m_debugWindow->resetPacketsCount();
    }
    if (ui->pushButton_ReadConfig->isEnabled() == false) {
        m_axesCurvesConfig->deviceStatus(false);
    }
}

// add/delete hid devices to/from combobox
void MainWindow::hidDeviceList(const QList<QPair<bool, QString>> &deviceNames)
{
    if (deviceNames.size() == 0) {
        ui->comboBox_HidDeviceList->clear();
        return;
    } else {
        ui->comboBox_HidDeviceList->clear();
        for (int i = 0; i < deviceNames.size(); ++i) {
            if (deviceNames[i].first) {
                // for old firmware
                ui->comboBox_HidDeviceList->addItem("ONLY FLASH " + deviceNames[i].second, 1);
            } else {
                ui->comboBox_HidDeviceList->addItem(deviceNames[i].second);
            }
        }
    }
}

// received device report
void MainWindow::getParamsPacket(uint8_t *buffer)
{
    ReportConverter::paramReport(buffer);
    // update button state without delay. fix gamepad_report.raw_button_data[0]
    // из-за задержки может не ловить изменения первых физических 64 кнопок или оставшихся.
    // Например, может подряд попасться gamepad_report.raw_button_data[0] = 0
    // и не видеть оставшиеся физические 64 кнопки.
    if(ui->tab_ButtonConfig->isVisible() == true || m_debugWindow){
        m_buttonConfig->buttonStateChanged();
    }

    static QElapsedTimer timer;
    static bool change = false;

    if (!change)
    {
        timer.start();
        change = true;
    }
    else if (timer.elapsed() > 17)    // обновление раз в 17мс(~60fps), мб сделать дефайн в герцах
    {
        // optimization
        if(ui->tab_LED->isVisible() == true){
            m_ledConfig->setLedsState();
        }
        if(ui->tab_AxesConfig->isVisible() == true){
            m_axesConfig->axesValueChanged();
        }
        if(ui->tab_AxesCurves->isVisible() == true){
            m_axesCurvesConfig->updateAxesCurves();
        }
        change = false;
    }
    // debug info
    if (m_debugWindow){
        m_debugWindow->devicePacketReceived();
    }

    // debug tab
#ifdef QT_DEBUG

#endif
}

// Flasher controller
void MainWindow::deviceFlasherController(bool isStartFlash)
{        // херня? mb QtConcurrent::run()
    // так оставить или как read/write а bool через сигнал?
    QEventLoop loop; // static?
    QObject context;
    context.moveToThread(m_threadGetSendConfig);
    connect(m_threadGetSendConfig, &QThread::started, &context, [&]() {
        qDebug()<<"Start flasher controller";
        if (isStartFlash == true){
            qDebug()<<"Start flash";
            m_hidDeviceWorker->flashFirmware(m_advSettings->flasher()->fileArray());
        } else {
            qDebug()<<"Enter to flash mode";
            m_hidDeviceWorker->enterToFlashMode();
        }
        qDebug()<<"Flasher controller finished";
        loop.quit();
    });
    m_threadGetSendConfig->start();
    loop.exec();
    m_threadGetSendConfig->quit();
    m_threadGetSendConfig->wait();
}




                                            /////////////////////    APP SETTINGS   /////////////////////

// slot interface style changed
void MainWindow::interfaceStyleChanged(bool isDark)
{
    m_axesCurvesConfig->setDarkInterface(isDark);
    if (isDark == false) {
        setDefaultStyleSheet();
    }
}

// we need set default styleSheet only for a small amount widgets
// if i use w.setStyleSheet(ts.readAll()); load time increases by 500ms!
// this function set styleSheet only for the necessary widgets
void MainWindow::setDefaultStyleSheet()
{
    ui->pushButton_Wiki->setStyleSheet(
    "QPushButton#pushButton_Wiki {"
        "border: 1px solid;"
        "border-color: rgb(173, 173, 173);"
        "border-radius:15px;"
        "padding:0px;"
        "margin: 0px;"
        "min-width: 30;"
        "max-width: 30;"
        "max-height: 30;"
        "min-height: 30;"
        "width: 30;"
        "height: 30;"
    "}"
    "QPushButton#pushButton_Wiki:hover {"
        "border: 1px solid;"
        "border-color: rgb(0, 120, 215);"
        "background-color: rgb(229, 241, 251);"
        "border-radius:15px;"
    "}");
}

// slot language change
void MainWindow::languageChanged(const QString &language)
{
    qDebug()<<"Retranslate UI";
    bool ok;
    if (language == "russian")
    {
        ok = gEnv.pTranslator->load(":/FreeJoyQt_ru");// + QString("ru_RU"));//QLocale::system().name();//QString("ru_RU"));//QLocale::name());
        if (ok == false) {
            qCritical()<<"failed to load translate file";
            return;
        }
        qApp->installTranslator(gEnv.pTranslator);
        ui->retranslateUi(this);
    }
    else if (language == "english")
    {
        ok = gEnv.pTranslator->load(":/NO_FILE_IS_OK!!_DEFAULT_TRANSLATE");
        if (ok == true) {
            qCritical()<<"failed to load translate file";
            return;
        }
        qApp->installTranslator(gEnv.pTranslator);
        ui->retranslateUi(this);
    }
    else if (language == "schinese")
    {
        ok = gEnv.pTranslator->load(":/FreeJoyQt_zh_CN");
        if (ok == false) {
            qCritical()<<"failed to load translate file";
            return;
        }
        qApp->installTranslator(gEnv.pTranslator);
        ui->retranslateUi(this);
    } else {
        return;
    }
    m_pinConfig->retranslateUi();
    m_buttonConfig->retranslateUi();
    m_ledConfig->retranslateUi();
    m_encoderConfig->retranslateUi();
    m_shiftRegConfig->retranslateUi();
    m_axesConfig->retranslateUi();
    m_axesCurvesConfig->retranslateUi();
    m_advSettings->retranslateUi();
    if(m_debugWindow){
        m_debugWindow->retranslateUi();
        ui->pushButton_ShowDebug->setText(tr("Hide debug"));
    }
    qDebug()<<"done";
}

// set font
void MainWindow::setFont()
{
    QWidgetList list = QApplication::allWidgets();
    for (QWidget *widget : list) {
        widget->setFont(QApplication::font());
        widget->update();
    }
}

// load app config
void MainWindow::loadAppConfig()
{
    QSettings *appS = gEnv.pAppSettings;
    qDebug()<<"Loading application config";

    // load window settings
    appS->beginGroup("WindowSettings");
    this->restoreGeometry(appS->value("Geometry").toByteArray());
    appS->endGroup();

    // load tab index
    appS->beginGroup("TabIndexSettings");
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_PinConfig),
                                      appS->value("PinConfig", ui->tabWidget->indexOf(ui->tab_PinConfig)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_ButtonConfig),
                                      appS->value("ButtonConfig", ui->tabWidget->indexOf(ui->tab_ButtonConfig)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_AxesConfig),
                                      appS->value("AxesConfig", ui->tabWidget->indexOf(ui->tab_AxesConfig)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_AxesCurves),
                                      appS->value("AxesCurves", ui->tabWidget->indexOf(ui->tab_AxesCurves)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_ShiftRegisters),
                                      appS->value("ShiftRegs", ui->tabWidget->indexOf(ui->tab_ShiftRegisters)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_Encoders),
                                      appS->value("Encoders", ui->tabWidget->indexOf(ui->tab_Encoders)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_LED),
                                      appS->value("LED", ui->tabWidget->indexOf(ui->tab_LED)).toInt());
    ui->tabWidget->tabBar()->moveTab (ui->tabWidget->indexOf(ui->tab_AdvancedSettings),
                                      appS->value("AdvSettings", ui->tabWidget->indexOf(ui->tab_AdvancedSettings)).toInt());
    ui->tabWidget->setCurrentIndex(appS->value("CurrentIndex", 0).toInt());
    appS->endGroup();

    // load debug window
    appS->beginGroup("OtherSettings");
    m_debugIsEnable = appS->value("DebugEnable", "false").toBool();
    if (m_debugIsEnable){
        on_pushButton_ShowDebug_clicked();
        if (this->isMaximized() == false){
            resize(width(), height() - 120 - ui->layoutG_MainLayout->verticalSpacing());
        }
    }
    appS->endGroup();

    //debug tab, only for debug build
    #ifdef QT_DEBUG
    #else
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tab_Debug));
    #endif
    qDebug()<<"Finished loading application config";
}

// save app config
void MainWindow::saveAppConfig()
{
    QSettings *appS = gEnv.pAppSettings;
    qDebug()<<"Saving application config";
    // save tab index
    appS->beginGroup("TabIndexSettings");
    appS->setValue("PinConfig",       ui->tabWidget->indexOf(ui->tab_PinConfig));
    appS->setValue("ButtonConfig",    ui->tabWidget->indexOf(ui->tab_ButtonConfig));
    appS->setValue("AxesConfig",      ui->tabWidget->indexOf(ui->tab_AxesConfig));
    appS->setValue("AxesCurves",      ui->tabWidget->indexOf(ui->tab_AxesCurves));
    appS->setValue("ShiftRegs",       ui->tabWidget->indexOf(ui->tab_ShiftRegisters));
    appS->setValue("Encoders",        ui->tabWidget->indexOf(ui->tab_Encoders));
    appS->setValue("LED",             ui->tabWidget->indexOf(ui->tab_LED));
    appS->setValue("AdvSettings",     ui->tabWidget->indexOf(ui->tab_AdvancedSettings));
    appS->setValue("CurrentIndex",    ui->tabWidget->currentIndex());
    appS->endGroup();
    // save window settings
    appS->beginGroup("WindowSettings");
    appS->setValue("Geometry",   this->saveGeometry());
    appS->endGroup();
    // save font settings
    appS->beginGroup("FontSettings");
    appS->setValue("FontSize", QApplication::font().pointSize());
    appS->endGroup();
    // save debug
    appS->beginGroup("OtherSettings");
    appS->setValue("DebugEnable", m_debugIsEnable);
    appS->endGroup();
    qDebug()<<"done";
}


                                                            ////////////////// buttons //////////////////
// comboBox selected device
void MainWindow::hidDeviceListChanged(int index)
{
    m_hidDeviceWorker->setSelectedDevice(index);
}
// reset all pins
void MainWindow::on_pushButton_ResetAllPins_clicked()
{
    qDebug()<<"Reset all started";
    gEnv.pDeviceConfig->resetConfig();

    readFromConfig();

    m_pinConfig->resetAllPins();
    qDebug()<<"done";
}
// read config from device
void MainWindow::on_pushButton_ReadConfig_clicked()
{
    qDebug()<<"Read config started";
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);

    m_hidDeviceWorker->getConfigFromDevice();
}
// write config to device
void MainWindow::on_pushButton_WriteConfig_clicked()
{
    qDebug()<<"Write config started";
    ui->pushButton_WriteConfig->setEnabled(false);  // не успевает блокироваться? таймер
    ui->pushButton_ReadConfig->setEnabled(false);

    writeToConfig();
    m_hidDeviceWorker->sendConfigToDevice();
}
// load from file
void MainWindow::on_pushButton_LoadFromFile_clicked()
{
    qDebug()<<"Load from file started";
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Config"), QDir::currentPath() + "/", tr("Config Files (*.cfg)"));

    QSettings device_settings( fileName, QSettings::IniFormat );
    loadDeviceConfigFromFile(&device_settings);
    qDebug()<<"done";
}
// save to file
void MainWindow::on_pushButton_SaveToFile_clicked()
{
    qDebug()<<"Save to file started";
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Config"), QDir::currentPath() + "/" + gEnv.pDeviceConfig->config.device_name, tr("Config Files (*.cfg)"));


    QSettings device_settings( fileName, QSettings::IniFormat );
    saveDeviceConfigToFile(&device_settings);
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
        loadDeviceConfigFromFile(&app_settings);
    }
    QSettings app_settings( "FreeJoySettings.conf", QSettings::IniFormat );
    gEnv.pAppSettings->endGroup();
    qDebug()<<"Load default config finished";
}
// Show debug widget
void MainWindow::on_pushButton_ShowDebug_clicked()
{
    if (m_debugWindow == nullptr)
    {
        m_debugWindow = new DebugWindow(this);       // хз мб всё же стоит создать дебаг в конструкторе и оставить скрытым и не экономить пару кб
        gEnv.pDebugWindow = m_debugWindow;
        ui->layoutV_DebugWindow->addWidget(m_debugWindow);
        m_debugWindow->hide();
    }

    if (m_debugWindow->isVisible() == false)
    {
        m_debugWindow->setMinimumHeight(120);
        if (this->isMaximized() == false){
            resize(width(), height() + 120 + ui->layoutG_MainLayout->verticalSpacing());
        }
        m_debugWindow->setVisible(true);
        m_debugIsEnable = true;
        ui->pushButton_ShowDebug->setText(tr("Hide debug"));    // in MainWindow::languageChanged
    } else {
        m_debugWindow->setVisible(false);
        m_debugWindow->setMinimumHeight(0);
        if (this->isMaximized() == false){
            resize(width(), height() - 120 - ui->layoutG_MainLayout->verticalSpacing());    // and in LoadAppConfig()
        }
        m_debugIsEnable = false;
        ui->pushButton_ShowDebug->setText(tr("Show debug"));
    }
}
// Wiki
void MainWindow::on_pushButton_Wiki_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/FreeJoy-Team/FreeJoyWiki"));
}




////////////////////////////////////////////////// debug tab //////////////////////////////////////////////////
// style
#include <QCheckBox>
void MainWindow::styleSwitched(int index)
{
    setCursor(Qt::WaitCursor);
    QString path;
    if (index == 0) {
        path = ":/styles/default.qss";
    } else if (index == 1) {
        path = ":qss/qss.qss";
    } else {
        path = ":qdarkstyle/style.qss";
    }

    QFile f(path);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Unable to set stylesheet, file not found\n";
    } else {
        QElapsedTimer timer;
        timer.start();

        for (auto &&child : window()->findChildren<QComboBox *>()) {
            child->hide();
        }
        for (auto &&child : window()->findChildren<QCheckBox *>()) {
            child->hide();
        }
        for (auto &&child : window()->findChildren<QSpinBox *>()) {
            child->hide();
        }

        window()->setStyleSheet(QLatin1String(f.readAll()));
        qApp->setStyleSheet(QLatin1String(""));

        for (auto &&child : window()->findChildren<QComboBox *>()) {
            child->show();
        }
        for (auto &&child : window()->findChildren<QCheckBox *>()) {
            child->show();
        }
        for (auto &&child : window()->findChildren<QSpinBox *>()) {
            child->show();
        }

        qDebug() << "Style changed in" <<timer.elapsed() <<"ms";
        f.close();
    }
    unsetCursor();
}

// test buttons in debug tab
#ifdef QT_DEBUG
dev_config_t testCfg;
#endif
void MainWindow::on_pushButton_TestButton_clicked()
{
#ifdef QT_DEBUG
    on_pushButton_LoadFromFile_clicked();
    testCfg = gEnv.pDeviceConfig->config;
    on_pushButton_WriteConfig_clicked();
#endif
}


void MainWindow::on_pushButton_TestButton_2_clicked()
{
#ifdef QT_DEBUG
    on_pushButton_ReadConfig_clicked();
    QElapsedTimer timer;
    timer.start();
    while (3000 > timer.elapsed()) {
        if (gEnv.readFinished) {
            qDebug()<<"compare before write to file";
            int cmp = memcmp(&testCfg, &gEnv.pDeviceConfig->config, sizeof(dev_config_t));
            if (cmp == 0) {
                qDebug()<<"equal";
            } else {
                qDebug()<<"ERROR. NOT EQUAL! memcmp ="<< cmp;
                return;
            }
            on_pushButton_SaveToFile_clicked();
            on_pushButton_LoadFromFile_clicked();
            qDebug()<<"final compare";
            cmp = memcmp(&testCfg, &gEnv.pDeviceConfig->config, sizeof(dev_config_t));
            if (cmp == 0) {
                qDebug()<<"equal";
            } else {
                qDebug()<<"ERROR. NOT EQUAL! memcmp ="<< cmp;
            }
            break;
        }
        QThread::msleep(50);
    }
#endif
}
