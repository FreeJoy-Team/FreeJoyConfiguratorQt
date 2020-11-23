#include "converter.h"

Converter::Converter() {}
#include <QDebug>
//! Find device enum in list and return its index
int Converter::EnumToIndex(const int deviceEmun, const QVector<deviceEnum_guiName_t> &list)
{
    for (int i = 0; i < list.size(); ++i) {
        if (deviceEmun == list[i].deviceEnumIndex) {
            return i;
        }
    }
    //throw "EnumToIndex not found device_emun in list, return -1";
    qDebug() << "Converter::EnumToIndex  -  ERROR";
    return -1;
}
