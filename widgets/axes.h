#ifndef AXES_H
#define AXES_H

#include <QWidget>
#include "converter.h"

#include "global.h"
#include "deviceconfig.h"

namespace Ui {
class Axes;
}

class Axes : public QWidget     // наверно надо было назвать axis
{
    Q_OBJECT

public:
    explicit Axes(int axis_number, QWidget *parent = nullptr);
    ~Axes();

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

    void UpdateAxisRaw();
    void UpdateAxisOut();

    void AddOrDeleteMainSource(int source_enum, bool is_add);

private slots:
    void filterChanged(int filter_level);
    void functionIndexChanged(int index);
    void calibMinMaxValueChanged(int value);
    void calibrationStarted(int raw_value);
    void mainSourceIndexChanged(int index);
//    void axisRawValueChanged(int);
//    void axisOutValueChanged(int);

    void outputValueChanged(bool is_checked);

    void on_pushButton_StartCalib_clicked(bool checked);

    void on_pushButton_SetCenter_clicked();

private:
    Ui::Axes *ui;
    bool calibration_started_ = false;
    QString start_calibration = tr("Calibrate");
    QString stop_calibration = tr("Stop and Save");
    //dev_config_t* pDev_config;

    std::vector<int> main_source_enum_index_;

    enum
    {
        Encoder = -3,
        I2C = -2,
        None = -1,
        A0 = 0,
        A1,
        A2,
        A3,
        A4,
        A5,
        A6,
        A7,
        A8,
        A9,
        A10,
        A15,
        B0,
        B1,
        B3,
        B4,
        B5,
        B6,
        B7,
        B8,
        B9,
        B10,
        B11,
        B12,
        B13,
        B14,
        B15,
        C13,
        C14,
        C15,
    };

    // const?
    std::vector <deviceEnum_guiName_t> axes_pin_list_ =      // любая последовательность, но первые 2 добавляются в конструкторе
    {{
        {None,     tr("None")},
        {Encoder,  tr("Encoder")},
        {I2C,      tr("I2C")},
        {A0,       tr("A0")},
        {A1,       tr("A1")},
        {A2,       tr("A2")},
        {A3,       tr("A3")},
        {A4,       tr("A4")},
        {A5,       tr("A5")},
        {A6,       tr("A6")},
        {A7,       tr("A7")},
        {A8,       tr("A8")},
        {A9,       tr("A9")},
        {A10,      tr("A10")},
        {A15,      tr("A15")},
        {B0,       tr("B0")},
        {B1,       tr("B1")},
        {B3,       tr("B3")},
        {B4,       tr("B4")},
        {B5,       tr("B5")},
        {B6,       tr("B6")},
        {B7,       tr("B7")},
        {B8,       tr("B8")},
        {B9,       tr("B9")},
        {B10,      tr("B10")},
        {B11,      tr("B11")},
        {B12,      tr("B12")},
        {B13,      tr("B13")},
        {B14,      tr("B14")},
        {B15,      tr("B15")},
        {C13,      tr("C13")},
        {C14,      tr("C14")},
        {C15,      tr("C15")},
    }};

    enum
    {
        AS5600 = 0x36,        //54
        ADS1115_00 = 0x48,    //72
        ADS1115_01,
        ADS1115_10,
        ADS1115_11,
    };

    std::vector <deviceEnum_guiName_t> i2c_address_list_ =      // порядов обязан быть как в enum!!!!!!!!!!!
    {{
        {AS5600,          tr("AS 5600")},
        {ADS1115_00,      tr("ADS 1115_00")},
        {ADS1115_01,      tr("ADS 1115_01")},
        {ADS1115_10,      tr("ADS 1115_10")},
        {ADS1115_11,      tr("ADS 1115_11")},
    }};

    std::vector <deviceEnum_guiName_t> axis_source_main_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {None,          tr("None")},
        {Encoder,       tr("Encoder")},
    }};

    std::vector <deviceEnum_guiName_t> function_list_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {NO_FUNCTION,            tr("None")},
        {FUNCTION_PLUS,      tr("Plus")},
        {FUNCTION_MINUS,     tr("Minus")},
        {FUNCTION_EQUAL,         tr("Equal")},
    }};

    std::vector <deviceEnum_guiName_t> button_1_3_list_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {AXIS_BUTTON_FUNC_EN,       tr("Function Enable")},
        {AXIS_BUTTON_PRESCALER_EN,  tr("Prescale enable")},
        {AXIS_BUTTON_CENTER,        tr("Center")},
        {AXIS_BUTTON_RESET,         tr("Reset")},
        {AXIS_BUTTON_DOWN,          tr("Down")},
        {AXIS_BUTTON_UP,            tr("Up")},
    }};

    std::vector <deviceEnum_guiName_t> button_2_list_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {AXIS_BUTTON_FUNC_EN,       tr("Function Enable")},
        {AXIS_BUTTON_PRESCALER_EN,  tr("Prescale enable")},
        {AXIS_BUTTON_CENTER,        tr("Center")},
        {AXIS_BUTTON_RESET,         tr("Reset")},
    }};

    std::vector <deviceEnum_guiName_t> filter_list_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {FILTER_NO,           tr("Off")},
        {FILTER_LEVEL_1,      tr("Level 1")},
        {FILTER_LEVEL_2,      tr("Level 2")},
        {FILTER_LEVEL_3,      tr("Level 3")},
        {FILTER_LEVEL_4,      tr("Level 4")},
        {FILTER_LEVEL_5,      tr("Level 5")},
        {FILTER_LEVEL_6,      tr("Level 6")},
        {FILTER_LEVEL_7,      tr("Level 7")},
    }};

    std::vector <deviceEnum_guiName_t> axes_list_ =     // дублируется в axestobuttons.h , axescurves.h
    {{
        {0,      tr("X")},
        {1,      tr("Y")},
        {2,      tr("Z")},
        {3,      tr("Rx")},
        {4,      tr("Ry")},
        {5,      tr("Rz")},
        {6,      tr("Slider 1")},
        {7,      tr("Slider 2")},
    }};

    int axis_number_;
};

#endif // AXES_H
