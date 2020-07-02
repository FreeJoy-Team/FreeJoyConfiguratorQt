#ifndef CONFIGUICONVERTER_H
#define CONFIGUICONVERTER_H

#include <vector>

#include "global.h"
#include "deviceconfig.h"

class Converter
{
public:
    Converter();

    static int EnumToIndex(const int device_emun, const std::vector<deviceEnum_guiName_t>& list);
};

#endif // CONFIGUICONVERTER_H
