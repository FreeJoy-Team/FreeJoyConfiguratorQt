#include <QGridLayout>      //?
#include <QDebug>
#include <QThread>

#include <QPixmap>          //?
#include <QScopedPointer>   //?
#include <time.h>

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

    hid_device_worker = new HidDevice();
    thread = new QThread;

    thread_getSend_config = new QThread;

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


                                            //////////////// READ FROM CONFIG ////////////////
    // read pin config
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            pin_config, &PinConfig::ReadFromConfig);
    // read axes config
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            axes_config, &AxesConfig::ReadFromConfig);
    // read axes curves config
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            axes_curves_config, &AxesCurvesConfig::ReadFromConfig);
    // read axes to buttons config
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            a2b_config, &AxesToButtonsConfig::ReadFromConfig);
    // read shift registers config
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            shift_reg_config, &ShiftRegistersConfig::ReadFromConfig);
    // read encoder config
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            encoder_config, &EncodersConfig::ReadFromConfig);
    // read LED config
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            led_config, &LedConfig::ReadFromConfig);
    // read adv.settings config
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            ui->widget_2, &AdvancedSettings::ReadFromConfig);
    // read button config
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,     // last
            button_config, &ButtonConfig::ReadFromConfig);


                                            //////////////// WRITE TO CONFIG ////////////////
    // write pin config
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            pin_config, &PinConfig::WriteToConfig);
    // write button config
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            button_config, &ButtonConfig::WriteToConfig);
    // write axes config
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            axes_config, &AxesConfig::WriteToConfig);
    // write axes curves config
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            axes_curves_config, &AxesCurvesConfig::WriteToConfig);
    // write axes to buttons config
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            a2b_config, &AxesToButtonsConfig::WriteToConfig);
    // write shift registers config
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            shift_reg_config, &ShiftRegistersConfig::WriteToConfig);
    // write encoder config
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            encoder_config, &EncodersConfig::WriteToConfig);
    // write LED config
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            led_config, &LedConfig::WriteToConfig);
    // write adv.settings config
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            ui->widget_2, &AdvancedSettings::WriteToConfig);


                                            //////////////// SIGNASL-SLOTS ////////////////
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



//    connect(hid_device_worker, SIGNAL(putConfigPacket(uint8_t *)),
//                          SLOT(getConfigPacket(uint8_t *)));
//    connect(hid_device_worker, SIGNAL(putConfigRequest(uint8_t *)),
//                          SLOT(sendConfigPacket(uint8_t *)));

    thread->start();
}

MainWindow::~MainWindow()
{
    hid_device_worker->SetIsFinish(true);
    thread->quit();
    thread->deleteLater();
    thread->wait();
    delete hid_device_worker;
    delete thread;
    delete thread_getSend_config;       //
    delete ui;
}


void MainWindow::on_button_GetConfig_clicked()              // ХЕРНЯ ПЕРЕДЕЛАТЬ
{
    qDebug() << "button GET CONFIG clicked";
//    //ui->button_GetConfig->setEnabled(false);
    QEventLoop loop;
    QObject context;
    context.moveToThread(thread_getSend_config);
    connect(thread_getSend_config, &QThread::started, &context, [&]() {
        qDebug() << "waiting... ";
        gEnv.pDeviceConfig->config = hid_device_worker->GetConfig();
        //ui->button_GetConfig->setEnabled(true);
        qDebug() << "done";
        loop.quit();
    });
    thread_getSend_config->start();
    loop.exec();
    thread_getSend_config->quit();
    thread_getSend_config->wait();
    ui->label_DeviceName->setText(gEnv.pDeviceConfig->config.device_name);
    ui->label_VID->setText(QString::number(gEnv.pDeviceConfig->config.vid));
}


void MainWindow::on_button_SendConfig_clicked()             // !!!!!!!!!!!!!
{
    qDebug() << "button SEND CONFIG clicked";
//    ui->button_SendConfig->setEnabled(false);
    QEventLoop loop;
    QObject context;
    context.moveToThread(thread_getSend_config);
    connect(thread_getSend_config, &QThread::started, &context, [&]() {
        qDebug() << "waiting... ";
        hid_device_worker->SendConfig();
        //QThread::msleep(1100);
        //ui->button_SendConfig->setEnabled(true);
        qDebug() << "done";
        loop.quit();
    });
    thread_getSend_config->start();
    loop.exec();
    thread_getSend_config->quit();
    thread_getSend_config->wait();
}


void MainWindow::showConnectDeviceInfo() {
    ui->label_DeviceStatus->setText(tr("Connected"));
    ui->label_DeviceStatus->setStyleSheet("background-color: green");
    //ui->label_DeviceStatus->setVisible(true);
}


void MainWindow::hideConnectDeviceInfo() {
    //ui->label_DeviceStatus->setVisible(false);
    ui->label_DeviceStatus->setText(tr("Disconnected"));
    ui->label_DeviceStatus->setStyleSheet("background-color: red");
}

void MainWindow::on_button_EngLang_clicked()
{
    //QString s = QVariant::fromValue(ModelApple::Big).toString();
    //qDebug() << s;
    translator.load(":/FreeJoyTest_en");// + QString("ru_RU"));//QLocale::system().name();//QString("ru_RU"));//QLocale::name());
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
}

void MainWindow::on_button_RusLang_clicked()
{
    translator.load(":/FreeJoyTest_ru");// + QString("ru_RU"));//QLocale::system().name();//QString("ru_RU"));//QLocale::name());
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
    button_config->RetranslateUi();
}

//GetConfigFromDevice

// попробовать вынести в отдельный поток и повесить дилей
int asd = 0;
void MainWindow::getGamepadPacket(uint8_t * buff)                                    // НЕ В ЯДРЕ ВОРКЕРА
{
    ui->lineEdit->setText(QString::number(asd));
    report_convert.GamepadReport(buff);
//    // optimization
//    if(ui->tab_ButtonConfig->isVisible() == true){      // оптимизация. если открыт таб, то выполнять обновление
//        button_config->ButtonStateChanged();
//    }
//    // optimization
//    if(ui->tab_AxesConfig->isVisible() == true){      // оптимизация. если открыт таб, то выполнять обновление
//        axes_config->AxesValueChanged();
//    }

    static clock_t timer;
    static bool change = false;

    if (!change)
    {
        timer = clock();
        change = true;
    }
    else if (change && clock() - timer > 17)    // обновление раз в 17мс
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
        qDebug()<<"items count > 0 delete";
        return;
    } else if (device_list->size() > 0)
    {
        //qDebug()<<"hidDeviceList - changed";
        ui->comboBox_HidDeviceList->clear();
        //qDebug()<<"hidDeviceList - clear";
        ui->comboBox_HidDeviceList->addItems(*device_list);
        qDebug()<<"hidDeviceList - add";
    } else {
        //qDebug()<<"MainWindow::hidDeviceList - ERROR";
    }
}

void MainWindow::hidDeviceListChanged(int index)
{
    hid_device_worker->SetSelectedDevice(index);
}

//void MainWindow::getConfigPacket(uint8_t * buff)
//{

//}


//void MainWindow::sendConfigPacket(uint8_t * buff)
//{

//}

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


void MainWindow::on_pushButton_15_clicked()
{
    pin_config->ResetAllPins();
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

