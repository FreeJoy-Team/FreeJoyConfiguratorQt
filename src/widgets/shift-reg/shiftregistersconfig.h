#ifndef SHIFTREGISTERSCONFIG_H
#define SHIFTREGISTERSCONFIG_H

#include "shiftregisters.h"
#include <array> // ??
#include <QWidget>

#include "deviceconfig.h"
#include "global.h"

namespace Ui {
class ShiftRegistersConfig;
}

class ShiftRegistersConfig : public QWidget
{
    Q_OBJECT

public:
    explicit ShiftRegistersConfig(QWidget *parent = nullptr);
    ~ShiftRegistersConfig();

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

signals:
    void shiftRegButtonsCountChanged(int buttonsCount);

public slots:
    void shiftRegSelected(int latchPin, int clkPin, int dataPin, const QString &pinGuiName);
private slots:
    void shiftRegButtonsCalc(int currentCount, int previousCount);

private:
    Ui::ShiftRegistersConfig *ui;

    struct ShiftRegData_t // можно и глобальную deviceEnum_guiName_t взять
    {
        int pinNumber;
        QString guiName;
    };

    int m_shiftButtonsCount;

    static bool sortByPinNumber(const ShiftRegData_t &lhs, const ShiftRegData_t &rhs);
    static bool sortNullLast(const ShiftRegData_t &lhs, const ShiftRegData_t &rhs);
    void addPinAndSort(int pin, const QString &pinGuiName, std::array<ShiftRegData_t, MAX_SHIFT_REG_NUM + 1> &arr);

    std::array<ShiftRegData_t, MAX_SHIFT_REG_NUM + 1> m_latchPinsArray{};
    std::array<ShiftRegData_t, MAX_SHIFT_REG_NUM + 1> m_clkPinsArray{};
    std::array<ShiftRegData_t, MAX_SHIFT_REG_NUM + 1> m_dataPinsArray{};

    QList<ShiftRegisters *> m_shiftRegsPtrList;
};

#endif // SHIFTREGISTERSCONFIG_H
