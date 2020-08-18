#include <QThread>
#include <QDebug>
#include <ctime>
#include "hiddevice.h"
#include "hidapi.h"
#include "common_defines.h"
#include "firmwareupdater.h"

//struct HidDeviceList
//{
//};
                                /////////////////////////// это самое слабое место в программе, переписать при первой возможности //////////////////////////
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

    while (is_finish_ == false)
    {
//        //////////////
//        clock_t start = clock();
//        for(int i = 0; i < 100; ++i){
//            hid_dev_info = hid_enumerate(0x0483, NULL);
//        }
//        clock_t stop = clock();
//        qDebug()<<"hid_enumerate time ="<<stop- start;
//        /////////////

        // check connected devices
        if (!change)
        {
            timer = clock();
            change = true;
        }
        else if (change && clock() - timer > 800)   // change always true
        {
            // goto
            //link:
            hid_dev_info = hid_enumerate(0x0483, 0x0); //NULL   FreeJoy Flasher flasher_
            if (!hid_dev_info && no_device_sent == false)
            {
                str_list.clear();
                HidDevicesAdrList.clear();
                //is_app_start = true;    //?
                emit hidDeviceList(&str_list);
                no_device_sent = true;
            }

            while(hid_dev_info)
            {
                if(QString::fromWCharArray(hid_dev_info->product_string) == "FreeJoy Flasher"){
                    if (!flasher_){
                        flasher_ = hid_dev_info;
                        emit flasherFound(true);
                    }
                    flasher_ = hid_dev_info;
                    hid_dev_info = hid_dev_info->next;
                    continue;
                }

                tmp_HidDevicesAdrList.append(hid_dev_info);
                hid_dev_info = hid_dev_info->next;
                if (!hid_dev_info && HidDevicesAdrList.size() != tmp_HidDevicesAdrList.size())
                {
                    //QThread::msleep(20);   // хз
                    HidDevicesAdrList.clear();
                    str_list.clear();
                    no_device_sent = false;
                    for (int i = 0; i < tmp_HidDevicesAdrList.size(); ++i)
                    {
                        //qDebug()<<"!!!!!!!!!!QWE = "<<Qwe();
//                        if (QString::fromWCharArray(tmp_HidDevicesAdrList[i]->product_string) == ""){
//                            qDebug()<<"NULL NULL";
//                            ////tmp_HidDevicesAdrList.clear();
//                            ////goto link;
//                        }
                        HidDevicesAdrList.append(tmp_HidDevicesAdrList[i]);
                        str_list << QString::fromWCharArray(tmp_HidDevicesAdrList[i]->product_string);
                    }
                    //is_app_start = true;    //?
                    emit hidDeviceList(&str_list);
                    tmp_HidDevicesAdrList.clear();
                }
            }
            tmp_HidDevicesAdrList.clear();
            change = false;
        }

        // no device
        if (!handle_read)
        {
            if (HidDevicesAdrList.size()){          // ?
                handle_read = hid_open(0x0483, HidDevicesAdrList[0]->product_id,nullptr);
            }
            if (!handle_read) {
                emit putDisconnectedDeviceInfo();
                //hid_free_enumeration(hid_dev_info);
                QThread::msleep(300);
            } else {
                //is_app_start = true;    //?
                emit putConnectedDeviceInfo();
            }
        }
        // device connected
        if (handle_read)
        {

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
                    //QThread::msleep(5);            // хз почему даже 5мс тормозит обновление интерфейса(или отправку сигнала?) на ~100мс
                }
                else if (buffer[0] == REPORT_ID_CONFIG_IN) {                         // config from device
                    memset(device_buffer_, 0, BUFFSIZE);
                    memcpy(device_buffer_, buffer, BUFFSIZE);
                    //emit putConfigPacket(config_buffer_);
                }
                else if (buffer[0] == REPORT_ID_CONFIG_OUT) {                        // config to device
                    memset(device_buffer_, 0, BUFFSIZE);
                    memcpy(device_buffer_, buffer, BUFFSIZE);
                    //emit putConfigRequest(device_buffer_);
                }
                //QThread::msleep(50);            // ????
            }
        }

    }
    hid_free_enumeration(hid_dev_info);
}


