#ifndef SHIFTREGISTERS_H
#define SHIFTREGISTERS_H

#include <QWidget>

#include "deviceconfig.h"
#include "global.h"

#define SHIFT_REG_TYPES 4

namespace Ui {
class ShiftRegisters;
}

class ShiftRegisters : public QWidget
{
    Q_OBJECT

public:
    explicit ShiftRegisters(int shiftRegNumber, QWidget *parent = nullptr);
    ~ShiftRegisters();

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

    void setLatchPin(int latchPin, QString pinGuiName);
    void setClkPin(int clkPin, QString pinGuiName);
    void setDataPin(int dataPin, QString pinGuiName);

    const QString &defaultText() const;

signals:
    void buttonCountChanged(int currentCount, int previousCount);

private slots:
    void calcRegistersCount(int buttonCount);

private:
    Ui::ShiftRegisters *ui;
    void setUiOnOff();

    static QString m_notDefined;
    int m_buttonsCount;
    int m_shiftRegNumber;
    int m_latchPin;
    int m_clkPin;
    int m_dataPin;

    const deviceEnum_guiName_t m_shiftRegistersList[SHIFT_REG_TYPES] = // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {
        {HC165_PULL_DOWN,   tr("HC165 Pull Down")},
        {CD4021_PULL_DOWN,  tr("CD4021 Pull Down")},
        {HC165_PULL_UP,     tr("HC165 Pull Up")},
        {CD4021_PULL_UP,    tr("CD4021 Pull Up")},
    };
};

#endif // SHIFTREGISTERS_H
