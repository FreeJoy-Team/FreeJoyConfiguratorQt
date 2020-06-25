#ifndef PINCOMBOBOX_H
#define PINCOMBOBOX_H

#include <QWidget>
#include <QTranslator>
#include <QDebug>

#include "global.h"
#include "deviceconfig.h"

#define PIN_TYPE_COUNT 25
enum
{
    PA_0 = 1,
    PA_1,
    PA_2,
    PA_3,
    PA_4,
    PA_5,
    PA_6,
    PA_7,
    PA_8,
    PA_9,
    PA_10,
    PA_15,

    PB_0,
    PB_1,
    PB_3,
    PB_4,
    PB_5,
    PB_6,
    PB_7,
    PB_8,
    PB_9,
    PB_10,
    PB_11,
    PB_12,
    PB_13,
    PB_14,
    PB_15,

    PC_13,
    PC_14,
    PC_15,


    ANALOG_IN,
    FAST_ENCODER_PIN,
    LED_PWM_PIN,

    I2C1_SDA,
    I2C1_SCL,
    I2C2_SDA,
    I2C2_SCL,

    SPI1_MOSI,
    SPI1_MISO,
    SPI1_SCK,
    SPI1_NSS,
    SPI2_MOSI,
    SPI2_MISO,
    SPI2_SCK,
    SPI2_NSS,



    ALL = 999,
};
typedef int8_t pin_type_t;

struct cBox
{
    int device_enum_index;
    QString gui_name;
    int pin_type[10];
    int pin_except[10];
    int interaction[10];
    QString styleSheet;         // стиль взаимодействия
};

struct pins
{
    int pin;
    int pin_type[10];
};


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
    void SetIndex(int index, int sender_index);
    void InitializationPins(uint pin);
    void ReadFromConfig(uint pin);
    void WriteToConfig(uint pin);

    int GetEnumValue();       //?

    //private?
    QString EnumToString(int deviceEnum);
    int EnumToIndex(int deviceEnum);
    int pin_number_ = -1;
    //! номер элемента в pin_types
    std::vector<int> enum_gui_index;            // rename to pin_types_index
    std::vector<int> enum_index;

    // public private _
    bool is_call_interaction_ = false;
    bool is_interacts_ = false;
    uint interact_count_ = 0;
    int call_interaction_;
    QString styleSheet_default_;

signals:
    void valueChanged(int index, int sender_index, int pin);
    //void valueChanged2(int pin_list_index);        // CHANGE SUM BUTTONS
private slots:
    void IndexChanged(int index);

public:     // private
    pins const pin_list[30] =
    {
        {PA_0,  {ANALOG_IN}},   // пин // его типы
        {PA_1,  {ANALOG_IN}},   // добавить SERIAL, PWM...
        {PA_2,  {ANALOG_IN}},
        {PA_3,  {ANALOG_IN}},
        {PA_4,  {ANALOG_IN}},
        {PA_5,  {ANALOG_IN}},
        {PA_6,  {ANALOG_IN}},
        {PA_7,  {ANALOG_IN}},
        {PA_8,  {}},
        {PA_9,  {}},
        {PA_10, {}},
        {PA_15, {SPI1_NSS}},
        {PB_0,  {}},
        {PB_1,  {}},
        {PB_3,  {SPI1_SCK}},
        {PB_4,  {SPI1_MISO}},
        {PB_5,  {SPI1_MOSI}},
        {PB_6,  {}},
        {PB_7,  {}},
        {PB_8,  {I2C1_SCL}},
        {PB_9,  {I2C1_SDA}},
        {PB_10, {}},
        {PB_11, {}},
        {PB_12, {}},
        {PB_13, {}},
        {PB_14, {}},
        {PB_15, {}},
        {PC_13, {}},
        {PC_14, {}},
        {PC_15, {}},
    };
    cBox const pin_types[PIN_TYPE_COUNT] =
    {
        {NOT_USED,       "Not Used",
         {ALL},
         {},
         {}, {}},

        {BUTTON_GND,     "Button Gnd",          // контроллер enum,  имя в ui
         {ALL},                                 // добавление в конкретный пин или пины типа
         {},                                    // исключая пины
         {}, {"color: rgb(255, 0, 255);"}},     // взаимодействие с другими типами // стиль color, background-color, border-color...

        {BUTTON_VCC,     "Button Vcc",
         {ALL},
         {},
         {}, {}},

        {BUTTON_ROW,     "Button Row",
         {ALL},
         {},
         {}, {}},

        {BUTTON_COLUMN,  "Button Column",
         {ALL},
         {},
         {}, {}},

        {SHIFT_REG_LATCH,"ShiftReg LATCH",
         {ALL},
         {SPI1_SCK},
         {SPI_SCK}, {"color: rgb(0, 153, 0);"}},

        {SHIFT_REG_DATA, "ShiftReg DATA",
         {ALL},
         {SPI1_SCK},
         {SPI_SCK}, {"color: rgb(0, 153, 0);"}},

        {TLE5011_CS,     "TLE5011 CS",
         {ALL},
         {SPI1_SCK, SPI1_MOSI, PB_6},
         {SPI_SCK, SPI_MOSI, TLE5011_GEN}, {"color: rgb(53, 153, 155);"}},

        {MCP3201_CS,     "MCP3201 CS",
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(153, 153, 55);"}},

        {MCP3202_CS,     "MCP3202 CS",
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(153, 53, 155);"}},

        {MCP3204_CS,     "MCP3204 CS",
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(53, 53, 0);"}},

        {MCP3208_CS,     "MCP3208 CS",
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(53, 0, 55);"}},

        {MLX90393_CS,    "MLX90393 CS",
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(0, 53, 55);"}},

        {LED_SINGLE,     "LED Single",
         {ALL},
         {},
         {}, {}},

        {LED_ROW,        "LED Row",
         {ALL},
         {},
         {}, {}},

        {LED_COLUMN,     "LED Column",
         {ALL},
         {},
         {}, {}},

        {LED_PWM,        "LED PWM",
         {PB_0, PB_1, PB_4},
         {},
         {}, {}},

        {AXIS_ANALOG,    "Axis Analog",
         {ANALOG_IN},
         {},
         {}, {}},

        {FAST_ENCODER,   "Fast Encoder",
         {PA_8, PA_9},
         {},
         {}, {}},

        {SPI_SCK,        "SPI SCK",
         {SPI1_SCK},
         {},
         {}, {}},

        {SPI_MOSI,       "SPI MOSI",
         {SPI1_MOSI},
         {},
         {}, {}},

        {SPI_MISO,       "SPI MISO",
         {SPI1_MISO},
         {},
         {}, {}},

        {TLE5011_GEN,    "TLE5011 GEN",
         {PB_6},
         {},
         {}, {}},

        {I2C_SCL,        "I2C SCL",
         {I2C1_SCL},
         {},
         {I2C_SDA}, {"color: rgb(53, 53, 255);"}},

        {I2C_SDA,        "I2C SDA",
         {I2C1_SDA},
         {},
         {I2C_SCL}, {"color: rgb(53, 53, 255);"}},

    };


private:
    Ui::PinComboBox *ui;
    int last_deleted_ = 0;
};

#endif // PINCOMBOBOX_H
