#ifndef CONFIGUICONVERTER_H
#define CONFIGUICONVERTER_H

#include "global.h"
#include "deviceconfig.h"

class Converter
{
public:
    Converter();

    static int EnumToIndex(const int device_emun, const QVector<deviceEnum_guiName_t>& list);
};

#endif // CONFIGUICONVERTER_H
