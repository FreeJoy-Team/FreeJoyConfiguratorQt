#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include "hidapi.h"
#include "reportconverter.h"

#include "global.h"
#include "deviceconfig.h"

#include <QObject>

#define BUFFSIZE                            64
#define CONFIG_COUNT                        16

class HidDevice : public QObject
{
    Q_OBJECT

public:

    void GetConfigFromDevice();
    void SendConfigToDevice();

    bool EnterToFlashMode();
    void FlashFirmware(const QByteArray* firmware);

    void SetIsFinish(bool is_finish);
    void SetSelectedDevice(int device_number);


public slots:
    void processData();


signals:
    void putDisconnectedDeviceInfo();
    void putConnectedDeviceInfo();
    void putGamepadPacket(uint8_t *);

    void configReceived(bool is_success);
    void configSent(bool is_success);

    void hidDeviceList(QStringList* device_list);

    void flasherFound(bool is_found);
    void flashStatus(int status, int percent);

private:
    hid_device *handle_read;
    bool is_finish_ = false;
    int selected_device_;
    int current_work_;

    void ReadConfigFromDevice(uint8_t *buffer);
    void WriteConfigToDevice(uint8_t *buffer);
    void FlashFirmwareToDevice();

    uint8_t device_buffer_[BUFFSIZE];
    dev_config_t device_config_;                // ????
    QList<hid_device_info*> HidDevicesAdrList;
    hid_device_info* flasher_;
    const QByteArray* firmware_;

    ReportConverter *report_convert;                     // !!!!
};

#endif // HIDDEVICE_H
