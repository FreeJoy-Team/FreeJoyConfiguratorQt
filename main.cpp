#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>

// CryEngine global environment
#include "global.h"
GlobalEnvironment gEnv;
#include "deviceconfig.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //qputenv("QT_SCALE_FACTOR", "0.8");

    QElapsedTimer time;
    time.start();

    QApplication a(argc, argv);

    // global
    QString app_version = "0.4.9";     // тупо, надо в дефайне
    QSettings app_settings( "FreeJoySettings.conf", QSettings::IniFormat );
    DeviceConfig device_config;
    gEnv.pAppVersion = &app_version;
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

    if (style == "default") // ?
    {
        QFile f(":/styles/default.qss");
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
        QFile f(":qss/qss.qss");
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
