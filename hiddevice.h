#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include "hidapi.h"
#include "reportconverter.h"

#include "global.h"
#include "deviceconfig.h"

#include <QObject>

class HidDevice : public QObject
{
    Q_OBJECT

public:

    hid_device_ *handle_read;                // private?
    void GetConfigFromDevice();
    void SendConfigToDevice();

    bool EnterToFlashMode();
    void FlashFirmware(const QByteArray* file_bytes);

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
    //hid_device *handle_read;
    bool is_finish_ = false;
    int selected_device_;
    int current_work_;

    uint8_t device_buffer_[BUFFSIZE];
    dev_config_t device_config_;                // ????
    QList<hid_device_info*> HidDevicesAdrList;
    hid_device_info* flasher_;

    ReportConverter *report_convert;                     // !!!!
};

#endif // HIDDEVICE_H
