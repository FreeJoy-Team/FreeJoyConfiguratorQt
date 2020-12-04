#ifndef REPORTCONVERTER_H
#define REPORTCONVERTER_H

#include "deviceconfig.h"
#include <vector>

#include "deviceconfig.h"
#include "global.h"

class ReportConverter
{
public:
    void gamepadReport(uint8_t *hid_buffer);

    dev_config_t getConfigFromDevice(uint8_t *hid_buffer);
    std::vector<uint8_t> sendConfigToDevice(uint8_t request_config_number); // ****
};

#endif // REPORTCONVERTER_H
