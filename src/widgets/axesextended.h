#ifndef AXESEXTENDED_H
#define AXESEXTENDED_H

#include <QWidget>

#include "deviceconfig.h"
#include "global.h"

namespace Ui {
class AxesExtended;
}

class AxesExtended : public QWidget
{
    Q_OBJECT

public:
    explicit AxesExtended(int axisNumber, QWidget *parent = nullptr);
    ~AxesExtended();

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

    void setI2CEnabled(bool isEnable);

private slots:
    void filterChanged(int filterLevel);
    void functionIndexChanged(int index);

private:
    Ui::AxesExtended *ui;

    int m_axisNumber;
    QVector<int> m_i2c_enumIndex;
    QVector<int> m_function_enumIndex;
    QVector<int> m_button_1_3_enumIndex;
    QVector<int> m_button_2_enumIndex;

    enum
    {
        AS5600 = 0x36,        //54
        ADS1115_00 = 0x48,    //72
        ADS1115_01,
        ADS1115_10,
        ADS1115_11,
    };

    const QVector <deviceEnum_guiName_t> m_i2cPtrList =      // static?
    {{
        {AS5600,          "AS 5600"},
        {ADS1115_00,      "ADS 1115_00"},
        {ADS1115_01,      "ADS 1115_01"},
        {ADS1115_10,      "ADS 1115_10"},
        {ADS1115_11,      "ADS 1115_11"},
    }};

    const QVector <deviceEnum_guiName_t> m_functionList =      // static?
    {{
        {NO_FUNCTION,            tr("None")},
        {FUNCTION_PLUS,          tr("Plus")},
        {FUNCTION_MINUS,         tr("Minus")},
        {FUNCTION_EQUAL,         tr("Equal")},
    }};

    const QVector <deviceEnum_guiName_t> m_button_1_3_list =      // static?
    {{
        {AXIS_BUTTON_FUNC_EN,       tr("Function enable")},
        {AXIS_BUTTON_PRESCALER_EN,  tr("Prescale enable")},
        {AXIS_BUTTON_CENTER,        tr("Center")},
        {AXIS_BUTTON_RESET,         tr("Reset")},
        {AXIS_BUTTON_DOWN,          tr("Down")},
        {AXIS_BUTTON_UP,            tr("Up")},
    }};

    const QVector <deviceEnum_guiName_t> m_button_2_list =      // static?
    {{
        {AXIS_BUTTON_FUNC_EN,       tr("Function enable")},
        {AXIS_BUTTON_PRESCALER_EN,  tr("Prescale enable")},
        {AXIS_BUTTON_CENTER,        tr("Center")},
        {AXIS_BUTTON_RESET,         tr("Reset")},
    }};

    const QVector <deviceEnum_guiName_t> m_filterList =      // порядок обязан быть как в common_types.h!!!!!!!!!!!      // static?
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
};

#endif // AXESEXTENDED_H
