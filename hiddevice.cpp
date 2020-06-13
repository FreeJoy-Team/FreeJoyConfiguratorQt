#include <QThread>
#include <QDebug>
#include <time.h>
#include "hiddevice.h"
#include "hidapi.h"
#include "common_defines.h"

void HidDevice::processData()
{
    //hid_device_info* qwe = hid_enumerate(0x0483, NULL);

    int res = 0;
    uint8_t buffer[BUFFSIZE]={0};
    while (1) {
        if (!handle_read) {
            handle_read = hid_open(0x0483, 0x5750,nullptr);
            if (!handle_read) {
                //emit putGamepadPacket(empty_buf_gamepad);
                emit putDisconnectedDeviceInfo();
                QThread::msleep(500);
            } else {
                emit putConnectedDeviceInfo();
            }
        }

        if (handle_read) {
            res=hid_read_timeout(handle_read, buffer, BUFFSIZE,10000);         // 10000?
            //  res=hid_read(handle_read, buf, BUFFSIZE);
            if (res < 0) {
                hid_close(handle_read);
                handle_read=nullptr;
            } else {
                if (buffer[0] == REPORT_ID_JOY) {
                    memset(device_buffer_, 0, BUFFSIZE);
                    memcpy(device_buffer_, buffer, BUFFSIZE);
                    emit putGamepadPacket(device_buffer_);
                }
                else if (buffer[0] == REPORT_ID_CONFIG_IN) {                         // NOT USED // config from device
                    //qDebug() << "hiddevice buf[0] == 2";
                    memset(device_buffer_, 0, BUFFSIZE);
                    memcpy(device_buffer_, buffer, BUFFSIZE);
                    //emit putConfigPacket(config_buffer_);
                }
                else if (buffer[0] == REPORT_ID_CONFIG_OUT) {                        // NOT USED // config to device
                    //qDebug() << "hiddevice buf[0] == 3// buf = " << buf[1];
                    memset(device_buffer_, 0, BUFFSIZE);
                    memcpy(device_buffer_, buffer, BUFFSIZE);
                    emit putConfigRequest(device_buffer_);
                }
                QThread::msleep(1);            // ????
            }
        }
    }
}


dev_config_t HidDevice::GetConfig()     // try catch
{
    clock_t millis;
    millis = clock() + 150;

    uint8_t config_request_buffer[2] = {REPORT_ID_CONFIG_IN, 1};
    hid_write(handle_read, config_request_buffer, 2);

    while (1)
    {
        if (device_buffer_[0] == REPORT_ID_CONFIG_IN)
        {
            if (device_buffer_[1] == config_request_buffer[1])
            {
                //device_config_
                gEnv.pDeviceConfig->config = report_convert->GetConfigFromDevice(device_buffer_);           // slow
                config_request_buffer[1] += 1;
                hid_write(handle_read, config_request_buffer, 2);

                if (config_request_buffer[1] > CONFIG_COUNT)
                {
                    //qDebug() << "break";
                    break;
                }
            }
        }
        else if (config_request_buffer[1] < 2 && (millis - clock()) <= 0)
        {
            //qDebug() << "RESEND ACTIVATED";
            config_request_buffer[1] = 1;
            hid_write(handle_read, config_request_buffer, 2);
        }
        //QThread::msleep(5);
    }
    //qDebug() << "return";
    return gEnv.pDeviceConfig->config;          // !!!!!!!!!!!!!!!!!!!!!! nah
}


void HidDevice::SendConfig()      // try catch
{
//    //uint8_t config_request_buffer[64] = {REPORT_ID_FIRMWARE,'b','o','o','t','l','o','a','d','e','r',' ','r','u','n'};
//    uint8_t config_buffer[64] = {REPORT_ID_FIRMWARE, 1};
//    hid_write(handle_read, config_buffer, 64);
//    dev_config_t device_config = gEnv.pDeviceConfig->config;
    clock_t millis;
    millis = clock() + 150;

    uint8_t config_buffer[64] = {REPORT_ID_CONFIG_OUT, 0};        // check 64 2
    hid_write(handle_read, config_buffer, 64);

    while (1)
    {
        if (device_buffer_[0] == REPORT_ID_CONFIG_OUT)
        {
            if (device_buffer_[1] == config_buffer[1] + 1)
            {
                config_buffer[1] += 1;
                std::vector<uint8_t> tmp_buf = report_convert->SendConfigToDevice(config_buffer[1]);

                //memcpy((uint8_t*)(config_buffer), tmp, BUFFSIZE);
                for (int i = 2; i < 64; i++)
                {                                                                                     // какой пиздец
                    config_buffer[i] = tmp_buf[i];
                }

                hid_write(handle_read, config_buffer, BUFFSIZE);
                //config_buffer[1] += 1;

                if (device_buffer_[1] == CONFIG_COUNT) break;
            }
        }
        else if (config_buffer[1] == 0 && (millis - clock()) <= 0)
        {
            qDebug() << "RESEND ACTIVATED";
            //config_buffer[1] = 0;
            hid_write(handle_read, config_buffer, 64);
        }
        //QThread::msleep(20);
    }
}



//vid 0x0483
//pid 0x5750
