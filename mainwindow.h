#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QSettings>

#include "hiddevice.h"
#include "reportconverter.h"

#include "widgets/encodersconfig.h"
#include "widgets/buttonconfig.h"
#include "widgets/pinconfig.h"
#include "widgets/advancedsettings.h"
#include "widgets/axesconfig.h"
#include "widgets/axescurvesconfig.h"
#include "widgets/ledconfig.h"
#include "widgets/shiftregistersconfig.h"
#include "widgets/debugwindow.h"

/* Defining */
//#define ORGANIZATION_NAME "FreeJoy"
//#define ORGANIZATION_DOMAIN "https://github.com/FreeJoy-Team"
//#define APPLICATION_NAME "FreeJoy QtConfigurator"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

//private:
    //QScopedPointer<QThread> thread;
    //QScopedPointer<HidDevice> worker;

signals:
    void getConfigDone(bool success);
    void sendConfigDone(bool success);

    void test();


private slots:
    void showConnectDeviceInfo();
    void hideConnectDeviceInfo();
    void getGamepadPacket(uint8_t *);

    void configReceived(bool success);
    void configSent(bool success);

    void deviceFlasherController(bool is_start_flash);

    void hidDeviceList(QStringList* device_list);
    void hidDeviceListChanged(int index);

    void languageChanged(QString language);
    void setFont();

    void loadDefaultConfig();

    void interfaceStyleChanged(bool is_dark);

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

private:
    Ui::MainWindow *ui;
    QTranslator translator_;

    QThread* thread_;
    HidDevice* hid_device_worker_;

    QThread* thread_getSend_config_;
    ReportConverter report_convert_;     // nah?

    PinConfig* pin_config_;
    ButtonConfig* button_config_;
    LedConfig* led_config_;
    EncodersConfig* encoder_config_;
    ShiftRegistersConfig* shift_reg_config_;
    AxesConfig* axes_config_;
    AxesCurvesConfig* axes_curves_config_;
    AdvancedSettings* adv_settings_;

    DebugWindow* debug_window_ = nullptr;
    bool debug_is_enable_;


    QString button_default_style_;  // мб статик в функции?
//    int index_device_before_write_;

    void ReadFromConfig();
    void WriteToConfig();

    void LoadAppConfig();
    void SaveAppConfig();

    void LoadDeviceConfigFromFile(QSettings* device_settings);
    void SaveDeviceConfigToFile(QSettings* device_settings);

    //void moveEvent(QMoveEvent *event);
};
#endif // MAINWINDOW_H
