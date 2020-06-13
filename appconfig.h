#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QWidget>
#include "common_defines.h"
#include "common_types.h"



//struct I3DEngine;
//// CryEngine global environment
//struct SSystemGlobalEnvironment
//{
//	I3DEngine*                 p3DEngine = nullptr;

//    // thread
//};


class AppConfig
{
private:
    struct pin_list_t
    {
        int device_enum_index;
        QString gui_name;
    };

public:
    AppConfig();

//    pin_list_t const pin_list[PIN_TYPE_COUNT] =
//        {
//            {NOT_USED,       "Not Used"},
//            {BUTTON_GND,     "Button Gnd"},
//            {BUTTON_VCC,     "Button Vcc"},
//            {BUTTON_ROW,     "Button Row"},
//            {BUTTON_COLUMN,  "Button Column"},
//            {AXIS_ANALOG,    "Axis Analog"},
//            {SPI_SCK,        "SPI SCK"},
//            {SPI_MOSI,       "SPI MOSI"},
//            {SPI_MISO,       "SPI MISO"},
//            {TLE5011_CS,     "TLE5011 CS"},
//            {TLE5011_GEN,    "TLE5011 GEN"},
//            {MCP3201_CS,     "MCP3201 CS"},
//            {MCP3202_CS,     "MCP3202 CS"},
//            {MCP3204_CS,     "MCP3204 CS"},
//            {MCP3208_CS,     "MCP3208 CS"},
//            {MLX90393_CS,    "MLX90393 CS"},
//            {SHIFT_REG_LATCH,"ShiftReg LATCH"},
//            {SHIFT_REG_DATA, "ShiftReg DATA"},
//            {LED_PWM,        "LED PWM"},
//            {LED_SINGLE,     "LED Single"},
//            {LED_ROW,        "LED Row"},
//            {LED_COLUMN,     "LED Column"},
//            {I2C_SCL,        "I2C SCL"},
//            {I2C_SDA,        "I2C SDA"},
//        };

    //QStringList pin_list;
    //QStringList button_list;
    //QStringList function_list;
    //QStringList filter_list;
    //QStringList timer_list;
    //                "Not Used"<<
    //                "Button Gnd"<< "Button Vcc"<<
    //                "Button Row"<<"Button Column"<<
    //                "Axis Analog" <<
    //                "SPI SCK"<<"SPI MOSI" <<"SPI MISO"<<
    //                "TLE5011 CS"<<"TLE5011 GEN"<<
    //                "MCP3201 CS"<<"MCP3202 CS"<<
    //                "MCP3204 CS"<<"MCP3208 CS"<<
    //                "MLX90393 CS"<<
    //                "ShiftReg LATCH"<<"ShiftReg DATA"<<
    //                "LED PWM"<< "LED Single"<<
    //                "LED Row"<<"LED Column"<<
    //                "I2C SCL"<<"I2C SDA");
    //QStringList list=(QStringList()<<

};

#endif // APPCONFIG_H
