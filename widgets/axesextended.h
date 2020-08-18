#ifndef AXESEXTENDED_H
#define AXESEXTENDED_H

#include <QWidget>

#include "global.h"
#include "deviceconfig.h"

namespace Ui {
class AxesExtended;
}

class AxesExtended : public QWidget
{
    Q_OBJECT

public:
    explicit AxesExtended(int axis_number, QWidget *parent = nullptr);
    ~AxesExtended();

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

    void SetI2CEnabled(bool is_enable);


private slots:
    void filterChanged(int filter_level);
    void functionIndexChanged(int index);

private:
    Ui::AxesExtended *ui;

    int axis_number_;


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
        {AS5600,          "AS 5600"},
        {ADS1115_00,      "ADS 1115_00"},
        {ADS1115_01,      "ADS 1115_01"},
        {ADS1115_10,      "ADS 1115_10"},
        {ADS1115_11,      "ADS 1115_11"},
    }};

    std::vector <deviceEnum_guiName_t> function_list_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {NO_FUNCTION,            tr("None")},
        {FUNCTION_PLUS,          tr("Plus")},
        {FUNCTION_MINUS,         tr("Minus")},
        {FUNCTION_EQUAL,         tr("Equal")},
    }};

    std::vector <deviceEnum_guiName_t> button_1_3_list_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {AXIS_BUTTON_FUNC_EN,       tr("Function enable")},
        {AXIS_BUTTON_PRESCALER_EN,  tr("Prescale enable")},
        {AXIS_BUTTON_CENTER,        tr("Center")},
        {AXIS_BUTTON_RESET,         tr("Reset")},
        {AXIS_BUTTON_DOWN,          tr("Down")},
        {AXIS_BUTTON_UP,            tr("Up")},
    }};

    std::vector <deviceEnum_guiName_t> button_2_list_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {AXIS_BUTTON_FUNC_EN,       tr("Function enable")},
        {AXIS_BUTTON_PRESCALER_EN,  tr("Prescale enable")},
        {AXIS_BUTTON_CENTER,        tr("Center")},
        {AXIS_BUTTON_RESET,         tr("Reset")},
    }};

    std::vector <deviceEnum_guiName_t> filter_list_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {FILTER_NO,           tr("off")},
        {FILTER_LEVEL_1,      tr("level 1")},
        {FILTER_LEVEL_2,      tr("level 2")},
        {FILTER_LEVEL_3,      tr("level 3")},
        {FILTER_LEVEL_4,      tr("level 4")},
        {FILTER_LEVEL_5,      tr("level 5")},
        {FILTER_LEVEL_6,      tr("level 6")},
        {FILTER_LEVEL_7,      tr("level 7")},
    }};

    std::vector <deviceEnum_guiName_t> axes_list_ =     // дублируется в axestobuttons.h , axescurves.h, axes.h, axesextended.h
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

};

#endif // AXESEXTENDED_H
