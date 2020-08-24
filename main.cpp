#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>

// CryEngine global environment
#include "global.h"
GlobalEnvironment gEnv;
#include "deviceconfig.h"

#include <ctime>
int main(int argc, char *argv[])
{
//    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
//    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
//    QCoreApplication::setApplicationName(APPLICATION_NAME); QFont::Bold
    QElapsedTimer time;
    time.start();

    QApplication a(argc, argv);

    // global
    QSettings app_settings( "FreeJoySettings.conf", QSettings::IniFormat );
    DeviceConfig device_config;
    gEnv.pAppSettings = &app_settings;
    gEnv.pDeviceConfig = &device_config;

    gEnv.pApp_start_time = &time;

    // set font size
    gEnv.pAppSettings->beginGroup("FontSettings");
    QApplication::setFont(QFont("MS Shell Dlg 2", gEnv.pAppSettings->value("FontSize", "8").toInt()));
    gEnv.pAppSettings->endGroup();

    // set styleSheet
    gEnv.pAppSettings->beginGroup("StyleSettings");
    QString style = gEnv.pAppSettings->value("StyleSheet", "default").toString();
    gEnv.pAppSettings->endGroup();

    if (style == "default")
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
    else if (style == "white")
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
    else if (style == "dark")
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


    MainWindow w;
    w.show();
    return a.exec();
}
