#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include "hidapi.h"
#include "reportconverter.h"

#include "global.h"
#include "deviceconfig.h"

#include <QObject>
#include <QTime>

class HidDevice : public QObject
{
    Q_OBJECT

public:

    hid_device *handle_read;                // private?
    dev_config_t GetConfig();
    void SendConfig();
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


private:
    //hid_device *handle_read;
    bool is_finish_ = false;
    int selected_device_;
    uint8_t device_buffer_[BUFFSIZE];
    dev_config_t device_config_;                // ????
    QList<hid_device_info*> HidDevicesAdrList;

    ReportConverter *report_convert;                     // !!!!
};

#endif // HIDDEVICE_H
