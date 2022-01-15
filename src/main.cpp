#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QTextStream>
#include <QStyleFactory>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include "infolabel.h"

// global environment
#include "global.h"
GlobalEnvironment gEnv;
#include "deviceconfig.h"

// Get the default Qt message handler.
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);

// define QT_NO_DEBUG_OUTPUT - no debug info
void CustomMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // mutex?
    if (gEnv.pDebugWindow != nullptr) {
        // для мультипотока, хз правильно ли, но работает // не уверен насчёт ссылки, мб надо копию передавать с мультипотоком
        QMetaObject::invokeMethod(gEnv.pDebugWindow, "printMsg", Qt::QueuedConnection, Q_ARG(QString, msg));
    }

    // Call the default handler.
    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
}


int main(int argc, char *argv[])
{
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #endif
    //qputenv("QT_SCALE_FACTOR", "0.8");
    qRegisterMetaType<QList<QPair<bool, QString>> >();

    QElapsedTimer time;
    time.start();

    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setStyle(new InfoProxyStyle(qApp->style()));
    QApplication a(argc, argv);

    QString docLoc = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (docLoc.isEmpty() == false) {
        docLoc+= "/FreeJoy/";
    }

    QDir dir(docLoc);
    if (dir.exists() == false) {
        dir.mkpath(".");
        dir.mkpath("configs");
    }

    QSettings appSettings(docLoc + "FreeJoySettings.conf", QSettings::IniFormat);

    DeviceConfig deviceConfig;
    QTranslator translator;

    // global
    gEnv.pAppSettings = &appSettings;
    gEnv.pDeviceConfig = &deviceConfig;
    gEnv.pTranslator = &translator;

    qInstallMessageHandler(CustomMessageHandler);

    gEnv.pApp_start_time = &time;

    // set font size
    gEnv.pAppSettings->beginGroup("FontSettings");
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(gEnv.pAppSettings->value("FontSize", "8").toInt());
    qApp->setFont(defaultFont);
    gEnv.pAppSettings->endGroup();

    // load language settings
    appSettings.beginGroup("LanguageSettings");
    bool ok = false;
    if (appSettings.value("Language", "english").toString() == "russian")
    {
        ok = gEnv.pTranslator->load(":/FreeJoyQt_ru");
        if (ok == false) {
            qCritical()<<"failed to load translate file";
        } else {
            qApp->installTranslator(gEnv.pTranslator);
        }
    }
    else if (appSettings.value("Language", "english").toString() == "schinese")
    {
        ok = gEnv.pTranslator->load(":/FreeJoyQt_zh_CN");
        if (ok == false) {
            qCritical()<<"failed to load translate file";
        } else {
            qApp->installTranslator(gEnv.pTranslator);
        }
    }
    appSettings.endGroup();

    MainWindow w;

    qDebug() << "Application startup time =" << gEnv.pApp_start_time->elapsed() << "ms";
    w.show();

    return a.exec();
}
