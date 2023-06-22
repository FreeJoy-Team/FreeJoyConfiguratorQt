#ifndef PINCOMBOBOX_H
#define PINCOMBOBOX_H

#include <QWidget>

#include "deviceconfig.h"
#include "global.h"

#define PINS_COUNT 30
#define PIN_TYPE_COUNT 30
enum // разделить и вынести отдельно?                 // все структуры в global.h?
{
  ANALOG_IN = 100,
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

struct cBox
{
    int deviceEnumIndex;
    QString guiName;
    int pinType[10];
    int pinExcept[10];
    int interaction[10];
    QColor color;
};

struct pins
{
    int pin;
    QString objectName;
    QString guiName;
    int pinType[10];
};

namespace Ui {
class PinComboBox;
}

class PinComboBox : public QWidget
{
    Q_OBJECT

public:
    explicit PinComboBox(uint pinNumber, QWidget *parent = nullptr);
    ~PinComboBox();

    //! return pointer to the first element, size=PINS_COUNT
    const pins *pinList() const;
    int currentDevEnum() const;
    //! return qvector of item index in m_pinTypes struct
    const QVector<int> &pinTypeIndex() const;
    //! return qvector of device enums in combobox
    const QVector<int> &enumIndex() const;

    uint interactCount() const;
    void setInteractCount(const uint &count);
    bool isInteracts() const;

    void setIndexStatus(int index, bool status);
    void resetPin();
    void setIndex_iteraction(int index, int senderIndex);
    void initializationPins(uint pin);
    void readFromConfig(uint pin);
    void writeToConfig(uint pin);

    void retranslateUi();

signals:
    void valueChangedForInteraction(int index, int senderIndex, int pin);
    void currentIndexChanged(int currentDeviceEnum, int previousDeviceEnum, int pinNumber, QString pinName);
private slots:
    void indexChanged(int index);

private:
    Ui::PinComboBox *ui;

    int m_currentDevEnum;
    int m_pinNumber;
    //! return qvector of item index in m_pinTypes struct
    QVector<int> m_pinTypesIndex;
    //! return qvector of device enums in combobox
    QVector<int> m_enumIndex;
    int m_previousIndex;

    bool m_isCall_Interaction;
    bool m_isInteracts;
    uint m_interactCount;
    int m_call_interaction;

    ////////////////////////////// СЛИШКОМ ЖИРНО СДЕЛАТЬ static!!///////////////////////
    const pins m_pinList[PINS_COUNT] = // каждый пин хранит по структуре. а жирно не будет?
    {
        {PA_0,  {"A0"},    {tr("Pin A0")},     {ANALOG_IN}}, // pin device enum // GUI name // pin type
        {PA_1,  {"A1"},    {tr("Pin A1")},     {ANALOG_IN}}, // todo: add SERIAL, PWM...
        {PA_2,  {"A2"},    {tr("Pin A2")},     {ANALOG_IN}},
        {PA_3,  {"A3"},    {tr("Pin A3")},     {ANALOG_IN}},
        {PA_4,  {"A4"},    {tr("Pin A4")},     {ANALOG_IN}},
        {PA_5,  {"A5"},    {tr("Pin A5")},     {ANALOG_IN}},
        {PA_6,  {"A6"},    {tr("Pin A6")},     {ANALOG_IN}},
        {PA_7,  {"A7"},    {tr("Pin A7")},     {ANALOG_IN}},
        {PA_8,  {"A8"},    {tr("Pin A8")},     {}},
        {PA_9,  {"A9"},    {tr("Pin A9")},     {}},
        {PA_10, {"A10"},   {tr("Pin A10")},    {}},
        {PA_15, {"A15"},   {tr("Pin A15")},    {SPI1_NSS}},
        {PB_0,  {"B0"},    {tr("Pin B0")},     {}},
        {PB_1,  {"B1"},    {tr("Pin B1")},     {}},
        {PB_3,  {"B3"},    {tr("Pin B3")},     {SPI1_SCK}},
        {PB_4,  {"B4"},    {tr("Pin B4")},     {SPI1_MISO}},
        {PB_5,  {"B5"},    {tr("Pin B5")},     {SPI1_MOSI}},
        {PB_6,  {"B6"},    {tr("Pin B6")},     {}},
        {PB_7,  {"B7"},    {tr("Pin B7")},     {}},
        {PB_8,  {"B8"},    {tr("Pin B8")},     {I2C1_SCL}},
        {PB_9,  {"B9"},    {tr("Pin B9")},     {I2C1_SDA}},
        {PB_10, {"B10"},   {tr("Pin B10")},    {I2C2_SCL}},
        {PB_11, {"B11"},   {tr("Pin B11")},    {I2C2_SDA}},
        {PB_12, {"B12"},   {tr("Pin B12")},    {}},
        {PB_13, {"B13"},   {tr("Pin B13")},    {}},
        {PB_14, {"B14"},   {tr("Pin B14")},    {}},
        {PB_15, {"B15"},   {tr("Pin B15")},    {}},
        {PC_13, {"C13"},   {tr("Pin C13")},    {}},
        {PC_14, {"C14"},   {tr("Pin C14")},    {}},
        {PC_15, {"C15"},   {tr("Pin C15")},    {}},
    };

