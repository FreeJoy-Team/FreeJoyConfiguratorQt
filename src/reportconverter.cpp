#include <QDebug>
#include <cstring>
#include "reportconverter.h"

uint8_t ReportConverter::buffer[BUFFERSIZE]{};

void ReportConverter::paramReport( uint8_t *paramsBuf)
{
    if (paramsBuf != nullptr) {
        if (paramsBuf[1] == 0) {
            memcpy((uint8_t *)&(gEnv.pDeviceConfig->paramsReport), paramsBuf + 2, 62); // paramsBuf +2  skip report ids
        } else {
            memcpy((uint8_t *)&(gEnv.pDeviceConfig->paramsReport) + 62, paramsBuf + 2, sizeof(params_report_t) - 62);
        }
    }
}


void ReportConverter::getConfigFromDevice(uint8_t *hidBuf)
{
    uint8_t cfg_count = sizeof(dev_config_t) / 62;
    uint8_t last_cfg_size = sizeof(dev_config_t) % 62;
    if (last_cfg_size > 0) {
        cfg_count++;
    }

    if (hidBuf[1] == cfg_count && last_cfg_size > 0) {
        memcpy((uint8_t *)&(gEnv.pDeviceConfig->config) + 62*(hidBuf[1]-1), hidBuf +2, last_cfg_size);
    } else {
        memcpy((uint8_t *)&(gEnv.pDeviceConfig->config) + 62*(hidBuf[1]-1), hidBuf +2, 62);
    }
}

uint8_t *ReportConverter::sendConfigToDevice(uint8_t requestConfigNumber)
{
    uint8_t cfg_count = sizeof(dev_config_t) / 62;
    uint8_t last_cfg_size = sizeof(dev_config_t) % 62;
    if (last_cfg_size > 0) {
        cfg_count++;
    }

    buffer[0] = REPORT_ID_CONFIG_OUT;
    buffer[1] = requestConfigNumber;

    if (requestConfigNumber == cfg_count && last_cfg_size > 0) {
        memcpy(&buffer[2], (uint8_t *)&(gEnv.pDeviceConfig->config) + 62*(requestConfigNumber-1), last_cfg_size);
    } else {
        memcpy(&buffer[2], (uint8_t *)&(gEnv.pDeviceConfig->config) + 62*(requestConfigNumber-1), 62);
    }

    return buffer;
}
