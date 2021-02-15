#ifndef CONFIGUICONVERTER_H
#define CONFIGUICONVERTER_H

#include "deviceconfig.h"
#include "global.h"

class Converter
{
public:
    Converter();

    static int EnumToIndex(const int deviceEmun, const QVector<deviceEnum_guiName_t> &list);
    static int EnumToIndex(const int deviceEnum, const QVector<int> &list);
};

#endif // CONFIGUICONVERTER_H