    const cBox m_pinTypes[PIN_TYPE_COUNT] = // static ?
    {
        {NOT_USED,       tr("Not Used"),
        {ALL},
        {},
        {}, {}},

        {BUTTON_GND,     tr("Button to Gnd"),   // device enum,   ui name   tr(need to translate)
        {ALL},                                  // add to (example: ALL - add to all comboboxes, PA_8 - add to PA_8 combobox
        {},                                     // except pins (example: add to - ALL, except - PA_8 = add everywhere except PA_8)
        {}, {QColor(25, 130, 240)}},            // interaction with pins, style of interaction with other pins // color, background-color, border-color...

        {BUTTON_VCC,     tr("Button to Vcc"),
        {ALL},
        {},
        {}, {QColor(170, 170, 0)}},

        {BUTTON_ROW,     tr("Button Row"),
        {ALL},
        {},
        {}, {QColor(120, 130, 250)}},

        {BUTTON_COLUMN,  tr("Button Column"),
        {ALL},
        {},
        {}, {QColor(120, 130, 250)}},

        {SHIFT_REG_LATCH,tr("ShiftReg LATCH"),
        {ALL},
        {},
        {}, {QColor(105, 180, 55)}},

        {SHIFT_REG_DATA, tr("ShiftReg DATA"),
        {ALL},
        {},
        {}, {QColor(105, 180, 55)}},

        {SHIFT_REG_CLK, tr("ShiftReg CLK"),
        {ALL},
        {},
        {}, {QColor(105, 180, 55)}},

        {TLE5011_CS,     tr("TLE5011 CS"),
        {ALL},
        {SPI1_SCK, SPI1_MOSI},  // check  PB_6 - not work //////////// NEED FIX !!!!!!!!!!!!!!!!!!!
        {SPI_SCK, SPI_MOSI, TLE5011_GEN}, {QColor(53, 153, 120)}},

        {TLE5012_CS,     tr("TLE5012B CS"),
        {ALL},
        {SPI1_SCK, SPI1_MOSI},
        {SPI_SCK, SPI_MOSI, TLE5011_GEN}, {QColor(53, 153, 120)}},

        {MCP3201_CS,     tr("MCP3201 CS"),
        {ALL},
        {SPI1_SCK, SPI1_MOSI, SPI1_MISO},
        {SPI_SCK, SPI_MOSI, SPI_MISO}, {QColor(53, 153, 120)}},

        {MCP3202_CS,     tr("MCP3202 CS"),
        {ALL},
        {SPI1_SCK, SPI1_MOSI, SPI1_MISO},
        {SPI_SCK, SPI_MOSI, SPI_MISO}, {QColor(53, 153, 120)}},

        {MCP3204_CS,     tr("MCP3204 CS"),
        {ALL},
        {SPI1_SCK, SPI1_MOSI, SPI1_MISO},
        {SPI_SCK, SPI_MOSI, SPI_MISO}, {QColor(53, 153, 120)}},

        {MCP3208_CS,     tr("MCP3208 CS"),
        {ALL},
        {SPI1_SCK, SPI1_MOSI, SPI1_MISO},
        {SPI_SCK, SPI_MOSI, SPI_MISO}, {QColor(53, 153, 120)}},

        {MLX90393_CS,    tr("MLX90393 CS"),
        {ALL},
        {SPI1_SCK, SPI1_MOSI, SPI1_MISO},
        {SPI_SCK, SPI_MOSI, SPI_MISO}, {QColor(53, 153, 120)}},

        {MLX90363_CS,    tr("MLX90363 CS"),
        {ALL},
        {SPI1_SCK, SPI1_MOSI, SPI1_MISO},
        {SPI_SCK, SPI_MOSI, SPI_MISO}, {QColor(53, 153, 120)}},

        {AS5048A_CS,    tr("AS5048A CS"),
        {ALL},
        {SPI1_SCK, SPI1_MOSI, SPI1_MISO},
        {SPI_SCK, SPI_MOSI, SPI_MISO}, {QColor(53, 153, 120)}},

        {LED_SINGLE,     tr("LED Single"),
        {ALL},
        {},
        {}, {QColor(200, 150, 70)}},

        {LED_ROW,        tr("LED Row"),
        {ALL},
        {},
        {}, {QColor(200, 130, 70)}},

        {LED_COLUMN,     tr("LED Column"),
        {ALL},
        {},
        {}, {QColor(200, 130, 70)}},

        {LED_PWM,        tr("LED PWM"),
        {PA_8, PB_0, PB_1, PB_4},
        {},
        {}, {QColor(200, 90, 70)}},

        {LED_RGB,        tr("LED WS2812b"),
        {PA_10},
        {},
        {}, {QColor(255, 85, 255)}},

        {AXIS_ANALOG,    tr("Axis Analog"),
        {ANALOG_IN},
        {},
        {}, {QColor(0, 160, 0)}},

        {FAST_ENCODER,   tr("Fast Encoder"),
        {PA_8, PA_9},
        {},
        {}, {QColor(55, 150, 25)}},

        {SPI_SCK,        tr("SPI SCK"),
        {SPI1_SCK},
        {},
        {}, {QColor(53, 153, 120)}},

        {SPI_MOSI,       tr("SPI MOSI"),
        {SPI1_MOSI},
        {},
        {}, {QColor(53, 153, 120)}},

        {SPI_MISO,       tr("SPI MISO"),
        {SPI1_MISO},
        {},
        {}, {QColor(53, 153, 120)}},

        {TLE5011_GEN,    tr("TLE5011 GEN"),
        {PB_6},
        {},
        {}, {QColor(53, 153, 120)}},

        {I2C_SCL,        tr("I2C SCL"),
        {I2C2_SCL},
        {},
        {I2C_SDA}, {QColor(90, 155, 140)}},

        {I2C_SDA,        tr("I2C SDA"),
        {I2C2_SDA},
        {},
        {I2C_SCL}, {QColor(90, 155, 140)}},
    };
};

#endif // PINCOMBOBOX_H
