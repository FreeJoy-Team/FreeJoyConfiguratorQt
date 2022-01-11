#ifndef REPORTCONVERTER_H
#define REPORTCONVERTER_H

#include "stdint.h"

#define BUFFERSIZE 64

namespace ReportConverter
{
    int paramReport(uint8_t *paramsBuffer);
    void resetReport();

    void getConfigFromDevice(uint8_t *hidBuffer);
    void sendConfigToDevice(uint8_t *hidBuf, uint8_t requestConfigNumber);
}

#endif // REPORTCONVERTER_H
