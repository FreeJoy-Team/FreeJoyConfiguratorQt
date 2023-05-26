#ifndef GLOBAL_H
#define GLOBAL_H

#include <QElapsedTimer> //?
#include <QString>

class DeviceConfig;
class QSettings;
class DebugWindow;
class QTranslator;
//class QElapsedTimer;    //?
//! CryEngine style global environment
//!	Global environment. Contains pointers to all global often needed interfaces.
//!	This is a faster way to get interface pointer then calling ISystem interface to retrieve one.
struct GlobalEnvironment
{
    QElapsedTimer *pApp_start_time = nullptr;
    DeviceConfig *pDeviceConfig = nullptr;
    QSettings *pAppSettings = nullptr;  // нахера?
    DebugWindow *pDebugWindow = nullptr;
    QTranslator *pTranslator = nullptr;

#ifdef QT_DEBUG
bool readFinished = false;
#endif
};
extern GlobalEnvironment gEnv;

struct deviceEnum_guiName_t
{
int deviceEnumIndex;
QString guiName;
};

// flasher
enum {
    FINISHED = 0,
    SIZE_ERROR,
    CRC_ERROR,
    ERASE_ERROR,

    IN_PROCESS = 99,
};

enum Pin {
    PA_0 = 1,
    PA_1,
    PA_2,
    PA_3,
    PA_4,
    PA_5,
    PA_6,
    PA_7,
    PA_8,
    PA_9,
    PA_10,
    PA_15,

    PB_0,
    PB_1,
    PB_3,
    PB_4,
    PB_5,
    PB_6,
    PB_7,
    PB_8,
    PB_9,
    PB_10,
    PB_11,
    PB_12,
    PB_13,
    PB_14,
    PB_15,

    PC_13,
    PC_14,
    PC_15,
};

#endif // GLOBAL_H
