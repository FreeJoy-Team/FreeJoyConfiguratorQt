#include "deviceconfig.h"

extern "C" {
#include "stm_main.h"
}

DeviceConfig::DeviceConfig()
{
    config = InitConfig();
    //joyReport = {};
    paramsReport = {};
}

void DeviceConfig::resetConfig()
{
    config = InitConfig();
}
