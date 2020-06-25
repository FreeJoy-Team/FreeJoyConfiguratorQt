#ifndef PINCONFIG_H
#define PINCONFIG_H

#include <QWidget>

#include "pincombobox.h"

namespace Ui {
class PinConfig;
}

class PinConfig : public QWidget
{
    Q_OBJECT

public:
    explicit PinConfig(QWidget *parent = nullptr);
    ~PinConfig();
    void WriteToConfig();
    void ReadFromConfig();

    // make valueChanged for singleButtons_count and etc

    //private:?
    //! 0 if no pins
    uint pin_count_ = 0;

    //! PinComboBox widget list
    QList<PinComboBox *> PinComboBoxPtrList;

private slots:
    void PinInteraction(int index, int sender_index, int pin);
    void SetCurrentConfig(int current_device_enum, int previous_device_enum);

private:
    Ui::PinConfig *ui;

    int axis_sources_ = 0;
    int buttons_from_axes_ = 0;
    int single_buttons_ = 0;
    int rows_of_buttons_ = 0;
    int columns_of_buttons_ = 0;
    int single_LED_ = 0;
    int rows_of_LED_ = 0;
    int columns_of_LED_ = 0;


    struct source_t
    {
        int type;
        int pin_type[PIN_TYPE_COUNT];
    };
    source_t source[8] =
    {
        {AXIS_SOURCE,        {AXIS_ANALOG, TLE5011_CS, MCP3201_CS, MCP3202_CS, MCP3204_CS, MCP3208_CS, MLX90393_CS}},

        {BUTTON_FROM_AXES,   {}},

        {SINGLE_BUTTON,      {BUTTON_VCC, BUTTON_GND}},
        {ROW_OF_BUTTONS,     {BUTTON_ROW}},
        {COLUMN_OF_BUTTONS,  {BUTTON_COLUMN}},

        {SINGLE_LED,         {LED_SINGLE}},
        {ROW_OF_LED,         {LED_ROW}},
        {COLUMN_OF_LED,         {LED_COLUMN}},
    };
    enum
    {
        AXIS_SOURCE = 0,
        BUTTON_FROM_AXES,
        SINGLE_BUTTON,
        ROW_OF_BUTTONS,
        COLUMN_OF_BUTTONS,
        SINGLE_LED,
        ROW_OF_LED,
        COLUMN_OF_LED,
    };
};

#endif // PINCONFIG_H
