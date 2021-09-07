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
#include "switchbutton.h"

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
    void setDefaultStyleSheet();

signals:
    void getConfigDone(bool success);
    void sendConfigDone(bool success);

private slots:
    void showConnectDeviceInfo();
    void hideConnectDeviceInfo();
    void flasherConnected();
    //void getParamsPacket(uint8_t *buffer);
    void getParamsPacket(bool firmwareCompatible);

    void configReceived(bool success);
    void configSent(bool success);

    void deviceFlasherController(bool isStartFlash);

    void hidDeviceList(const QList<QPair<bool, QString>> &deviceNames);
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

    void on_pushButton_TestButton_clicked();

    void on_pushButton_ShowDebug_clicked();

    void on_pushButton_TestButton_2_clicked();

    void on_pushButton_Wiki_clicked();

    void styleSwitched(int index);

private:
    Ui::MainWindow *ui;

    QThread *m_thread;
    HidDevice *m_hidDeviceWorker;

    QThread *m_threadGetSendConfig;

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

    bool m_deviceChanged;

    QString m_buttonDefaultStyle;

    void readFromConfig();
    void writeToConfig();
    void oldConfigHandler();

    void loadAppConfig();
    void saveAppConfig();

    void loadDeviceConfigFromFile(QSettings *deviceSettings);
    void saveDeviceConfigToFile(QSettings *deviceSettings);
};
#endif // MAINWINDOW_H
