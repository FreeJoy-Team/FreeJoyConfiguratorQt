#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include "hidapi.h"
#include "reportconverter.h"

#include "deviceconfig.h"
#include "global.h"

#include <QObject>

class HidDevice : public QObject
{
    Q_OBJECT

public:
    void getConfigFromDevice();
    void sendConfigToDevice();

    bool enterToFlashMode();
    void flashFirmware(const QByteArray *firmware);

    void setIsFinish(bool is_finish);
    void setSelectedDevice(int deviceNumber);

public slots:
    void processData();

signals:
    void deviceDisconnected();
    void deviceConnected();
    void flasherConnected();
    void paramsPacketReceived(uint8_t *buffer);

    void configReceived(bool is_success);
    void configSent(bool is_success);

    void hidDeviceList(const QList<QPair<bool, QString>> &deviceNames);

    void flasherFound(bool is_found);
    void flashStatus(int status, int percent);

private:
    hid_device *m_paramsRead;
    hid_device *m_joyRead;
    bool m_isFinish = false;
    volatile int m_selectedDevice = -1;
    volatile int m_currentWork;

    bool m_oldFirmwareSelected;

    void readConfigFromDevice(uint8_t *buffer);
    void writeConfigToDevice(uint8_t *buffer);
    void flashFirmwareToDevice();

    QList<QPair<bool, QString>> m_deviceNames;
    uint8_t m_deviceBuffer[BUFFERSIZE];
    QList<hid_device_info *> m_HidDevicesAdrList;
    hid_device_info *m_flasher;
    const QByteArray *m_firmware;
};

#endif // HIDDEVICE_H
