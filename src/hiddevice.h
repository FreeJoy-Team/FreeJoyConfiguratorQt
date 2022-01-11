#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include <QObject>
#include <mutex>
#include "hidapi.h"

class HidDevice : public QObject
{
    Q_OBJECT

public:
    void getConfigFromDevice();
    void sendConfigToDevice();

    bool enterToFlashMode();
    void flashFirmware(const QByteArray *firmware);

    void setIsFinish(bool isFinish);
    void setSelectedDevice(int deviceNumber);

public slots:
    void processData();

signals:
    void deviceDisconnected();
    void deviceConnected();
    void flasherConnected();
    void paramsPacketReceived(bool firmwareCompatible);

    void configReceived(bool isSuccess);
    void configSent(bool isSuccess);

    void hidDeviceList(const QList<QPair<bool, QString>> &deviceNames);

    void flasherFound(bool isFound);
    void flashStatus(int status, int percent);

private:
    struct Device
    {
        ushort vid;
        ushort pid;
        std::wstring serNum;
        std::string path;

        Device (const hid_device_info *hid) {
            this->vid = hid->vendor_id;
            this->pid = hid->product_id;
            this->serNum = hid->serial_number;
            this->path = hid->path;
        }

        void operator = (const hid_device_info *hid) {
            this->vid = hid->vendor_id;
            this->pid = hid->product_id;
            this->serNum = hid->serial_number;
            this->path = hid->path;
        }
    };
    QList<Device> m_hidDevicesList;  // should be thread safe

    hid_device *m_paramsRead;
    hid_device *m_joyRead;

    int m_selectedDevice = -1;
    int m_currentWork;
    bool m_oldFirmwareSelected;
    bool m_isFinish = false;

    void readConfigFromDevice(uint8_t *buffer);
    void writeConfigToDevice(uint8_t *buffer);
    void flashFirmwareToDevice();

    QList<QPair<bool, QString>> m_deviceNames;
    QByteArray m_flasherPath;
    const QByteArray *m_firmware;

    mutable std::mutex m_mutex;
};

#endif // HIDDEVICE_H
