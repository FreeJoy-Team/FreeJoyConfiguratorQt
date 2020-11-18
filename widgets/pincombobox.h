#ifndef PINCOMBOBOX_H
#define PINCOMBOBOX_H

#include <QWidget>

#include "global.h"
#include "deviceconfig.h"

#define PINS_COUNT 30
#define PIN_TYPE_COUNT 25
enum        // разделить и вынести отдельно?                 // все структуры в global.h?
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
    QString gui_name;
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

    //! return pointer to the first element, size=PINS_COUNT
    const pins *PinList() const;
    int CurrentDevEnum() const;
    //! номер элемента в pin_types
    const QVector<int> & PinTypeIndex() const;
    //! device enum присутствующие в комбобоксе
    const QVector<int> & EnumIndex() const;

    uint InteractCount() const;
    void SetInteractCount(const uint &count);
    bool IsInteracts() const;

    void SetIndexStatus(int index, bool status);
    //void SetIndex();
    void ResetPin();
    void SetIndex_Iteraction(int index, int sender_index);
    void InitializationPins(uint pin);
    void ReadFromConfig(uint pin);
    void WriteToConfig(uint pin);

    void RetranslateUi();

signals:
    void valueChangedForInteraction(int index, int sender_index, int pin);
    void currentIndexChanged(int current_device_enum, int previous_device_enum, int pin_number);
private slots:
    void IndexChanged(int index);


