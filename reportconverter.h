#ifndef REPORTCONVERTER_H
#define REPORTCONVERTER_H

#include "deviceconfig.h"
#include <vector>

#include "global.h"
#include "deviceconfig.h"


class ReportConverter
{
public:
    void GamepadReport(uint8_t * hid_buffer);

    dev_config_t GetConfigFromDevice(uint8_t * hid_buffer);
    //std::vector<uint8_t>
    std::vector<uint8_t> SendConfigToDevice(uint8_t request_config_number);        // ****
};

#endif // REPORTCONVERTER_H
