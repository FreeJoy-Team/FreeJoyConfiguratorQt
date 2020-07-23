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

    void RetranslateUi();

    //private:?
    //! 0 if no pins
    uint pin_count_;    // need?
    //! PinComboBox widget list
    QList<PinComboBox *> PinComboBoxPtrList;

    void ResetAllPins();

signals:
    void totalButtonsValueChanged(int total_buttons);
    void totalLEDsValueChanged(int total_led);
    void fastEncoderSelected(QString pin_gui_name, bool is_selected);
    void shiftRegSelected(int latch_pin, int data_pin, QString pin_gui_name);
    void i2cSelected(bool i2c_selected);
    void axesSourceChanged(int source_enum, bool is_add);

//protected:
//    void resizeEvent(QResizeEvent*) override;

private slots:
    void pinInteraction(int index, int sender_index, int pin);
    void pinIndexChanged(int current_device_enum, int previous_device_enum, int pin_number);
    void a2bCountChanged(int count);
    void shiftRegButtonsCountChanged(int count);
    void totalButtonsChanged(int count);

private:
    Ui::PinConfig *ui;

    QString default_style_;
    bool max_buttons_warning_;

    int shift_latch_count_;
    int shift_data_count_;
    void shiftRegOnOff();

    int axis_sources_;
    int buttons_from_axes_;
    int buttons_from_shift_regs_;
    int single_buttons_;
    int rows_of_buttons_;
    int columns_of_buttons_;
    int single_LED_;
    int rows_of_LED_;
    int columns_of_LED_;


    struct source_t
    {
        int type;
        int pin_type[PIN_TYPE_COUNT];
    };
    const source_t source[8] =
    {
        {AXIS_SOURCE,        {AXIS_ANALOG, TLE5011_CS, MCP3201_CS, MCP3202_CS, MCP3204_CS, MCP3208_CS, MLX90393_CS}},

        {BUTTON_FROM_AXES,   {678}},        // 678 в DeviceConfig

        {SINGLE_BUTTON,      {BUTTON_VCC, BUTTON_GND}},
        {ROW_OF_BUTTONS,     {BUTTON_ROW}},
        {COLUMN_OF_BUTTONS,  {BUTTON_COLUMN}},

        {SINGLE_LED,         {LED_SINGLE}},
        {ROW_OF_LED,         {LED_ROW}},
        {COLUMN_OF_LED,      {LED_COLUMN}},
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
