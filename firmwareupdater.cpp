#include "firmwareupdater.h"

#include <QDebug>
FirmwareUpdater::FirmwareUpdater() {}
// хз зачем мне статик постоянно висящий в памяти
uint16_t FirmwareUpdater::table[256]{};
unsigned short FirmwareUpdater::computeChecksum(const QByteArray *fileBytes)
{
    // generate crc
    uint16_t value;
    uint16_t temp;
    for (uint16_t i = 0; i < sizeof(table) / sizeof(uint16_t); ++i) {
        value = 0;
        temp = i;
        for (uint8_t j = 0; j < 8; ++j) {
            if (((value ^ temp) & 0x0001) != 0) {
                value = (uint16_t)((value >> 1) ^ polynomial);
            } else {
                value >>= 1;
            }
            temp >>= 1;
        }
        table[i] = value;
    }

    // generate checksum
    uint16_t crc = 0;
    for (int i = 0; i < fileBytes->size(); ++i) {
        uint8_t index = (uint8_t)(crc ^ fileBytes->at(i));
        crc = (uint16_t)((crc >> 8) ^ table[index]);
    }
    return crc;
}
