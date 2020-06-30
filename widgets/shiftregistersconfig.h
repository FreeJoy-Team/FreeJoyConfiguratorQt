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

    QList<ShiftRegisters*> ShiftRegistersAdrList;


};

#endif // SHIFTREGISTERSCONFIG_H
