#ifndef REPORTCONVERTER_H
#define REPORTCONVERTER_H

#include "stdint.h"

#define BUFFERSIZE 64

class ReportConverter
{
public:
    static void paramReport(uint8_t *paramsBuffer);

    static void getConfigFromDevice(uint8_t *hidBuffer);
    static uint8_t *sendConfigToDevice(uint8_t requestConfigNumber);

private:
    static uint8_t buffer[BUFFERSIZE];
};

#endif // REPORTCONVERTER_H
