#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include "hidapi.h"
#include "reportconverter.h"

#include "global.h"
#include "deviceconfig.h"

#include <QObject>
#include <QTime>    //?
//#include <QMutex>

class HidDevice : public QObject
{
    Q_OBJECT

public:

    hid_device *handle_read;                // private?
    bool GetConfigFromDevice();
    bool SendConfigToDevice();

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
    void putConfigPacket(uint8_t *);        // not used
    void putConfigRequest(uint8_t *);       // not used

    void hidDeviceList(QStringList* device_list);

    void flasherFound(bool is_found);
    void flashStatus(int status, int percent);

private:
    //hid_device *handle_read;
    bool is_finish_ = false;
    bool is_app_start = false;
    int selected_device_;
    uint8_t device_buffer_[BUFFSIZE];
    dev_config_t device_config_;                // ????
    QList<hid_device_info*> HidDevicesAdrList;
    hid_device_info* flasher_;

    //QMutex mutex_;
    ReportConverter *report_convert;                     // !!!!
};

#endif // HIDDEVICE_H
