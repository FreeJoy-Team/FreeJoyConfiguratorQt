//#include <QGridLayout>      //?
#include <QDebug>
#include <QThread>

//#include <QPixmap>          //?
//#include <QScopedPointer>   //?
#include <ctime>
#include <QTimer>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common_types.h"


#include "global.h"
//GlobalEnvironment gEnv;
// отсортировать include

//#include "reportconverter.h"
//#include "hiddevice.h"
//#include <memory>
//GLOBAL;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // сделать версию
    this->setWindowTitle("FreeJoy QtConfigurator");
//    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);


    hid_device_worker = new HidDevice();
    thread = new QThread;

    thread_getSend_config = new QThread;


    // load language settings
    gEnv.pAppSettings->beginGroup("LanguageSettings");
    if (gEnv.pAppSettings->value("Language", "english").toString() == "russian")
    {
        translator.load(":/FreeJoyTest_ru");
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
    }
    gEnv.pAppSettings->endGroup();




//    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
//            ui->widget_2, &ButtonLogical::Initialization);
//    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
//            ui->widget_2, &AdvancedSettings::WriteToConfig);

    // add dyn
//    QGridLayout* layout = new QGridLayout;
//    Encoders* qwe = new Encoders;
//    layout->addWidget(qwe);
//    ui->tab_Encoders->setLayout(layout);

    // dynamic widgets spawn
//    QScrollBar* scroll = ui->scrollArea_2->verticalScrollBar();
//    connect( scroll , SIGNAL(valueChanged(int)) ,this , SLOT(addvalues(int)) );

    //    /// needs to be deleted by you
    //    m_HeapWidgetWithoutParent = new QWidget();
    //    //doesn't need to be deleted by you
    //    m_HeapWidgetWithParent = new QWidget(this);
    //    // needs to be deleted by you
    //    m_HeapNoQObj = new NoQObjectDerivedClass();


                                            //////////////// ADD WIDGETS ////////////////
    // add pin config
    pin_config = new PinConfig(this);
    ui->layoutV_tabPinConfig->addWidget(pin_config);
    // add button config
    button_config = new ButtonConfig(this);     // not need delete. this - parent
    ui->layoutV_tabButtonConfig->addWidget(button_config);
    // add axes config
    axes_config = new AxesConfig(this);
    ui->layoutV_tabAxesConfig->addWidget(axes_config);
    // add axes curves config
    axes_curves_config = new AxesCurvesConfig(this);
    ui->layoutV_tabAxesCurvesConfig->addWidget(axes_curves_config);
    // add axes to buttons config
    a2b_config = new AxesToButtonsConfig(this);
    ui->layoutV_tabAxesToButtons->addWidget(a2b_config);
    // add shift registers config
    shift_reg_config = new ShiftRegistersConfig(this);
    ui->layoutV_tabShiftRegistersConfig->addWidget(shift_reg_config);
    // add encoders config
    encoder_config = new EncodersConfig(this);
    ui->layoutV_tabEncodersConfig->addWidget(encoder_config);
    // add led config
    led_config = new LedConfig(this);
    ui->layoutV_tabLedConfig->addWidget(led_config);      


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


    // read pin config
    pin_config->ReadFromConfig();
    // read axes config
    axes_config->ReadFromConfig();
    // read axes curves config
    axes_curves_config->ReadFromConfig();
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


    thread->start();
}

MainWindow::~MainWindow()
{
    hid_device_worker->SetIsFinish(true);
    thread->quit();
    thread->deleteLater();
    thread->wait();
    delete hid_device_worker;
    delete thread;              // не уверен в нужности, если есть thread->deleteLater();
    delete thread_getSend_config;       // hz
    delete ui;
}


void MainWindow::showConnectDeviceInfo() {
    ui->label_DeviceStatus->setText(tr("Connected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: green;");
    //ui->label_DeviceStatus->setVisible(true);
}


void MainWindow::hideConnectDeviceInfo() {
    //ui->label_DeviceStatus->setVisible(false);
    ui->label_DeviceStatus->setText(tr("Disconnected"));
    ui->label_DeviceStatus->setStyleSheet("color: white; background-color: red;");
}

void MainWindow::languageChanged(QString language)        // QSignalBlocker blocker(ui->comboBox);
{
    if (language == "russian")
    {
        translator.load(":/FreeJoyTest_ru");// + QString("ru_RU"));//QLocale::system().name();//QString("ru_RU"));//QLocale::name());
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
    }
    else if (language == "english")
    {
        translator.load(":/FreeJoyTest_en");
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
        change = false;
    }

    asd++;
}


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


void MainWindow::on_pushButton_11_clicked()
{
//   // set text in centr
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
}


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


#include <QFile>
#include <QTextStream>
void MainWindow::on_pushButton_QssDefault_clicked()
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
}

void MainWindow::on_pushButton_QssChinaWhite_clicked()
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
}

