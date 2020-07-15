#include <QThread>
#include <QDebug>
#include <ctime>
#include "hiddevice.h"
#include "hidapi.h"
#include "common_defines.h"

//struct HidDeviceList
//{
//};

void HidDevice::processData()
{
    int res = 0;
    clock_t timer;
    bool change = false;
    bool no_device_sent = false;
    QList<hid_device_info*> tmp_HidDevicesAdrList;
    hid_device_info* hid_dev_info;
    QStringList str_list;
    uint8_t buffer[BUFFSIZE]={0};
    while (is_finish_ == false) {


        if (!change)
        {
            timer = clock();
            change = true;
        }
        else if (change && clock() - timer > 800)
        {
            hid_dev_info = hid_enumerate(0x0483, NULL);

            if (!hid_dev_info && no_device_sent == false){
                str_list.clear();
                HidDevicesAdrList.clear();
                emit hidDeviceList(&str_list);
                no_device_sent = true;
            }

            while(hid_dev_info)
            {
                tmp_HidDevicesAdrList.append(hid_dev_info);
                hid_dev_info = hid_dev_info->next;
                if (!hid_dev_info && HidDevicesAdrList.size() != tmp_HidDevicesAdrList.size())
                {
                    QThread::msleep(200);   // хз
                    HidDevicesAdrList.clear();
                    str_list.clear();
                    no_device_sent = false;
                    for (int i = 0; i < tmp_HidDevicesAdrList.size(); ++i)
                    {
                        HidDevicesAdrList.append(tmp_HidDevicesAdrList[i]);
                        str_list << QString::fromWCharArray(tmp_HidDevicesAdrList[i]->product_string);
                    }

                    emit hidDeviceList(&str_list);
                    tmp_HidDevicesAdrList.clear();
                }
            }
            tmp_HidDevicesAdrList.clear();
            change = false;
        }

        if (HidDevicesAdrList.size() && !handle_read) {
            handle_read = hid_open(0x0483, HidDevicesAdrList[0]->product_id,nullptr);
            if (!handle_read) {
                //emit putGamepadPacket(empty_buf_gamepad);
                emit putDisconnectedDeviceInfo();
                //hid_free_enumeration(hid_dev_info);
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
                    //QThread::msleep(5);            // обновление для обычных пакетов
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
                //QThread::msleep(50);            // ????
            }
        }
    }
    hid_free_enumeration(hid_dev_info);
}

void HidDevice::SetSelectedDevice(int device_number)
{
    if (device_number < 0){
        //device_number = 0;
        return;
    } else if (device_number > HidDevicesAdrList.size() - 1){
        device_number = HidDevicesAdrList.size() - 1;
    }
    selected_device_ = device_number;
    handle_read = hid_open(0x0483, HidDevicesAdrList[selected_device_]->product_id, HidDevicesAdrList[selected_device_]->serial_number);
}

void HidDevice::SetIsFinish(bool is_finish)
{
    is_finish_ = is_finish;
}

dev_config_t HidDevice::GetConfig()     // try catch
{
    clock_t millis;
    millis = clock() + 150;

    uint8_t config_request_buffer[2] = {REPORT_ID_CONFIG_IN, 1};
    hid_write(handle_read, config_request_buffer, 2);

    while (1)   // сделать таймаут
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
    //QThread::msleep(1500);
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

    while (1)   // сделать таймаут
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
