#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTranslator>

#include "hiddevice.h"
#include "reportconverter.h"

#include "widgets/advancedsettings.h"
#include "widgets/axesconfig.h"
#include "widgets/axescurvesconfig.h"
#include "widgets/buttonconfig.h"
#include "widgets/debugwindow.h"
#include "widgets/encodersconfig.h"
#include "widgets/ledconfig.h"
#include "widgets/pinconfig.h"
#include "widgets/shiftregistersconfig.h"

/* Defining */
//#define ORGANIZATION_NAME "FreeJoy"
//#define ORGANIZATION_DOMAIN "https://github.com/FreeJoy-Team"
//#define APPLICATION_NAME "FreeJoy QtConfigurator"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void getConfigDone(bool success);
    void sendConfigDone(bool success);

private slots:
    void showConnectDeviceInfo();
    void hideConnectDeviceInfo();
    void flasherConnected();
    void getParamsPacket(uint8_t *buffer);

    void configReceived(bool success);
    void configSent(bool success);

    void deviceFlasherController(bool isStartFlash);

    void hidDeviceList(const QStringList &deviceList);
    void hidDeviceListChanged(int index);

    void languageChanged(const QString &language);
    void setFont();

    void loadDefaultConfig();

    void interfaceStyleChanged(bool isDark);

    void on_pushButton_ResetAllPins_clicked();

    void on_pushButton_ReadConfig_clicked();
    void on_pushButton_WriteConfig_clicked();

    void on_pushButton_SaveToFile_clicked();
    void on_pushButton_LoadFromFile_clicked();

    void on_pushButton_SetDefaultConfig_clicked();
    void on_pushButton_LoadDefaultConfig_clicked();

    //void addvalues(int);

    void on_pushButton_TestButton_clicked();

    void on_pushButton_ShowDebug_clicked();

    void on_pushButton_TestButton_2_clicked();

    void on_pushButton_Wiki_clicked();

private:
    Ui::MainWindow *ui;
    //QTranslator m_translator;

    QThread *m_thread;            // smart pointer?
    HidDevice *m_hidDeviceWorker; // smart pointer?
    //QScopedPointer<QThread> thread;
    //QScopedPointer<HidDevice> worker;

    QThread *m_threadGetSendConfig; // smart pointer?

    PinConfig *m_pinConfig;
    ButtonConfig *m_buttonConfig;
    LedConfig *m_ledConfig;
    EncodersConfig *m_encoderConfig;
    ShiftRegistersConfig *m_shiftRegConfig;
    AxesConfig *m_axesConfig;
    AxesCurvesConfig *m_axesCurvesConfig;
    AdvancedSettings *m_advSettings;

    DebugWindow *m_debugWindow = nullptr;
    bool m_debugIsEnable;

    QString m_buttonDefaultStyle; // мб статик в функции?
                                  //    int index_device_before_write_;

    void readFromConfig();
    void writeToConfig();

    void loadAppConfig();
    void saveAppConfig();

    void loadDeviceConfigFromFile(QSettings *deviceSettings);
    void saveDeviceConfigToFile(QSettings *deviceSettings);

    //void moveEvent(QMoveEvent *event);
};
#endif // MAINWINDOW_H
