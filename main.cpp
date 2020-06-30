#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

// CryEngine global environment
#include "global.h"
GlobalEnvironment gEnv;
#include "appconfig.h"
#include "deviceconfig.h"
#include "signalhandler.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // set stylesheet
    //QFile f(":qdarkstyle/style.qss");
    //QFile f(":/darkstyle/darkstyle.qss");
//    if (!f.exists())   {
//        printf("Unable to set stylesheet, file not found\n");
//    }
//    else   {
//        f.open(QFile::ReadOnly | QFile::Text);
//        QTextStream ts(&f);
//        qApp->setStyleSheet(ts.readAll());
//    }

    // new?
    AppConfig app_config;
    DeviceConfig device_config;
    SignalHandler signal_handler;   // снести нахер это ненужное говно
    gEnv.pAppConfig = &app_config;
    gEnv.pDeviceConfig = &device_config;
    gEnv.pSignalHandler = &signal_handler;


    MainWindow w;
    w.show();
    return a.exec();
}
