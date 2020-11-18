#include "converter.h"

Converter::Converter()
{

}
#include <QDebug>
//! Find device enum in list and return its index
int Converter::EnumToIndex(const int device_emun, const QVector<deviceEnum_guiName_t>& list)
{
    for (int i = 0; i < list.size(); ++i){
        if (device_emun == list[i].device_enum_index){
            return i;
        }
    }
    //throw "EnumToIndex not found device_emun in list, return -1";
    qDebug()<<"Converter::EnumToIndex  -  ERROR";
    return -1;
}
