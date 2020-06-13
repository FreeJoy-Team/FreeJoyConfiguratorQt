#include <QGridLayout>
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
#include "appconfig.h"


//#include "reportconverter.h"
//#include "hiddevice.h"
//#include <memory>
//GLOBAL;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, thread(new QThread)           // ???????????????????????
    //, worker(new HidDevice)         // ??????????????????????????
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    connect(this, &Sender::valueChanged,
//     receiver, &Receiver::updateValue );
//    connect(sender, &Sender::valueChanged,
//     receiver, &Receiver::updateValue );

//    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
//            ui->widget_2, &ButtonLogical::Initialization);
//    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
//            ui->widget_2, &AdvancedSettings::WriteToConfig);

//    QList<QWidget*> LogicButtonAdrList;
//    QWidget *label = new QLabel();
//    ui->gridLayout->addWidget(label, row, column);
//    LogicButtonAdrList.append(label);

    // add dyn
//    QGridLayout* layout = new QGridLayout;
//    Encoders* qwe = new Encoders;
//    layout->addWidget(qwe);
//    ui->tab_Encoders->setLayout(layout);

    // dynamic widgets spawn
    QScrollBar* scroll = ui->scrollArea_2->verticalScrollBar();
    connect( scroll , SIGNAL(valueChanged(int)) ,this , SLOT(addvalues(int)) );

    // add button config
    button_config = new ButtonConfig(this);     // not need delete. this - parent
    ui->layoutV_tabButtonConfig->addWidget(button_config);
    // add pin config
    pin_config = new PinConfig(this);
    ui->layoutV_tabPinConfig->addWidget(pin_config);



    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            button_config, &ButtonConfig::Initialization);
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            pin_config, &PinConfig::ReadFromConfig);
    connect(ui->pushButton_TEST_MAIN_BUTTON, &QPushButton::clicked,
            ui->widget_2, &AdvancedSettings::Initialization);


    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            button_config, &ButtonConfig::WriteToConfig);
    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
            pin_config, &PinConfig::WriteToConfig);
//    connect(ui->pushButton_TEST_2_BUTTON, &QPushButton::clicked,
//            ui->widget_2, &AdvancedSettings::WriteToConfig);

//    /// needs to be deleted by you
//    m_HeapWidgetWithoutParent = new QWidget();
//    //doesn't need to be deleted by you
//    m_HeapWidgetWithParent = new QWidget(this);
//    // needs to be deleted by you
//    m_HeapNoQObj = new NoQObjectDerivedClass();


    hid_device_worker = new HidDevice();     //УТЕЧКА!!!!!!!!
    thread = new QThread;
     //thread.reset(new QThread);       //in some arbitrary member function
//    QScopedPointer<QThread> p(new QThread);
//    connect(p.data(), SIGNAL(finished()), SLOT(onThreadFinish()));
    //QScopedPointer<QThread> thread(new QThread);
    //QScopedPointer<HidDevice> worker(new HidDevice);
    //std::unique_ptr<QThread> thread(new QThread);
    //QThread* thread = new QThread;              // delete? //unique_ptr #include <memory>
    //HidDevice* worker = new HidDevice();                   //std::unique_ptr<Item> item(new Item);      //УТЕЧКА!!!!!!!!
    //std::unique_ptr<HidDevice> worker(new HidDevice);
    //connect(ui->button_GetConfig, SIGNAL(clicked()), SLOT(getConfig_Slot()));

    //    worker->moveToThread(thread.data());    //thread.data()
    //    connect(thread.data(), SIGNAL(started()), worker.data(), SLOT(processData()));      //worker.data()
    //    connect(worker.data(), SIGNAL(putGamepadPacket(uint8_t *)),
    //                          SLOT(getGamepadPacket(uint8_t *)));
    //    connect(worker.data(), SIGNAL(putConnectedDeviceInfo()),
    //                          SLOT(showConnectDeviceInfo()));
    //    connect(worker.data(), SIGNAL(putDisconnectedDeviceInfo()),
    //                          SLOT(hideConnectDeviceInfo()));
    //    connect(worker.data(), SIGNAL(putConfigPacket(uint8_t *)),
    //                          SLOT(getConfigPacket(uint8_t *)));
    //    thread->start();

