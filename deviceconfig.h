#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include "common_defines.h"
#include "common_types.h"

class DeviceConfig
{
public:
    DeviceConfig();

    void resetConfig();

    dev_config_t config;
    joy_report_t gamepadReport;
};

#endif // DEVICECONFIG_H
