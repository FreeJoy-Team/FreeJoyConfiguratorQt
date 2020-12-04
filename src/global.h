#ifndef GLOBAL_H
#define GLOBAL_H

#include <QElapsedTimer> //?
#include <QString>
//QT_BEGIN_NAMESPACE
//class QString;
//class QElapsedTimer;
//QT_END_NAMESPACE

class DeviceConfig;
class QSettings;
class DebugWindow; /////////////////////
//class QElapsedTimer;    //?
//! CryEngine style global environment
//!	Global environment. Contains pointers to all global often needed interfaces.
//!	This is a faster way to get interface pointer then calling ISystem interface to retrieve one.
struct GlobalEnvironment
{
    QElapsedTimer *pApp_start_time = nullptr;
    DeviceConfig *pDeviceConfig = nullptr;
    QSettings *pAppSettings = nullptr;
    QString *pAppVersion = nullptr; // тупо, надо в дефайне?
    DebugWindow *pDebugWindow = nullptr;

    // thread
};
extern GlobalEnvironment gEnv;

// flasher
enum {
    FINISHED = 0,
    SIZE_ERROR,
    CRC_ERROR,
    ERASE_ERROR,

    IN_PROCESS = 99,
};

struct deviceEnum_guiName_t
{
    int deviceEnumIndex;
    QString guiName;
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
