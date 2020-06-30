#ifndef SHIFTREGISTERSCONFIG_H
#define SHIFTREGISTERSCONFIG_H

#include <QWidget>
#include <array>
#include "shiftregisters.h"

#include "global.h"
#include "deviceconfig.h"

namespace Ui {
class ShiftRegistersConfig;
}

class ShiftRegistersConfig : public QWidget
{
    Q_OBJECT

public:
    explicit ShiftRegistersConfig(QWidget *parent = nullptr);
    ~ShiftRegistersConfig();

    void ReadFromConfig();
    void WriteToConfig();

private slots:
    void shiftRegSelected(int latch_pin, int data_pin, QString pin_gui_name);

private:
    Ui::ShiftRegistersConfig *ui;

    struct ShiftRegData_t
    {
        int pin_number;
        QString gui_name;
    };

    static bool SortByPinNumber(const ShiftRegData_t& lhs, const ShiftRegData_t& rhs);
    static bool SortNullLast(const ShiftRegData_t& lhs, const ShiftRegData_t& rhs);

    std::array <ShiftRegData_t, MAX_SHIFT_REG_NUM + 1> latch_pins_array{};
    std::array <ShiftRegData_t, MAX_SHIFT_REG_NUM + 1> data_pins_array{};


//    ShiftRegData_t latch_pins_array[MAX_SHIFT_REG_NUM + 1]{};
//    ShiftRegData_t data_pins_array[MAX_SHIFT_REG_NUM + 1]{};

//    int latch_pins_array_[MAX_SHIFT_REG_NUM + 1]{};
//    int data_pins_array_[MAX_SHIFT_REG_NUM + 1]{};


//    int latch_pin_count_ = 0;
//    int data_pin_count_ = 0;

    QList<ShiftRegisters*> ShiftRegistersAdrList;


};

#endif // SHIFTREGISTERSCONFIG_H
