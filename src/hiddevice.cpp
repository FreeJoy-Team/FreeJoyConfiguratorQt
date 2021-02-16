#include <QThread>
#include <QDebug>
#include <QElapsedTimer>
#include <QMap>
#include "hiddevice.h"
#include "hidapi.h"
#include "common_defines.h"
#include "firmwareupdater.h"

#define REPORT_ID_FLASH 4
#define OLD_FIRMWARE_VID 0x0483

void HidDevice::processData()
{
    QElapsedTimer timer;
    timer.start();

    int res = 0;
    int oldSelectedDevice = -1;
    bool deviceCountChanged = false;
    bool change = false;
    bool noDeviceSent = false;
    m_flasher = nullptr;
    QList<QPair<bool, hid_device_info*>> tmp_HidList;

    hid_device_info *hidDevInfo;
    uint8_t buffer[BUFFERSIZE]{};

    m_currentWork = REPORT_ID_PARAM;

    while (m_isFinish == false)
    {
        // check connected devices
        if (!change) {
            timer.start();
            change = true;
        }
        else if (timer.elapsed() > 800)
        {
            hidDevInfo = hid_enumerate(0x0, 0x0);
            if (!hidDevInfo && (noDeviceSent == false || m_flasher))
            {
                m_deviceNames.clear();
                m_HidDevicesAdrList.clear();
                emit hidDeviceList(m_deviceNames);
                noDeviceSent = true;
                m_flasher = nullptr;
                emit flasherFound(false);
            }
            while(hidDevInfo) {
                // flasher search
                if(QString::fromWCharArray(hidDevInfo->product_string) == "FreeJoy Flasher") {
                    if (!m_flasher) {
                        m_flasher = hidDevInfo;
                        m_HidDevicesAdrList.clear();
                        m_deviceNames.clear();
                        m_deviceNames.append(qMakePair(false, QString("FreeJoy Flasher")));
                        emit hidDeviceList(m_deviceNames);
                        emit flasherConnected();
                        emit flasherFound(true);
                    }
                    hidDevInfo = hidDevInfo->next;

                    if (m_currentWork == REPORT_ID_FIRMWARE) {
                        flashFirmwareToDevice();
                        // flash done
                        m_flasher = nullptr;
                        m_currentWork = REPORT_ID_PARAM;
                        m_deviceNames.clear();
                        emit hidDeviceList(m_deviceNames);
                        emit deviceConnected();
                        hid_free_enumeration(hidDevInfo);   // hz nado li
                        QThread::msleep(300);
                        break;
                    }
                    break;//continue
                }

                // add devices ptr to list
                if (QString::fromWCharArray(hidDevInfo->manufacturer_string) == "FreeJoy" && hidDevInfo->interface_number == 1) {
                    tmp_HidList.append(qMakePair(false, hidDevInfo));
                } else if (hidDevInfo->vendor_id == OLD_FIRMWARE_VID && QString::fromWCharArray(hidDevInfo->manufacturer_string) != "FreeJoy" &&
                           hidDevInfo->interface_number <= 0)
                {
                    tmp_HidList.append(qMakePair(true, hidDevInfo));
                }
                hidDevInfo = hidDevInfo->next;
                // all devices added
                if (!hidDevInfo) {
                    // old devices count != new devices count
                    if (m_HidDevicesAdrList.size() != tmp_HidList.size()) {
                        m_HidDevicesAdrList.clear();
                        m_deviceNames.clear();
                        noDeviceSent = false;
                        for (int i = 0; i < tmp_HidList.size(); ++i) {
                            qDebug()<<tmp_HidList.size();
                            m_HidDevicesAdrList.append(tmp_HidList[i].second);
                            m_deviceNames.append(qMakePair(tmp_HidList[i].first, QString::fromWCharArray(tmp_HidList[i].second->product_string)));
                        }
                        emit hidDeviceList(m_deviceNames);
                        tmp_HidList.clear();
                        deviceCountChanged = true;
                        if (m_flasher) {
                            m_flasher = nullptr;
                            emit flasherFound(false);
                        }
                    }
                }
            }
            tmp_HidList.clear();
            //hid_free_enumeration(hidDevInfo);
            change = false;
        }
        if (!m_flasher) {
            // all devices disconnected
            if (m_HidDevicesAdrList.empty()) {
                emit deviceDisconnected();
                oldSelectedDevice = m_selectedDevice = -1;
                QThread::msleep(600);
                continue;
            }
            // open HID
            if (m_selectedDevice != oldSelectedDevice || (deviceCountChanged && m_selectedDevice >= 0)) {
                if (!m_HidDevicesAdrList.empty() && m_selectedDevice < m_HidDevicesAdrList.size()) {
                    const ushort vid = m_HidDevicesAdrList[m_selectedDevice]->vendor_id;
                    const ushort pid = m_HidDevicesAdrList[m_selectedDevice]->product_id;
                    const wchar_t *serNum = m_HidDevicesAdrList[m_selectedDevice]->serial_number;
                    const char *path = m_HidDevicesAdrList[m_selectedDevice]->path;

                    qDebug().nospace()<<"Open HID device №"<<m_selectedDevice + 1
                                     <<". VID"<<QString::number(vid, 16).toUpper().rightJustified(4, '0')
                                    <<", PID"<<QString::number(pid, 16).toUpper().rightJustified(4, '0')
                                   <<", Serial number "<<QString::fromWCharArray(serNum);
                    m_paramsRead = hid_open_path(path);

                    if (m_paramsRead) {
                        emit deviceConnected();
                        oldSelectedDevice = m_selectedDevice;
                        deviceCountChanged = false;
                        // for old firmware
                        if (m_deviceNames[m_selectedDevice].first) {
                            m_oldFirmwareSelected = true;
                        } else {
                            m_oldFirmwareSelected = false;
                        }
                        qDebug()<<"HID opened, connected devices ="<<m_HidDevicesAdrList.size();
                    }
                }
            }
            // device connected
            if (m_paramsRead) {
                // read joy report
                if (m_currentWork == REPORT_ID_PARAM && !m_oldFirmwareSelected) {
                    res=hid_read_timeout(m_paramsRead, buffer, BUFFERSIZE,10000);
                    if (res < 0) {
                        hid_close(m_paramsRead);
                        m_paramsRead=nullptr;
                    } else {
                        if (buffer[0] == REPORT_ID_PARAM) {   // перестраховка
                            memset(m_deviceBuffer, 0, BUFFERSIZE);
                            memcpy(m_deviceBuffer, buffer, BUFFERSIZE);
                            emit paramsPacketReceived(m_deviceBuffer);
                        }
                    }
                }
                // read config from device
                else if (m_currentWork == REPORT_ID_CONFIG_IN) {
                    readConfigFromDevice(buffer);
                    #ifdef QT_DEBUG
                    gEnv.readFinished = true;
                    #endif
                }
                // write config to device
                else if (m_currentWork == REPORT_ID_CONFIG_OUT) { ////////////// ХУЁВО ПАШЕТ редко
                    writeConfigToDevice(buffer);
                    // disconnect all devices
                    emit deviceDisconnected();
                    m_deviceNames.clear();
                    emit hidDeviceList(m_deviceNames);
                    m_HidDevicesAdrList.clear();
                    oldSelectedDevice = m_selectedDevice = -1;
                    #ifdef QT_DEBUG
                    gEnv.readFinished = false;
                    #endif
                    QThread::msleep(200);
                }
            }
        }
    }
    //hid_free_enumeration(hidDevInfo);
}