private:
    Ui::PinComboBox *ui;

    int current_dev_enum_;
    int pin_number_;
    //! номер элемента в pin_types
    QVector<int> pin_types_index_;
    //! device enum присутствующие в комбобоксе
    QVector<int> enum_index_;
    int previous_index_;

    bool is_call_interaction_;
    bool is_interacts_;
    uint interact_count_;
    int call_interaction_;
    QString styleSheet_default_;

    ////////////////////////////// СЛИШКОМ ЖИРНО СДЕЛАТЬ static!!///////////////////////
    const pins pin_list_[PINS_COUNT] =       // каждый пин хранит по структуре. а жирно не будет?
    {
        {PA_0,  {tr("Pin A0")},     {ANALOG_IN}},   // пин // GUI name // его типы
        {PA_1,  {tr("Pin A1")},     {ANALOG_IN}},   // добавить SERIAL, PWM...
        {PA_2,  {tr("Pin A2")},     {ANALOG_IN}},
        {PA_3,  {tr("Pin A3")},     {ANALOG_IN}},
        {PA_4,  {tr("Pin A4")},     {ANALOG_IN}},
        {PA_5,  {tr("Pin A5")},     {ANALOG_IN}},
        {PA_6,  {tr("Pin A6")},     {ANALOG_IN}},
        {PA_7,  {tr("Pin A7")},     {ANALOG_IN}},
        {PA_8,  {tr("Pin A8")},     {}},
        {PA_9,  {tr("Pin A9")},     {}},
        {PA_10, {tr("Pin A10")},    {}},
        {PA_15, {tr("Pin A15")},    {SPI1_NSS}},
        {PB_0,  {tr("Pin B0")},     {}},
        {PB_1,  {tr("Pin B1")},     {}},
        {PB_3,  {tr("Pin B3")},     {SPI1_SCK}},
        {PB_4,  {tr("Pin B4")},     {SPI1_MISO}},
        {PB_5,  {tr("Pin B5")},     {SPI1_MOSI}},
        {PB_6,  {tr("Pin B6")},     {}},
        {PB_7,  {tr("Pin B7")},     {}},
        {PB_8,  {tr("Pin B8")},     {I2C1_SCL}},
        {PB_9,  {tr("Pin B9")},     {I2C1_SDA}},
        {PB_10, {tr("Pin B10")},    {}},
        {PB_11, {tr("Pin B11")},    {}},
        {PB_12, {tr("Pin B12")},    {}},
        {PB_13, {tr("Pin B13")},    {}},
        {PB_14, {tr("Pin B14")},    {}},
        {PB_15, {tr("Pin B15")},    {}},
        {PC_13, {tr("Pin C13")},    {}},
        {PC_14, {tr("Pin C14")},    {}},
        {PC_15, {tr("Pin C15")},    {}},
    };

    const cBox pin_types_[PIN_TYPE_COUNT] =          // static ?
    {
        {NOT_USED,       tr("Not Used"),
         {ALL},
         {},
         {}, {}},

        {BUTTON_GND,     tr("Button Gnd"),      // контроллер enum,  имя в ui   tr(нужен перевод)
         {ALL},                                 // добавление в конкретный пин или пины типа
         {},                                    // исключая пины
         {}, {"color: rgb(150, 155, 55);"}},     // взаимодействие с другими типами // стиль color, background-color, border-color...

        {BUTTON_VCC,     tr("Button Vcc"),
         {ALL},
         {},
         {}, {"color: rgb(170, 25, 25);"}},

        {BUTTON_ROW,     tr("Button Row"),
         {ALL},
         {},
         {}, {"color: rgb(25, 130, 220);"}},

        {BUTTON_COLUMN,  tr("Button Column"),
         {ALL},
         {},
         {}, {"color: rgb(120, 130, 250);"}},

        {SHIFT_REG_LATCH,tr("ShiftReg LATCH"),
         {ALL},
         {SPI1_SCK},
         {SPI_SCK}, {"color: rgb(53, 153, 120);"}},

        {SHIFT_REG_DATA, tr("ShiftReg DATA"),
         {ALL},
         {SPI1_SCK},
         {SPI_SCK}, {"color: rgb(53, 153, 120);"}},

        {TLE5011_CS,     tr("TLE5011 CS"),
         {ALL},
         {SPI1_SCK, SPI1_MOSI, PB_6},
         {SPI_SCK, SPI_MOSI, TLE5011_GEN}, {"color: rgb(53, 153, 120);"}},

        {MCP3201_CS,     tr("MCP3201 CS"),
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(53, 153, 120);"}},

        {MCP3202_CS,     tr("MCP3202 CS"),
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(53, 153, 120);"}},

        {MCP3204_CS,     tr("MCP3204 CS"),
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(53, 153, 120);"}},

        {MCP3208_CS,     tr("MCP3208 CS"),
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(53, 153, 120);"}},

        {MLX90393_CS,    tr("MLX90393 CS"),
         {ALL},
         {SPI1_SCK, SPI1_MOSI, SPI_MISO},
         {SPI_SCK, SPI_MOSI, SPI_MISO}, {"color: rgb(53, 153, 120);"}},

        {LED_SINGLE,     tr("LED Single"),
         {ALL},
         {},
         {}, {"color: rgb(155, 0, 200);"}},

        {LED_ROW,        tr("LED Row"),
         {ALL},
         {},
         {}, {"color: rgb(155, 0, 120);"}},

        {LED_COLUMN,     tr("LED Column"),
         {ALL},
         {},
         {}, {"color: rgb(190, 20, 120);"}},

        {LED_PWM,        tr("LED PWM"),
         {PB_0, PB_1, PB_4},
         {},
         {}, {"color: rgb(200, 0, 180);"}},

        {AXIS_ANALOG,    tr("Axis Analog"),
         {ANALOG_IN},
         {},
         {}, {"color: rgb(0, 155, 0);"}},

        {FAST_ENCODER,   tr("Fast Encoder"),
         {PA_8, PA_9},
         {},
         {}, {"color: rgb(200, 10, 10);"}},

        {SPI_SCK,        tr("SPI SCK"),
         {SPI1_SCK},
         {},
         {}, {}},

        {SPI_MOSI,       tr("SPI MOSI"),
         {SPI1_MOSI},
         {},
         {}, {}},

        {SPI_MISO,       tr("SPI MISO"),
         {SPI1_MISO},
         {},
         {}, {}},

        {TLE5011_GEN,    tr("TLE5011 GEN"),
         {PB_6},
         {},
         {}, {}},

        {I2C_SCL,        tr("I2C SCL"),
         {I2C1_SCL},
         {},
         {I2C_SDA}, {"color: rgb(53, 53, 255);"}},

        {I2C_SDA,        tr("I2C SDA"),
         {I2C1_SDA},
         {},
         {I2C_SCL}, {"color: rgb(53, 53, 255);"}},

    };
};

#endif // PINCOMBOBOX_H