//    QThread* thread = new QThread;
//    Worker* worker = new Worker();
//    worker->moveToThread(thread);
//    connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
//    connect(thread, SIGNAL(started()), worker, SLOT(process()));
//    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
//    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
//    thread->start();


    hid_device_worker->moveToThread(thread);    //thread.data()


    connect(thread, SIGNAL(started()), hid_device_worker, SLOT(processData()));      //worker.data()


    connect(hid_device_worker, SIGNAL(putGamepadPacket(uint8_t *)),
                          SLOT(getGamepadPacket(uint8_t *)));
    connect(hid_device_worker, SIGNAL(putConnectedDeviceInfo()),
                          SLOT(showConnectDeviceInfo()));
    connect(hid_device_worker, SIGNAL(putDisconnectedDeviceInfo()),
                          SLOT(hideConnectDeviceInfo()));

//    connect(hid_device_worker, SIGNAL(putConfigPacket(uint8_t *)),
//                          SLOT(getConfigPacket(uint8_t *)));
//    connect(hid_device_worker, SIGNAL(putConfigRequest(uint8_t *)),
//                          SLOT(sendConfigPacket(uint8_t *)));

    thread->start();

}

MainWindow::~MainWindow()
{
    //thread->quit();
    //thread->wait();
    //delete worker;
    //delete thread;
//    if (thread)
//    {
//        thread->free();
//        thread = 0;
//    }
    delete ui;
}


void MainWindow::on_button_GetConfig_clicked()              // ХЕРНЯ ПЕРЕДЕЛАТЬ
{
    qDebug() << "button GET CONFIG clicked";
    //ui->button_GetConfig->setEnabled(false);
    QThread thread;
    QEventLoop loop;
    QObject context;
    context.moveToThread(&thread);
    connect(&thread, &QThread::started, &context, [&]() {
        qDebug() << "waiting... ";
        gEnv.pDeviceConfig->config = hid_device_worker->GetConfig();
        //device_config.config = hid_device_worker->GetConfig();
        //QThread::msleep(2000);
        //ui->button_GetConfig->setEnabled(true);
        qDebug() << "done";
        loop.quit();
    });
    thread.start();
    loop.exec();
    thread.quit();
    thread.wait();
    ui->label_DeviceName->setText(gEnv.pDeviceConfig->config.device_name);
    ui->label_VID->setText(QString::number(gEnv.pDeviceConfig->config.vid));
}


void MainWindow::on_button_SendConfig_clicked()             // !!!!!!!!!!!!!
{
    qDebug() << "button SEND CONFIG clicked";
    ui->button_SendConfig->setEnabled(false);
    QThread thread;
    QEventLoop loop;
    QObject context;
    context.moveToThread(&thread);
    connect(&thread, &QThread::started, &context, [&]() {
        qDebug() << "waiting... ";
        hid_device_worker->SendConfig();
        QThread::msleep(1100);
        ui->button_SendConfig->setEnabled(true);
        qDebug() << "done";
        loop.quit();
    });
    thread.start();
    loop.exec();
    thread.quit();
    thread.wait();
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
}