void HidDevice::SetSelectedDevice(int device_number)        // заблочить сигнал до запуска, скорее всего крашит из-за разных потоков
{                                                           // нее, нет эффекта от мьютекса. хз
    if (device_number < 0){
        //device_number = 0;
        return;
    } else if (device_number > HidDevicesAdrList.size() - 1){
        device_number = HidDevicesAdrList.size() - 1;
    }
    selected_device_ = device_number; 
    qDebug()<<"HID open start";
    // возможно не стоит здесь открывать, оставить изменение selected_device_, а открытие в processData()
    handle_read = hid_open(0x0483, HidDevicesAdrList[selected_device_]->product_id, HidDevicesAdrList[selected_device_]->serial_number);
//    if (!handle_read) {
//        emit putDisconnectedDeviceInfo();
//        //hid_free_enumeration(hid_dev_info);
//    } else {
//        emit putConnectedDeviceInfo();
//    }
    qDebug()<<"No crash, HID opened";
    qDebug()<<"!!!!!!!!!!QWE = "<<Qwe();
}


void HidDevice::SetIsFinish(bool is_finish)
{
    is_finish_ = is_finish;
}


bool HidDevice::GetConfigFromDevice()     // try catch
{
    qDebug()<<"before if(handle_read)";
    if(handle_read)
    {
        qDebug()<<"after if(handle_read)";
        clock_t millis;
        millis = clock();
        int report_count = 0;

        uint8_t config_request_buffer[2] = {REPORT_ID_CONFIG_IN, 1};
        qDebug()<<"before hid_write";
        hid_write(handle_read, config_request_buffer, 2);       // тут проблема, редко виснет на этой функции. мб мьютекс хз
        qDebug()<<"start read cycle";
        while (clock() < millis + 1000)   // сделать таймаут 2000ms
        {
            qDebug()<<"read";
            if (device_buffer_[0] == REPORT_ID_CONFIG_IN)
            {
                if (device_buffer_[1] == config_request_buffer[1])
                {
                    //device_config_
                    gEnv.pDeviceConfig->config = report_convert->GetConfigFromDevice(device_buffer_);           // slow
                    config_request_buffer[1] += 1;
                    hid_write(handle_read, config_request_buffer, 2);
                    report_count++;

                    if (config_request_buffer[1] > CONFIG_COUNT)
                    {
                        //qDebug() << "break";
                        break;
                    }
                }
            }
            else if (config_request_buffer[1] < 2 && ( (millis + 150) - clock()) <= 0)
            {
                qDebug() << "RESEND ACTIVATED";
                config_request_buffer[1] = 1;
                hid_write(handle_read, config_request_buffer, 2);
            }
            //QThread::msleep(5);
        }
        qDebug()<<"report_count="<<report_count<<"  CONFIG_COUNT = "<<CONFIG_COUNT;
        if (report_count == CONFIG_COUNT){
            return true;
        } else {
            return false;
        }

    }
    return false;
}


bool HidDevice::SendConfigToDevice()      // try catch
{
//    //uint8_t config_request_buffer[64] = {REPORT_ID_FIRMWARE,'b','o','o','t','l','o','a','d','e','r',' ','r','u','n'};
//    hid_write(handle_read, config_buffer, 64);
    if(handle_read)
    {
        clock_t millis;
        millis = clock();
        int report_count = 0;

        uint8_t config_buffer[64] = {REPORT_ID_CONFIG_OUT, 0};        // check 64 2
        hid_write(handle_read, config_buffer, 64);
        qDebug()<<"start write cycle";
        while (clock() < millis + 1000)   // сделать таймаут
        {
            qDebug()<<"write";
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
                    report_count++;

                    if (device_buffer_[1] == CONFIG_COUNT){
                        break;
                    }
                }
            }
            else if (config_buffer[1] == 0 && ( (millis + 150) - clock()) <= 0)
            {
                qDebug() << "RESEND ACTIVATED";
                //config_buffer[1] = 0;
                hid_write(handle_read, config_buffer, 64);
            }
            //QThread::msleep(20);
        }

        if (report_count == CONFIG_COUNT){
            return true;
        } else {
            return false;
        }
    }
    return false;
}


bool HidDevice::EnterToFlashMode()
{
    if(handle_read)
    {
        clock_t millis;
        millis = clock();

        uint8_t config_buffer[64] = {REPORT_ID_FIRMWARE,'b','o','o','t','l','o','a','d','e','r',' ','r','u','n'};
        hid_write(handle_read, config_buffer, 64);

        while (clock() < millis + 1000)
        {
            if (flasher_){
                return true;
            }
        }
    }
    return false;
}