// another device selected in comboBox
void HidDevice::setSelectedDevice(int deviceNumber)
{
    if (deviceNumber < 0 || m_flasher){
        return;
    } else if (deviceNumber > m_HidDevicesAdrList.size() - 1){
        deviceNumber = m_HidDevicesAdrList.size() - 1;
    }
    m_selectedDevice = deviceNumber;
//#ifdef _WIN32
//    qDebug()<<"Unsuccessful serial number attempts ="<<GetSerialNumberAttemption()<<"(not a error)";
//    qDebug()<<"Unsuccessful product string attempts ="<<GetProductStrAttemption()<<"(not a error)";
//#endif
}

// stop processData, close app
void HidDevice::setIsFinish(bool is_finish)
{
    m_isFinish = is_finish;
}

// read config
void HidDevice::readConfigFromDevice(uint8_t *buffer)
{
    QElapsedTimer timer;
    timer.start();
    int res = 0;
    qint64 start_time = 0;
    qint64 resend_time = 0;
    int report_count = 0;
    uint8_t config_request_buffer[2] = {REPORT_ID_CONFIG_IN, 1};

    uint8_t cfg_count = sizeof(dev_config_t) / 62;
    uint8_t last_cfg_size = sizeof(dev_config_t) % 62;
    if (last_cfg_size > 0)
    {
        cfg_count++;
    }

    start_time = timer.elapsed();
    resend_time = start_time;
    hid_write(m_paramsRead, config_request_buffer, 2);

    while (timer.elapsed() < start_time + 2000)
    {
        if (m_paramsRead)    // перестаховка
        {
            res=hid_read_timeout(m_paramsRead, buffer, BUFFERSIZE,100);
            if (res < 0) {
                hid_close(m_paramsRead);
                m_paramsRead=nullptr;
            }
            else
            {
                if (buffer[0] == REPORT_ID_CONFIG_IN)
                {
                    if (buffer[1] == config_request_buffer[1])
                    {
                        //gEnv.pDeviceConfig->config = m_reportConvert->getConfigFromDevice(buffer);
                        ReportConverter::getConfigFromDevice(buffer);
                        config_request_buffer[1] += 1;
                        hid_write(m_paramsRead, config_request_buffer, 2);
                        report_count++;
                        qDebug()<<"Config"<<report_count<<"received";
                        if (config_request_buffer[1] > cfg_count)
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
                    hid_write(m_paramsRead, config_request_buffer, 2);
                }
            }
        } else {    // перестаховка
            m_currentWork = REPORT_ID_PARAM;
            emit configReceived(false);
            break;
        }
    }
    qDebug()<<"Read report_count ="<<report_count<<"/"<<cfg_count;
    if (report_count == cfg_count){
        qDebug() << "All config received";
    } else {
        qDebug() << "ERROR, not all config received";
    }

    if (report_count == cfg_count) {
        m_currentWork = REPORT_ID_PARAM;
        emit configReceived(true);
    } else {
        m_currentWork = REPORT_ID_PARAM;
        emit configReceived(false);
    }
}

// write config
void HidDevice::writeConfigToDevice(uint8_t *buffer)
{
    QElapsedTimer timer;
    timer.start();
    int res = 0;
    qint64 startTime = 0;
    qint64 resendTime = 0;
    int reportCount = 0;
    uint8_t configOutBuf[BUFFERSIZE] = {REPORT_ID_CONFIG_OUT, 0};

    uint8_t cfg_count = sizeof(dev_config_t) / 62;
    uint8_t last_cfg_size = sizeof(dev_config_t) % 62;
    if (last_cfg_size > 0)
    {
        cfg_count++;
    }

    startTime = timer.elapsed();
    resendTime = startTime;
    hid_write(m_paramsRead, configOutBuf, BUFFERSIZE);

    while (timer.elapsed() < startTime + 2000)
    {
        if (m_paramsRead)    // перестаховка
        {
            res = hid_read_timeout(m_paramsRead, buffer, BUFFERSIZE,100);
            if (res < 0) {
                hid_close(m_paramsRead);
                m_paramsRead=nullptr;
            }
            else
            {
                if (buffer[0] == REPORT_ID_CONFIG_OUT)
                {
                    if (buffer[1] == configOutBuf[1] + 1)
                    {
                        configOutBuf[1] += 1;
                        memcpy(configOutBuf, ReportConverter::sendConfigToDevice(configOutBuf[1]), BUFFERSIZE);

                        hid_write(m_paramsRead, configOutBuf, BUFFERSIZE);
                        reportCount++;
                        qDebug()<<"Config"<<reportCount<<"sent";

                        if (buffer[1] == cfg_count){
                            break;
                        }
                    }
                }
                else if (configOutBuf[1] == 0 && (resendTime + 250 - timer.elapsed()) <= 0) // for first packet
                {
                    qDebug() << "Resend activated";
                    resendTime = timer.elapsed();
                    hid_write(m_paramsRead, configOutBuf, BUFFERSIZE);
                }
            }
        } else {    // перестаховка
            m_currentWork = REPORT_ID_PARAM;
            emit configSent(false);
            break;
        }
    }
    qDebug()<<"Write report_count ="<<reportCount<<"/"<<cfg_count;
    if (reportCount == cfg_count) {
        qDebug() << "All config sent";
        while (timer.elapsed() < startTime + 2000) {
            if (m_paramsRead) {
                res = hid_read_timeout(m_paramsRead, buffer, BUFFERSIZE,100);
                if (res < 0) {
                    hid_close(m_paramsRead);
                    m_paramsRead=nullptr;
                } else if (buffer[1] == 0xFE) {
                    qDebug() << "ERROR! Version doesnt match";
                    m_currentWork = REPORT_ID_PARAM;
                    emit configSent(false);
                    return;
                }
            }
        }
        m_currentWork = REPORT_ID_PARAM;
        emit configSent(true);
    } else {
        qDebug() << "ERROR, not all config sent";
        m_currentWork = REPORT_ID_PARAM;
        emit configSent(false);
    }
}

// flash firmware
void HidDevice::flashFirmwareToDevice()
{
    qDebug()<<"flash size = "<<m_firmware->size();
    if(m_flasher)
    {
        //hid_device* flasher = hid_open(FLASHER_VID, m_flasher->product_id, m_flasher->serial_number);
        hid_device* flasher = hid_open_path(m_flasher->path);
        qint64 millis;
        QElapsedTimer time;
        time.start();
        millis = time.elapsed();
        uint8_t flash_buffer[BUFFERSIZE]{};
        uint8_t flasher_device_buffer[BUFFERSIZE]{};
        uint16_t length = (uint16_t)m_firmware->size();
        uint16_t crc16 = FirmwareUpdater::computeChecksum(m_firmware);
        int update_percent = 0;

        flash_buffer[0] = REPORT_ID_FLASH;
        flash_buffer[1] = 0;
        flash_buffer[2] = 0;
        flash_buffer[3] = 0;
        flash_buffer[4] = (uint8_t)(length & 0xFF);
        flash_buffer[5] = (uint8_t)(length >> 8);
        flash_buffer[6] = (uint8_t)(crc16 & 0xFF);
        flash_buffer[7] = (uint8_t)(crc16 >> 8);

        hid_write(flasher, flash_buffer, BUFFERSIZE);

        int res = 0;
        uint8_t buffer[BUFFERSIZE]{};
        while (time.elapsed() < millis + 30000) // 30 сек на прошивку
        {
            if (flasher){
                res=hid_read_timeout(flasher, buffer, BUFFERSIZE,5000);  // ?
                if (res < 0) {
                    hid_close(flasher);
                    flasher=nullptr;
                    //flasher_ = nullptr;
                    return;
                } else {
                    if (buffer[0] == REPORT_ID_FLASH) {
                        memset(flasher_device_buffer, 0, BUFFERSIZE);
                        memcpy(flasher_device_buffer, buffer, BUFFERSIZE);
                    }
                }
            }

            if (flasher_device_buffer[0] == REPORT_ID_FLASH)
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
                        return;
                    }
                    else if (cnt == 0xF002) // CRC error
                    {
                        hid_close(flasher);
                        //flasher_ = nullptr;
                        emit flashStatus(CRC_ERROR, update_percent);
                        return;
                    }
                    else if (cnt == 0xF003) // flash erase error
                    {
                        hid_close(flasher);
                        //flasher_ = nullptr;
                        emit flashStatus(ERASE_ERROR, update_percent);
                        return;
                    }
                    else if (cnt == 0xF000) // OK
                    {
                        hid_close(flasher);
                        //flasher_ = nullptr;
                        emit flashStatus(FINISHED, update_percent);
                        return;
                    }
                }
                else
                {
                    qDebug()<<"Firmware packet requested:"<<cnt;

                    flash_buffer[0] = REPORT_ID_FLASH;
                    flash_buffer[1] = (uint8_t)(cnt >> 8);
                    flash_buffer[2] = (uint8_t)(cnt & 0xFF);
                    flash_buffer[3] = 0;

                    if (cnt * 60 < m_firmware->size())
                    {
                        memcpy(flash_buffer +4, m_firmware->constData() + (cnt - 1) * 60, 60);
                        update_percent = ((cnt - 1) * 60 * 100 / m_firmware->size());
                        hid_write(flasher, flash_buffer, 64);
                        emit flashStatus(IN_PROCESS, update_percent);

                        qDebug()<<"Firmware packet sent:"<<cnt;
                    }
                    else
                    {
                        memcpy(flash_buffer +4, m_firmware->constData() + (cnt - 1) * 60, m_firmware->size() - (cnt - 1) * 60);
                        update_percent = 0;
                        hid_write(flasher, flash_buffer, 64);
                        emit flashStatus(IN_PROCESS, update_percent);

                        qDebug()<<"Firmware packet sent:"<<cnt;
                    }
                }
            }
        }
        hid_close(flasher);
        //flasher_ = nullptr;
        emit flashStatus(666, update_percent);
    }
}

// button "get config" clicked
void HidDevice::getConfigFromDevice()
{
    m_currentWork = REPORT_ID_CONFIG_IN;
}
// button "send config" clicked
void HidDevice::sendConfigToDevice()
{
    m_currentWork = REPORT_ID_CONFIG_OUT;
}
// button "flash firmware" clicked
void HidDevice::flashFirmware(const QByteArray* firmware)
{
    m_firmware = firmware;
    m_currentWork = REPORT_ID_FIRMWARE;
}

bool HidDevice::enterToFlashMode()
{
    if(m_paramsRead)
    {
        m_flasher = nullptr;
        qint64 millis;
        QElapsedTimer time;
        time.start();
        millis = time.elapsed();
        uint8_t report = REPORT_ID_FIRMWARE;
        if (m_oldFirmwareSelected) {
            report = 4;
        }
        uint8_t config_buffer[64] = {report,'b','o','o','t','l','o','a','d','e','r',' ','r','u','n'};
        hid_write(m_paramsRead, config_buffer, 64);
        while (time.elapsed() < millis + 1000)
        {
            if (m_flasher){
                return true;
            }
        }
    }
    return false;
}
