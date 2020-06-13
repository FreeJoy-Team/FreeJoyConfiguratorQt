#ifndef PINCOMBOBOX_H
#define PINCOMBOBOX_H

#include <QWidget>
#include <QTranslator>

#include "global.h"
#include "deviceconfig.h"

#define PIN_TYPE_COUNT 25

namespace Ui {
class PinComboBox;
}

class PinComboBox : public QWidget
{
    Q_OBJECT

public:
    explicit PinComboBox(QWidget *parent = nullptr);
    ~PinComboBox();
    int GetIndex();
    void Initialization_pinA(int pin);
    void Initialization_pinB(int pin);
    void Initialization_pinC(int pin);
    void ReadFromConfig(uint pin);
    void WriteToConfig(uint pin);

    int GetEnumValue();       //?

    //private?
    QString EnumToString(int deviceEnum);
    int EnumToIndex(int deviceEnum);
    int pin_a_ = -1;
    int pin_b_ = -1;
    int pin_c_ = -1;

signals:
    void valueChanged();
    //void valueChanged2(int pin_list_index);        // CHANGE SUM BUTTONS
private slots:
    void IndexChanged(int index);

    // private: ?
public:
    deviceEnum_guiName_t const pin_list_[PIN_TYPE_COUNT] =
    {
        {NOT_USED,       "Not Used"},
        {BUTTON_GND,     "Button Gnd"},
        {BUTTON_VCC,     "Button Vcc"},
        {BUTTON_ROW,     "Button Row"},
        {BUTTON_COLUMN,  "Button Column"},
        {SHIFT_REG_LATCH,"ShiftReg LATCH"},
        {SHIFT_REG_DATA, "ShiftReg DATA"},
        {TLE5011_CS,     "TLE5011 CS"},
        {MCP3201_CS,     "MCP3201 CS"},
        {MCP3202_CS,     "MCP3202 CS"},
        {MCP3204_CS,     "MCP3204 CS"},
        {MCP3208_CS,     "MCP3208 CS"},
        {MLX90393_CS,    "MLX90393 CS"},
        {LED_SINGLE,     "LED Single"},
        {LED_ROW,        "LED Row"},
        {LED_COLUMN,     "LED Column"},
////////// ^for all

        {AXIS_ANALOG,    "Axis Analog"},
        {FAST_ENCODER,   "Fast Encoder"},
        {SPI_SCK,        "SPI SCK"},
        {SPI_MOSI,       "SPI MOSI"},
        {SPI_MISO,       "SPI MISO"},
        {TLE5011_GEN,    "TLE5011 GEN"},
        {LED_PWM,        "LED PWM"},
        {I2C_SCL,        "I2C SCL"},
        {I2C_SDA,        "I2C SDA"},
    };

private:
    Ui::PinComboBox *ui;
    int last_deleted_ = 0;
};

#endif // PINCOMBOBOX_H
