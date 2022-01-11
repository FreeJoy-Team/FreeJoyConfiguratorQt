#include "reportconverter.h"

#include "deviceconfig.h"
#include "global.h"

namespace ReportConverter
{
    int8_t firmwareCompatible = -1;
//    namespace
//    {
//        int8_t firmwareCompatible = -1;
//    }
}

int ReportConverter::paramReport(uint8_t *paramsBuf)
{
    if (paramsBuf != nullptr) {
        if (paramsBuf[1] == 0) {
            if ((*(uint16_t *)(paramsBuf + 2) & 0xFFF0) == (FIRMWARE_VERSION & 0xFFF0)) {
                firmwareCompatible = 1;
                memcpy((uint8_t *)&(gEnv.pDeviceConfig->paramsReport), paramsBuf + 2, 62); // paramsBuf +2  skip report ids
                return 1;
            } else {
                firmwareCompatible = 0;
            }
        } else if (firmwareCompatible == 1) {
            memcpy((uint8_t *)&(gEnv.pDeviceConfig->paramsReport) + 62, paramsBuf + 2, sizeof(params_report_t) - 62);
            return 1;
        } else if (firmwareCompatible == -1) {
            return -1;
        }
    }
    return 0;
}

void ReportConverter::resetReport()
{
    firmwareCompatible = -1;
    memset(&gEnv.pDeviceConfig->paramsReport, 0, sizeof(params_report_t));
}

void ReportConverter::getConfigFromDevice(uint8_t *hidBuf)
{
    uint8_t cfg_count = sizeof(dev_config_t) / 62;
    uint8_t last_cfg_size = sizeof(dev_config_t) % 62;
    if (last_cfg_size > 0) {
        cfg_count++;
    }

    if (hidBuf[1] == cfg_count && last_cfg_size > 0) {
        memcpy((uint8_t *)&(gEnv.pDeviceConfig->config) + 62*(hidBuf[1] - 1), hidBuf + 2, last_cfg_size);
    } else {
        memcpy((uint8_t *)&(gEnv.pDeviceConfig->config) + 62*(hidBuf[1] - 1), hidBuf + 2, 62);
    }
}

void ReportConverter::sendConfigToDevice(uint8_t *hidBuf, uint8_t requestConfigNumber)
{
    uint8_t cfg_count = sizeof(dev_config_t) / 62;
    uint8_t last_cfg_size = sizeof(dev_config_t) % 62;
    if (last_cfg_size > 0) {
        cfg_count++;
    }

    hidBuf[0] = REPORT_ID_CONFIG_OUT;
    hidBuf[1] = requestConfigNumber;

    if (requestConfigNumber == cfg_count && last_cfg_size > 0) {
        memcpy(&hidBuf[2], (uint8_t *)&(gEnv.pDeviceConfig->config) + 62*(requestConfigNumber - 1), last_cfg_size);
    } else {
        memcpy(&hidBuf[2], (uint8_t *)&(gEnv.pDeviceConfig->config) + 62*(requestConfigNumber - 1), 62);
    }
}