void MainWindow::on_pushButton_QssUbuntu_clicked()
{
    QFile f(":/QSS-master/Ubuntu.qss");
    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}

void MainWindow::on_pushButton_QssQDarkStyle_clicked()
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
}

void MainWindow::on_pushButton_QssBreezeDark_clicked()
{
    QFile f(":/dark.qss");
    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
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

// reset all pins
void MainWindow::on_pushButton_ResetAllPins_clicked()
{
    pin_config->ResetAllPins();
}


// read config from device
void MainWindow::on_pushButton_ReadConfig_clicked()        // херня? mb QtConcurrent::run()
{
    qDebug() << "button GET CONFIG clicked";
    ui->pushButton_ReadConfig->setEnabled(false);
//    //ui->button_GetConfig->setEnabled(false);
    QEventLoop loop;
    QObject context;
    context.moveToThread(thread_getSend_config);
    connect(thread_getSend_config, &QThread::started, &context, [&]() {
        qDebug() << "waiting... ";

        emit getConfigDone(hid_device_worker->GetConfigFromDevice());

        qDebug() << "done";
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

    // read pin config
    pin_config->WriteToConfig();            // проверить снизу
    // read axes config
    axes_config->WriteToConfig();
    // read axes curves config
    axes_curves_config->WriteToConfig();
    // read axes to buttons config
    a2b_config->WriteToConfig();
    // read shift registers config
    shift_reg_config->WriteToConfig();
    // read encoder config
    encoder_config->WriteToConfig();
    // read LED config
    led_config->WriteToConfig();
    // read adv.settings config
    ui->widget_2->WriteToConfig();
    // read button config
    button_config->WriteToConfig();

    qDebug() << "button SEND CONFIG clicked";
//    ui->button_SendConfig->setEnabled(false);
    QEventLoop loop;
    QObject context;
    context.moveToThread(thread_getSend_config);
    connect(thread_getSend_config, &QThread::started, &context, [&]() {
        qDebug() << "waiting... ";

        emit sendConfigDone(hid_device_worker->SendConfigToDevice());

        qDebug() << "done";
        loop.quit();
    });
    thread_getSend_config->start();
    loop.exec();
    thread_getSend_config->quit();
    thread_getSend_config->wait();
}


void MainWindow::configReceived(bool success)        // повторное наатие
{
    button_default_style_ = ui->pushButton_ReadConfig->styleSheet();
    static QString button_default_text = ui->pushButton_ReadConfig->text();

    if (success == true){
        // read pin config
        pin_config->ReadFromConfig();
        // read axes config
        axes_config->ReadFromConfig();
        // read axes curves config
        axes_curves_config->ReadFromConfig();
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

        // set firmware version
        QString str = QString::number(gEnv.pDeviceConfig->config.firmware_version, 16);
        if (str.size() == 4){
            ui->label_FirmwareVersion->setText(tr("Device firmware v") + str[0] + "." + str[1] + "." + str[2] + "b" + str[3]);
        }

        ui->pushButton_ReadConfig->setText(tr("Received"));
        ui->pushButton_ReadConfig->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");

        QTimer::singleShot(1000, [&]{
            ui->pushButton_ReadConfig->setStyleSheet(button_default_style_);
            ui->pushButton_ReadConfig->setText(button_default_text);
            ui->pushButton_ReadConfig->setEnabled(true);
        });
    } else {
        ui->pushButton_ReadConfig->setText(tr("Error"));
        ui->pushButton_ReadConfig->setStyleSheet("color: white; background-color: rgb(200, 0, 0);");

        QTimer::singleShot(1000, [&]{
            ui->pushButton_ReadConfig->setStyleSheet(button_default_style_);
            ui->pushButton_ReadConfig->setText(button_default_text);
            ui->pushButton_ReadConfig->setEnabled(true);
        });
    }
}

void MainWindow::configSent(bool success)
{
    button_default_style_ = ui->pushButton_ReadConfig->styleSheet();
    static QString button_default_text = ui->pushButton_WriteConfig->text();

    if (success == true){
        ui->pushButton_WriteConfig->setText(tr("Sent"));
        ui->pushButton_WriteConfig->setStyleSheet("color: white; background-color: rgb(0, 128, 0);");

        QTimer::singleShot(1000, [&]{
            ui->pushButton_WriteConfig->setStyleSheet(button_default_style_);
            ui->pushButton_WriteConfig->setText(button_default_text);
            ui->pushButton_WriteConfig->setEnabled(true);
        });
    } else {
        ui->pushButton_WriteConfig->setText(tr("Error"));
        ui->pushButton_WriteConfig->setStyleSheet("color: white; background-color: rgb(200, 0, 0);");

        QTimer::singleShot(1000, [&]{
            ui->pushButton_WriteConfig->setStyleSheet(button_default_style_);
            ui->pushButton_WriteConfig->setText(button_default_text);
            ui->pushButton_WriteConfig->setEnabled(true);
        });
    }
}
