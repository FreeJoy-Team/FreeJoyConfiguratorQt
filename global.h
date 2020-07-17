#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <ctime>            // временно

class DeviceConfig;
class QSettings;
//! CryEngine style global environment
//!	Global environment. Contains pointers to all global often needed interfaces.
//!	This is a faster way to get interface pointer then calling ISystem interface to retrieve one.
struct GlobalEnvironment
{
    clock_t*                   pApp_start_time = nullptr;
    DeviceConfig*              pDeviceConfig = nullptr;
    QSettings*                 pAppSettings = nullptr;
    // thread
};
extern GlobalEnvironment gEnv;


struct deviceEnum_guiName_t
{
    int device_enum_index;
    QString gui_name;
};

    //! Remove pointer indirection.
//        ILINE SSystemGlobalEnvironment* operator->()
//        {
//            return this;
//        }
//        ILINE SSystemGlobalEnvironment& operator*()
//        {
//            return *this;
//        }
//        ILINE const bool operator!() const
//        {
//            return false;
//        }
//        ILINE operator bool() const
//        {
//            return true;
//        }


#endif // GLOBAL_H
//#if defined(SYS_ENV_AS_STRUCT)