//GetConfigFromDevice
int asd = 0;
void MainWindow::getGamepadPacket(uint8_t * buff){
    //ReportConverter report_convert;                                      // НЕ В ЯДРЕ ВОРКЕРА
    report_convert.GamepadReport(buff);
    //memcpy(&(gamepad_report.id), buff, sizeof(joy_report_t));
    //ui->label_ID->setText(QString::number(config->gamepad_report.id));
    //ui->label_ID->setText(QString::number(config->config.toggle_press_time_ms));
    ui->lineEdit->setText(QString::number(asd));
    button_config->ButtonStateChanged();
    //qDebug() << config->config.toggle_press_time_ms;
    //qDebug() << init_config.button_delay3_ms;
    //qDebug() << device_config.config.device_name;
    asd++;
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

void MainWindow::on_pushButton_4_clicked()
{
    //qDebug() << device_config.config.button_timer3_ms;
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

void MainWindow::on_pushButton_QssAqua_clicked()
{
    QFile f(":/QSS-master/Aqua.qss");
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

void MainWindow::on_pushButton_QssElegantDark_clicked()
{
    QFile f(":/QSS-master/ElegantDark.qss");
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

void MainWindow::on_pushButton_QssAMOLED_clicked()
{
    QFile f(":/QSS-master/AMOLED.qss");
    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}

void MainWindow::on_pushButton_QssConsoleStyle_clicked()
{
    QFile f(":/QSS-master/ConsoleStyle.qss");
    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}

void MainWindow::on_pushButton_QssManjaroMix_clicked()
{
    QFile f(":/QSS-master/ManjaroMix.qss");
    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}

void MainWindow::on_pushButton_QssMaterialDark_clicked()
{
    QFile f(":/QSS-master/MaterialDark.qss");
    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}






///////////////////////////////////

QList<QWidget*> LogicButtonAdrList;
int column = 0;
int row = 0;
int ButtonNumber;

void MainWindow::on_pushButton_clicked()
{
    qDebug() << (uint8_t*)REPORT_ID_CONFIG_IN;
    ButtonNumber = ui->spinBoxButtonNumber->value();
    for (int i = 0; i < ButtonNumber; i++) {
        QWidget *label = new QLabel();

        if(column>5)
        {
            row++;
            column = 0;
        }
        label->setStyleSheet("background-image: url(:/Images/OFF_small_t.png)");
        label->setMaximumSize(50, 43);
        label->setMinimumSize(50, 43);
        ui->gridLayout->addWidget(label, row, column);  //QWidget *label = new QLabel();
        column++;
        LogicButtonAdrList.append(label);       //QList<QWidget*> LogicButtonAdrList;

    }
}

void MainWindow::on_pushButton_2_clicked()
{
    ButtonNumber = LogicButtonAdrList.size();
    //qDebug() << ButtonNumber;

    while (LogicButtonAdrList.size() > 0) {
        QWidget *widget = LogicButtonAdrList.takeLast();
        ui->gridLayout->removeWidget(widget);
        widget->deleteLater();
    }
    row = column = 0;
}

void MainWindow::on_pushButton_3_clicked()
{
    ButtonNumber = ui->spinBoxButtonNumber->value();

    for (int i = 0; i < ButtonNumber; i++) {
        if (LogicButtonAdrList.size() > 0) {
            QWidget *widget = LogicButtonAdrList.takeLast();
            ui->gridLayout->removeWidget(widget);
            widget->deleteLater();
        }

//        column--;
//        if (column == 0)
//        {
//            row--;
//            column = 5;
//        }
    }
    row = ui->gridLayout->rowCount();
    column = ui->gridLayout->columnCount();
    if (column < 5) row--;
}

#define BUTTON_COUNT_TEST 40
QList<ButtonConfig*> LogicButtonAdrList2;
void MainWindow::on_pushButton_15_clicked()
{
    clock_t start = clock();
    for (int i = 0; i< BUTTON_COUNT_TEST; i++) {
        ButtonConfig* qwe = new ButtonConfig;
        ui->verticalLayout_3->addWidget(qwe, i);
        LogicButtonAdrList2.append(qwe);
    }
    clock_t stop = clock();
    qDebug()<< "layout create ms= " << stop - start;
    QList<PinComboBox *> allPButtons = this->findChildren<PinComboBox *>();


//    std::thread thread2([&](){
//        for (int i = 0; i< allPButtons.size(); i++) {
//            allPButtons[i]->PinComboBox::GenText();
//        }
//        }
//        );
//    thread2.detach();

//    for (int i = 0; i< allPButtons.size(); i++) {
//        allPButtons[i]->PinComboBox::GenText();
//    }

    stop = clock();
    qDebug()<< "суммарно все вычисления = " << stop - start;
}

// dynamic widgets spawn
void MainWindow::addvalues(int value)
{
    if (value > 90 && LogicButtonAdrList2.size()< 512)
    {
        for (int i = 0; i< 5; i++) {
            ButtonConfig* qwe = new ButtonConfig;
            ui->verticalLayout_3->addWidget(qwe, i);
            LogicButtonAdrList2.append(qwe);
        }
//        QList<PinComboBox *> allPButtons = this->findChildren<PinComboBox *>();
//        for (int i = 0; i< allPButtons.size(); i++) {
//            allPButtons[i]->PinComboBox::GenText();
//        }
    }
}
