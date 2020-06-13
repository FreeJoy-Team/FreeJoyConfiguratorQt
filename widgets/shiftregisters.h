#ifndef SHIFTREGISTERS_H
#define SHIFTREGISTERS_H

#include <QWidget>

namespace Ui {
class ShiftRegisters;
}

class ShiftRegisters : public QWidget
{
    Q_OBJECT

public:
    explicit ShiftRegisters(QWidget *parent = nullptr);
    ~ShiftRegisters();

private:
    Ui::ShiftRegisters *ui;
};

#endif // SHIFTREGISTERS_H
