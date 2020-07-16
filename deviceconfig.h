#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include "common_types.h"
#include "common_defines.h"

class DeviceConfig
{
public:
    DeviceConfig();

    dev_config_t config;
    joy_report_t gamepad_report;
};


#endif // DEVICECONFIG_H
