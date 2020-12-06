#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include "hidapi.h"
#include "reportconverter.h"

#include "deviceconfig.h"
#include "global.h"

#include <QObject>

#define BUFFSIZE 64
#define CONFIG_COUNT 16

class HidDevice : public QObject
{
    Q_OBJECT

public:
    void getConfigFromDevice();
    void sendConfigToDevice();

    bool enterToFlashMode();
    void flashFirmware(const QByteArray *firmware);

    void setIsFinish(bool is_finish);
    void setSelectedDevice(int device_number);

public slots:
    void processData();

signals:
    void deviceDisconnected();
    void deviceConnected();
    void flasherConnected();
    void gamepadPacketReceived(uint8_t *);

    void configReceived(bool is_success);
    void configSent(bool is_success);

    void hidDeviceList(const QStringList &deviceList);

    void flasherFound(bool is_found);
    void flashStatus(int status, int percent);

private:
    hid_device *m_handleRead;
    bool m_isFinish = false;
    int m_selectedDevice;
    volatile int m_currentWork;

    void updateHidNames();

    void readConfigFromDevice(uint8_t *buffer);
    void writeConfigToDevice(uint8_t *buffer);
    void flashFirmwareToDevice();

    QStringList m_devicesNames;
    uint8_t m_deviceBuffer[BUFFSIZE];
    dev_config_t m_deviceConfig; // ????
    QList<hid_device_info *> m_HidDevicesAdrList;
    hid_device_info *m_flasher;
    const QByteArray *m_firmware;

    ReportConverter *m_reportConvert; // !!!!
};

#endif // HIDDEVICE_H
