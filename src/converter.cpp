#include "converter.h"
#include <QDebug>

Converter::Converter() {}

//! Returns the index of the item containing the deviceEnum ; otherwise returns -1
int Converter::EnumToIndex(const int deviceEnum, const QVector<deviceEnum_guiName_t> &list)
{
    for (int i = 0; i < list.size(); ++i) {
        if (deviceEnum == list[i].deviceEnumIndex) {
            return i;
        }
    }
    qCritical() << "Converter::EnumToIndex returns -1";
    return -1;
}

//! Returns the index of the item containing the deviceEnum ; otherwise returns -1
int Converter::EnumToIndex(const int deviceEnum, const QVector<int> &list)
{
    for (int i = 0; i < list.size(); ++i) {
        if (deviceEnum == list[i]) {
            return i;
        }
    }
    qCritical() << "Converter::FindIndex returns -1";
    return -1;
}