void HidDevice::FlashFirmware(const QByteArray* file_bytes)
{
    qDebug()<<"size = "<<file_bytes->size();
    if(flasher_)
    {
        hid_device* flasher = hid_open(0x0483, flasher_->product_id, flasher_->serial_number);;
        clock_t millis;
        millis = clock();
        uint8_t flash_buffer[BUFFSIZE]{};
        uint8_t flasher_device_buffer[BUFFSIZE]{};
        uint16_t length = (uint16_t)file_bytes->size();
        uint16_t crc16 = FirmwareUpdater::ComputeChecksum(file_bytes);
        int update_percent = 0;

        if (flasher){
            qDebug()<<"flasher true ";
        }

        flash_buffer[0] = REPORT_ID_FIRMWARE;
        flash_buffer[1] = 0;
        flash_buffer[2] = 0;
        flash_buffer[3] = 0;
        flash_buffer[4] = (uint8_t)(length & 0xFF);
        flash_buffer[5] = (uint8_t)(length >> 8);
        flash_buffer[6] = (uint8_t)(crc16 & 0xFF);
        flash_buffer[7] = (uint8_t)(crc16 >> 8);

        hid_write(flasher, flash_buffer, BUFFSIZE);

        int res = 0;
        uint8_t buffer[BUFFSIZE]={0};
        while (clock() < millis + 40000)
        {
            if (flasher){
                res=hid_read_timeout(flasher, buffer, BUFFSIZE,5000);  // ?
                if (res < 0) {
                    hid_close(flasher);
                    flasher=nullptr;
                    break;
                } else {
                    if (buffer[0] == REPORT_ID_FIRMWARE) {
                        memset(flasher_device_buffer, 0, BUFFSIZE);
                        memcpy(flasher_device_buffer, buffer, BUFFSIZE);
                    }
                }
            }

            if (flasher_device_buffer[0] == REPORT_ID_FIRMWARE)
            {
                uint16_t cnt = (uint16_t)(flasher_device_buffer[1] << 8 | flasher_device_buffer[2]);
                if ((cnt & 0xF000) == 0xF000)  // status packet
                {
                    if (cnt == 0xF001)  // firmware size error
                    {
                        hid_close(flasher);
                        emit flashStatus(SIZE_ERROR, update_percent);
                        break;
                    }
                    else if (cnt == 0xF002) // CRC error
                    {
                        hid_close(flasher);
                        emit flashStatus(CRC_ERROR, update_percent);
                        break;
                    }
                    else if (cnt == 0xF003) // flash erase error
                    {
                        hid_close(flasher);
                        emit flashStatus(ERASE_ERROR, update_percent);
                        break;
                    }
                    else if (cnt == 0xF000) // OK
                    {
                        hid_close(flasher);
                        emit flashStatus(FINISHED, update_percent);
                        break;
                    }
                }
                else
                {
                    qDebug()<<"Firmware packet requested:"<<cnt;

                    flash_buffer[0] = REPORT_ID_FIRMWARE;
                    flash_buffer[1] = (uint8_t)(cnt >> 8);
                    flash_buffer[2] = (uint8_t)(cnt & 0xFF);
                    flash_buffer[3] = 0;

                    if (cnt * 60 < file_bytes->size())
                    {
                        memcpy(flash_buffer +4, file_bytes->constData() + (cnt - 1) * 60, 60);
                        update_percent = ((cnt - 1) * 60 * 100 / file_bytes->size());
                        hid_write(flasher, flash_buffer, 64);
                        emit flashStatus(IN_PROCESS, update_percent);

                        qDebug()<<"Firmware packet sent:"<<cnt;
                    }
                    else
                    {
                        memcpy(flash_buffer +4, file_bytes->constData() + (cnt - 1) * 60, file_bytes->size() - (cnt - 1) * 60);     // file_bytes->size() для 32 и 64 бит одинаков?
                        update_percent = 0;
                        hid_write(flasher, flash_buffer, 64);
                        emit flashStatus(IN_PROCESS, update_percent);

                        qDebug()<<"Firmware packet sent:"<<cnt;
                    }
                }
            }
        }
    }
}
