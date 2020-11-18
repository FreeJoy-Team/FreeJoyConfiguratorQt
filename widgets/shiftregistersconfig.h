#ifndef SHIFTREGISTERSCONFIG_H
#define SHIFTREGISTERSCONFIG_H

#include <QWidget>
#include <array>        // ??
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

    void RetranslateUi();

signals:
    void shiftRegButtonsCountChanged(int buttons_count);

private slots:
    void shiftRegSelected(int latch_pin, int data_pin, QString pin_gui_name);
    void shiftRegButtonsCalc(int count, int previous_count);

private:
    Ui::ShiftRegistersConfig *ui;

    struct ShiftRegData_t       // можно и глобальную deviceEnum_guiName_t взять
    {
        int pin_number;
        QString gui_name;
    };

    int shift_buttons_count_;

    static bool SortByPinNumber(const ShiftRegData_t& lhs, const ShiftRegData_t& rhs);
    static bool SortNullLast(const ShiftRegData_t& lhs, const ShiftRegData_t& rhs);

    std::array <ShiftRegData_t, MAX_SHIFT_REG_NUM + 1> latch_pins_array{};
    std::array <ShiftRegData_t, MAX_SHIFT_REG_NUM + 1> data_pins_array{};

    QList<ShiftRegisters*> ShiftRegsPtrList_;


};

#endif // SHIFTREGISTERSCONFIG_H
