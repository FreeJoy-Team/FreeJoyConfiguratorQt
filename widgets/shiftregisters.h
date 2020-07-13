#ifndef SHIFTREGISTERS_H
#define SHIFTREGISTERS_H

#include <QWidget>

#include "global.h"
#include "deviceconfig.h"

#define SHIFT_REG_TYPES 4

namespace Ui {
class ShiftRegisters;
}

class ShiftRegisters : public QWidget
{
    Q_OBJECT

public:
    explicit ShiftRegisters(int shift_reg_number, QWidget *parent = nullptr);
    ~ShiftRegisters();

    void ReadFromConfig();
    void WriteToConfig();

//    int GetLatchPin();
//    int GetDataPin();
//    QString GetLatchString();
//    QString GetDataString();

    void SetLatchPin(int latch_pin, QString pin_gui_name);
    void SetDataPin(int data_pin, QString pin_gui_name);

    QString not_defined_ = tr("Not defined");       // hz // getter?

signals:
    void buttonCountChanged(int buttons_count, int shift_reg_number);

private slots:
    void calcRegistersCount(int button_count);

private:
    Ui::ShiftRegisters *ui;
    void SetUiOnOff();

    int buttons_count_;
    int shift_reg_number_;
    int latch_pin_ = 0;
    int data_pin_ = 0;

    deviceEnum_guiName_t const shift_registers_list_[SHIFT_REG_TYPES] =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {
        {HC165_PULL_DOWN,      tr("HC165 Pull Down")},
        {CD4021_PULL_DOWN,     tr("CD4021 Pull Down")},
        {HC165_PULL_UP,        tr("HC165 Pull Up")},
        {CD4021_PULL_UP,       tr("CD4021 Pull Up")},
    };
};

#endif // SHIFTREGISTERS_H
