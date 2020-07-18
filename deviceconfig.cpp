#include "deviceconfig.h"

extern "C" {
#include "stm_main.h"
}

DeviceConfig::DeviceConfig()
{
    config = InitConfig();
    gamepad_report = {};        //??
}


void DeviceConfig::ResetConfig()
{
    config = InitConfig();
}
