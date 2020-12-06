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
    //setWindowFlags( Qt::FramelessWindowHint );

    // firmware version
    QString str = QString::number(FIRMWARE_VERSION, 16);
    if (str.size() >= 4){
        setWindowTitle(tr("FreeJoy Configurator") + " v" + str[0] + "." + str[1] + "." + str[2] + " (" + APP_VERSION + ")");// + "b" + str[3]);
    }

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

                                            //////////////// SIGNASL-SLOTS ////////////////
            ///////// GET / SEND     CONFIG ///////
    connect(this, SIGNAL(getConfigDone(bool)),
            this, SLOT(configReceived(bool)));
    connect(this, SIGNAL(sendConfigDone(bool)),
            this, SLOT(configSent(bool)));



            /////////      /////////////
    // buttons pin changed
    connect(m_pinConfig, SIGNAL(totalButtonsValueChanged(int)),
                m_buttonConfig, SLOT(setUiOnOff(int)));
    // LEDs changed
    connect(m_pinConfig, SIGNAL(totalLEDsValueChanged(int)),
                m_ledConfig, SLOT(spawnLeds(int)));
    // encoder changed
    connect(m_buttonConfig, SIGNAL(encoderInputChanged(int, int)),
            m_encoderConfig, SLOT(encoderInputChanged(int, int)));
    // fast encoder
    connect(m_pinConfig, SIGNAL(fastEncoderSelected(QString, bool)),
            m_encoderConfig, SLOT(fastEncoderSelected(QString, bool)));
    // shift registers
    connect(m_pinConfig, SIGNAL(shiftRegSelected(int, int, QString)),
            m_shiftRegConfig, SLOT(shiftRegSelected(int, int, QString)));
    // a2b count
    connect(m_axesConfig, SIGNAL(a2bCountChanged(int)),
            m_pinConfig, SLOT(a2bCountChanged(int)));
    // shift reg buttons count shiftRegsButtonsCount
    connect(m_shiftRegConfig, SIGNAL(shiftRegButtonsCountChanged(int)),
            m_pinConfig, SLOT(shiftRegButtonsCountChanged(int)));
    // axes source changed//axesSourceChanged
    connect(m_pinConfig, SIGNAL(axesSourceChanged(int, bool)),
            m_axesConfig, SLOT(addOrDeleteMainSource(int, bool)));
    // language changed
    connect(m_advSettings, SIGNAL(languageChanged(const QString &)),
            this, SLOT(languageChanged(const QString &)));
    // interface changed
    connect(m_advSettings, SIGNAL(interfaceStyleChanged(bool)),
            this, SLOT(interfaceStyleChanged(bool)));
    // font changed
    connect(m_advSettings, SIGNAL(fontChanged()),
            this, SLOT(setFont()));


    // enter flash mode clicked
    connect(m_advSettings->flasher(), SIGNAL(flashModeClicked(bool)),
            this, SLOT(deviceFlasherController(bool)));
    // flasher found
    connect(m_hidDeviceWorker, SIGNAL(flasherFound(bool)),
            m_advSettings->flasher(), SLOT(flasherFound(bool)));
    // start flash
    connect(m_advSettings->flasher(), SIGNAL(startFlash(bool)),              // thread  flashStatus
            this, SLOT(deviceFlasherController(bool)));
    // flash status
    connect(m_hidDeviceWorker, SIGNAL(flashStatus(int, int)),              // thread  flashStatus
            m_advSettings->flasher(), SLOT(flashStatus(int, int)));
    // set selected hid device
    connect(ui->comboBox_HidDeviceList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(hidDeviceListChanged(int)));


    // HID worker
    m_hidDeviceWorker->moveToThread(m_thread);

    connect(m_thread, SIGNAL(started()), m_hidDeviceWorker, SLOT(processData()));

    connect(m_hidDeviceWorker, SIGNAL(gamepadPacketReceived(uint8_t *)),
                          this, SLOT(getGamepadPacket(uint8_t *)));
    connect(m_hidDeviceWorker, SIGNAL(deviceConnected()),
                          this, SLOT(showConnectDeviceInfo()));
    connect(m_hidDeviceWorker, SIGNAL(deviceDisconnected()),
                          this, SLOT(hideConnectDeviceInfo()));
    connect(m_hidDeviceWorker, SIGNAL(flasherConnected()),
                          this, SLOT(flasherConnected()));
    connect(m_hidDeviceWorker, SIGNAL(hidDeviceList(const QStringList &)),
                          this, SLOT(hidDeviceList(const QStringList &)));


    // read config from device
    connect(m_hidDeviceWorker, SIGNAL(configReceived(bool)),
            this, SLOT(configReceived(bool)));
    // write config to device
    connect(m_hidDeviceWorker, SIGNAL(configSent(bool)),
            this, SLOT(configSent(bool)));


    // load default config // loading will occur after loading buttons config
    // комбобоксы у кнопок заполняются после старта приложения и конфиг должен
    // запускаться сигналом от кнопок
    connect(m_buttonConfig, SIGNAL(logicalButtonsCreated()),
            this, SLOT(loadDefaultConfig()));


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
    m_thread->quit();
    m_thread->deleteLater();
    m_thread->wait();
    m_threadGetSendConfig->quit();
    m_threadGetSendConfig->deleteLater();
    m_threadGetSendConfig->wait();
    delete m_hidDeviceWorker;
    delete m_thread;              // не уверен в нужности, если есть thread->deleteLater();
    delete m_threadGetSendConfig;
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
    m_advSettings->flasher()->deviceConnected(true);
}
// device disconnected
void MainWindow::hideConnectDeviceInfo() {
    //qDebug()<<"Device disconected";
    ui->label_DeviceStatus->setText(tr("Disconnected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: rgb(160, 0, 0);");
    ui->pushButton_ReadConfig->setEnabled(false);
    ui->pushButton_WriteConfig->setEnabled(false);
    m_advSettings->flasher()->deviceConnected(false);
    if (m_debugWindow){
        m_debugWindow->resetPacketsCount();
    }

    // disable curve point
    QTimer::singleShot(3000, [&]{   // не лучший способ
        if (ui->pushButton_ReadConfig->isEnabled() == false){
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
    if (m_debugWindow){
        m_debugWindow->resetPacketsCount();
    }
    if (ui->pushButton_ReadConfig->isEnabled() == false){
        m_axesCurvesConfig->deviceStatus(false);
    }
}

// add/delete hid devices to/from combobox
void MainWindow::hidDeviceList(const QStringList &deviceList)
{
//    QSignalBlocker blocker(ui->comboBox_HidDeviceList);
//    int curIndex = ui->comboBox_HidDeviceList->currentIndex();
    if (deviceList.size() == 0) {
        ui->comboBox_HidDeviceList->clear();
        return;
    } else {
        ui->comboBox_HidDeviceList->clear();
        ui->comboBox_HidDeviceList->addItems(deviceList);
    }
//    blocker.unblock();
//    if (curIndex >=0) {
//        ui->comboBox_HidDeviceList->setCurrentIndex(curIndex);
//    }
}

// received device report
void MainWindow::getGamepadPacket(uint8_t *buff)
{
    reportConvert.gamepadReport(buff);
    // update button state without delay. fix gamepad_report.raw_button_data[0]
    // из-за задержки может не ловить изменения первых физических 64 кнопок или оставшихся.
    // Например, может подряд попасться gamepad_report.raw_button_data[0] = 0
    // и не видеть оставшиеся физические 64 кнопки. ленивый и неоптимизированный фикс
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
    static int asd = 0;
    ui->lineEdit->setText(QString::number(++asd));
#endif
}

// Flasher controller
void MainWindow::deviceFlasherController(bool isStartFlash)
{        // херня? mb QtConcurrent::run()
    // так оставить или как read/write а bool через сигнал?
    QEventLoop loop;
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




                                            /////////////////////                 /////////////////////

// slot interface style changed
void MainWindow::interfaceStyleChanged(bool isDark)
{
    m_axesCurvesConfig->setDarkInterface(isDark);
}

// slot language change
void MainWindow::languageChanged(const QString &language)
{
    qDebug()<<"Retranslate UI";
    if (language == "russian")
    {
        m_translator.load(":/FreeJoyQt_ru");// + QString("ru_RU"));//QLocale::system().name();//QString("ru_RU"));//QLocale::name());
        qApp->installTranslator(&m_translator);
        ui->retranslateUi(this);
    }
    else if (language == "english")
    {
        m_translator.load(":/FreeJoyQt_en");
        qApp->installTranslator(&m_translator);
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
    for (QWidget *widget : QApplication::allWidgets()) {
        widget->setFont(QApplication::font());
        widget->update();
    }
}

// load app config
void MainWindow::loadAppConfig()
{
    QSettings *appS = gEnv.pAppSettings;
    qDebug()<<"Loading application config";
    // load language settings
    appS->beginGroup("LanguageSettings");
    if (appS->value("Language", "english").toString() == "russian")
    {
        m_translator.load(":/FreeJoyQt_ru");
        qApp->installTranslator(&m_translator);
        ui->retranslateUi(this);
    }
    appS->endGroup();

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
void MainWindow::saveAppConfig()    // перенести сюда сохранение профилей кривых
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
