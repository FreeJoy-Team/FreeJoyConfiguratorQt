#ifndef FIRMWAREUPDATER_H
#define FIRMWAREUPDATER_H

#include <QByteArray>

class FirmwareUpdater
{
public:
    FirmwareUpdater();

    static uint16_t computeChecksum(const QByteArray *fileBytes);

private:
    static const uint16_t polynomial = 0xA001;
    static uint16_t table[256];
};

#endif // FIRMWAREUPDATER_H
