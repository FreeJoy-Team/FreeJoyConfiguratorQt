#include <QThread>
#include <QDebug>
#include <QElapsedTimer>
#include "hiddevice.h"
#include "hidapi.h"
#include "common_defines.h"
#include "firmwareupdater.h"

#define VID 0x0483  //0x0483

void HidDevice::processData()
{
    QElapsedTimer timer;
    timer.start();

    int res = 0;
    bool change = false;
    bool no_device_sent = false;
    flasher_ = nullptr;
    QList<hid_device_info*> tmp_HidDevicesAdrList;
    hid_device_info* hid_dev_info;
    QStringList str_list;
    uint8_t buffer[BUFFSIZE]={0};

    current_work_ = REPORT_ID_JOY;

    while (is_finish_ == false)
    {
        // check connected devices
        if (!change)
        {
            timer.start();
            change = true;
        }
        else if (change && timer.elapsed() > 800)   // change is always true
        {
            hid_dev_info = hid_enumerate(VID, 0x0);
            if (!hid_dev_info && no_device_sent == false)
            {
                str_list.clear();
                HidDevicesAdrList.clear();
                emit hidDeviceList(&str_list);
                no_device_sent = true;
            }

            while(hid_dev_info)
            {
                if(QString::fromWCharArray(hid_dev_info->product_string) == "FreeJoy Flasher"){
                    if (!flasher_){
                        qDebug()<<"processData - Flasher found";
                        flasher_ = hid_dev_info;
                        emit flasherFound(true);
                    }
                    flasher_ = hid_dev_info;
                    hid_dev_info = hid_dev_info->next;
                    if (current_work_ == REPORT_ID_FIRMWARE)    // опаять дерьма накодил
                    {
                        FlashFirmwareToDevice();
                        current_work_ = REPORT_ID_JOY;
                    }
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

        // no device
        if (!handle_read)
        {
            if (HidDevicesAdrList.size()){          // ?
                handle_read = hid_open(VID, HidDevicesAdrList[0]->product_id,nullptr);
            }
            if (!handle_read) {
                emit putDisconnectedDeviceInfo();
                //hid_free_enumeration(hid_dev_info);
                QThread::msleep(300);
            } else {
                emit putConnectedDeviceInfo();
            }
        }
        // device connected
        if (handle_read)
        {
            // read joy report
            if (current_work_ == REPORT_ID_JOY)
            {
                res=hid_read_timeout(handle_read, buffer, BUFFSIZE,10000);         // 10000?
                if (res < 0) {
                    hid_close(handle_read);
                    handle_read=nullptr;
                } else {
                    if (buffer[0] == REPORT_ID_JOY) {   // перестраховка
                        memset(device_buffer_, 0, BUFFSIZE);
                        memcpy(device_buffer_, buffer, BUFFSIZE);
                        emit putGamepadPacket(device_buffer_);      // можно и не передавать а тут записывать!!!
                                            // и здесь же сделать задержку на обновление

                        //QThread::msleep(5);            // хз почему даже 5мс тормозит обновление интерфейса(или отправку сигнала?) на ~100мс
                    }
                }
            }
            // read config from device
            else if (current_work_ == REPORT_ID_CONFIG_IN)
            {
                ReadConfigFromDevice(buffer);
            }
            // write config to device
            else if (current_work_ == REPORT_ID_CONFIG_OUT)
            {
                WriteConfigToDevice(buffer);
            }
//            else if (current_work_ == REPORT_ID_FIRMWARE)
//            {
//                EnterToFlashMode();
//            }
        }
    }
    hid_free_enumeration(hid_dev_info);            // ????
}

// stop processData, close app
void HidDevice::SetIsFinish(bool is_finish)
{
    is_finish_ = is_finish;
}

// read config
void HidDevice::ReadConfigFromDevice(uint8_t *buffer)
{
    QElapsedTimer timer;
    timer.start();
    int res = 0;
    qint64 start_time = 0;
    qint64 resend_time = 0;
    int report_count = 0;
    uint8_t config_request_buffer[2] = {REPORT_ID_CONFIG_IN, 1};

    start_time = timer.elapsed();
    resend_time = timer.elapsed();
    hid_write(handle_read, config_request_buffer, 2);

    while (timer.elapsed() < start_time + 2000)
    {
        if (handle_read)    // перестаховка
        {
            res=hid_read_timeout(handle_read, buffer, BUFFSIZE,100);
            if (res < 0) {
                hid_close(handle_read);
                handle_read=nullptr;
            }
            else
            {
                if (buffer[0] == REPORT_ID_CONFIG_IN)
                {
                    if (buffer[1] == config_request_buffer[1])
                    {
                        gEnv.pDeviceConfig->config = report_convert->GetConfigFromDevice(buffer);
                        config_request_buffer[1] += 1;
                        hid_write(handle_read, config_request_buffer, 2);
                        report_count++;

                        if (config_request_buffer[1] > CONFIG_COUNT)
                        {
                            break;
                        }
                    }
                }
                else if (config_request_buffer[1] < 2 && (resend_time + 250 - timer.elapsed()) <= 0) // for first packet
                {
                    qDebug() << "Resend activated";
                    config_request_buffer[1] = 1;
                    resend_time = timer.elapsed();
                    hid_write(handle_read, config_request_buffer, 2);
                }
            }
        } else {    // перестаховка
            current_work_ = REPORT_ID_JOY;
            emit configReceived(false);
            break;
        }
    }
    qDebug()<<"read report_count ="<<report_count<<"/"<<CONFIG_COUNT;
    if (report_count == CONFIG_COUNT) {
        current_work_ = REPORT_ID_JOY;
        emit configReceived(true);
    } else {
        current_work_ = REPORT_ID_JOY;
        emit configReceived(false);
    }
}

// write config
void HidDevice::WriteConfigToDevice(uint8_t *buffer)
{
    QElapsedTimer timer;
    timer.start();
    int res = 0;
    qint64 start_time = 0;
    qint64 resend_time = 0;
    int report_count = 0;
    uint8_t config_out_buffer[BUFFSIZE] = {REPORT_ID_CONFIG_OUT, 0};

    start_time = timer.elapsed();
    resend_time = timer.elapsed();
    hid_write(handle_read, config_out_buffer, BUFFSIZE);

    while (timer.elapsed() < start_time + 2000)
    {
        if (handle_read)    // перестаховка
        {
            res=hid_read_timeout(handle_read, buffer, BUFFSIZE,100);
            if (res < 0) {
                hid_close(handle_read);
                handle_read=nullptr;
            }
            else
            {
                if (buffer[0] == REPORT_ID_CONFIG_OUT)
                {
                    if (buffer[1] == config_out_buffer[1] + 1)
                    {
                        config_out_buffer[1] += 1;
                        std::vector<uint8_t> tmp_buf = report_convert->SendConfigToDevice(config_out_buffer[1]);
                        //memcpy((uint8_t*)(config_buffer), tmp, BUFFSIZE);
                        for (int i = 2; i < 64; i++)
                        {                                       // какой пиздец
                            config_out_buffer[i] = tmp_buf[i];
                        }

                        hid_write(handle_read, config_out_buffer, BUFFSIZE);
                        report_count++;

                        if (buffer[1] == CONFIG_COUNT){
                            break;
                        }
                    }
                }
                else if (config_out_buffer[1] == 0 && (resend_time + 250 - timer.elapsed()) <= 0) // for first packet
                {
                    qDebug() << "Resend activated";
                    resend_time = timer.elapsed();
                    hid_write(handle_read, config_out_buffer, BUFFSIZE);
                }
            }
        } else {    // перестаховка
            current_work_ = REPORT_ID_JOY;
            emit configSent(false);
            break;
        }
    }
    qDebug()<<"write report_count ="<<report_count<<"/"<<CONFIG_COUNT;
    if (report_count == CONFIG_COUNT) {
        current_work_ = REPORT_ID_JOY;
        emit configSent(true);
    } else {
        current_work_ = REPORT_ID_JOY;
        emit configSent(false);
    }
}

// flash firmware
void HidDevice::FlashFirmwareToDevice()
{
    qDebug()<<"flash size = "<<firmware_->size();
    if(flasher_)
    {
        hid_device* flasher = hid_open(VID, flasher_->product_id, flasher_->serial_number);;
        qint64 millis;
        QElapsedTimer time;
        time.start();
        millis = time.elapsed();
        uint8_t flash_buffer[BUFFSIZE]{};
        uint8_t flasher_device_buffer[BUFFSIZE]{};
        uint16_t length = (uint16_t)firmware_->size();
        uint16_t crc16 = FirmwareUpdater::ComputeChecksum(firmware_);
        int update_percent = 0;

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
        while (time.elapsed() < millis + 30000) // 30 сек на прошивку
        {
            if (flasher){
                res=hid_read_timeout(flasher, buffer, BUFFSIZE,5000);  // ?
                if (res < 0) {
                    hid_close(flasher);
                    flasher=nullptr;
                    //flasher_ = nullptr;
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
                    //qDebug()<<"ERROR";
                    if (cnt == 0xF001)  // firmware size error
                    {
                        hid_close(flasher);
                        //flasher_ = nullptr;
                        emit flashStatus(SIZE_ERROR, update_percent);
                        break;
                    }
                    else if (cnt == 0xF002) // CRC error
                    {
                        hid_close(flasher);
                        //flasher_ = nullptr;
                        emit flashStatus(CRC_ERROR, update_percent);
                        break;
                    }
                    else if (cnt == 0xF003) // flash erase error
                    {
                        hid_close(flasher);
                        //flasher_ = nullptr;
                        emit flashStatus(ERASE_ERROR, update_percent);
                        break;
                    }
                    else if (cnt == 0xF000) // OK
                    {
                        hid_close(flasher);
                        //flasher_ = nullptr;
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

                    if (cnt * 60 < firmware_->size())
                    {
                        memcpy(flash_buffer +4, firmware_->constData() + (cnt - 1) * 60, 60);
                        update_percent = ((cnt - 1) * 60 * 100 / firmware_->size());
                        hid_write(flasher, flash_buffer, 64);
                        emit flashStatus(IN_PROCESS, update_percent);

                        qDebug()<<"Firmware packet sent:"<<cnt;
                    }
                    else
                    {
                        memcpy(flash_buffer +4, firmware_->constData() + (cnt - 1) * 60, firmware_->size() - (cnt - 1) * 60);     // file_bytes->size() для 32 и 64 бит одинаков?
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

// button "get config" clicked
void HidDevice::GetConfigFromDevice()
{
    current_work_ = REPORT_ID_CONFIG_IN;
}
// button "send config" clicked
void HidDevice::SendConfigToDevice()
{
    current_work_ = REPORT_ID_CONFIG_OUT;
}
// button "flash firmware" clicked
void HidDevice::FlashFirmware(const QByteArray* firmware)
{
    firmware_ = firmware;
    current_work_ = REPORT_ID_FIRMWARE;
}

bool HidDevice::EnterToFlashMode()
{
    if(handle_read)
    {
        flasher_ = nullptr;
        qint64 millis;
        QElapsedTimer time;
        time.start();
        millis = time.elapsed();
        qDebug()<<"before hid_write";
        uint8_t config_buffer[64] = {REPORT_ID_FIRMWARE,'b','o','o','t','l','o','a','d','e','r',' ','r','u','n'};
        hid_write(handle_read, config_buffer, 64);
        qDebug()<<"after hid_write";
        while (time.elapsed() < millis + 1000)
        {
            if (flasher_){
                return true;
            }
        }
    }
    return false;
}


// another device selected in comboBox
void HidDevice::SetSelectedDevice(int device_number)        // заблочить сигнал до запуска, скорее всего крашит из-за разных потоков
{                                                           // только в винде. решил костылём в hidapi.c Qwe();
    if (device_number < 0){
        //device_number = 0;
        return;
    } else if (device_number > HidDevicesAdrList.size() - 1){
        device_number = HidDevicesAdrList.size() - 1;
    }
    selected_device_ = device_number; 
    qDebug()<<"HID open start";
    qDebug()<<device_number + 1<<"devices connected";
        // возможно не стоит здесь открывать, оставить изменение selected_device_, а открытие в processData()
    handle_read = hid_open(VID, HidDevicesAdrList[selected_device_]->product_id, HidDevicesAdrList[selected_device_]->serial_number);
//    if (!handle_read) {
//        emit putDisconnectedDeviceInfo();
//        //hid_free_enumeration(hid_dev_info);
//    } else {
//        emit putConnectedDeviceInfo();
//    }
#ifdef _WIN32
    qDebug()<<"Unsuccessful attempts ="<<Qwe();
#endif
    qDebug()<<"HID opened";
}
