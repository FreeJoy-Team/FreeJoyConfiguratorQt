#include "deviceconfig.h"

extern "C" {
#include "stm_main.h"
}

DeviceConfig::DeviceConfig()
{
    config = InitConfig();
}
