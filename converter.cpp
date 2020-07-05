#include "converter.h"

Converter::Converter()
{

}

//! Find device enum in list and return its index
int Converter::EnumToIndex(const int device_emun, const std::vector<deviceEnum_guiName_t>& list)
{
    for (uint i = 0; i < list.size(); ++i){
        if (device_emun == list[i].device_enum_index){
            return i;
        }
    }
    //throw "EnumToIndex not found device_emun in list, return -1";
    return -1;
}
