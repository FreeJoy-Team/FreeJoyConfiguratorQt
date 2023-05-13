#include "shiftregistersconfig.h"
#include "ui_shiftregistersconfig.h"
#include <QDebug>

ShiftRegistersConfig::ShiftRegistersConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShiftRegistersConfig)
{
    ui->setupUi(this);
    m_shiftButtonsCount = 0;

    ui->layoutV_ShiftRegisters->setAlignment(Qt::AlignTop);
    // shift registers spawn
    for (int i = 0; i < MAX_SHIFT_REG_NUM; i++)
    {
        ShiftRegisters * shift_register = new ShiftRegisters(i, this);
        ui->layoutV_ShiftRegisters->addWidget(shift_register);
        m_shiftRegsPtrList.append(shift_register);
        connect(shift_register, &ShiftRegisters::buttonCountChanged,
                this, &ShiftRegistersConfig::shiftRegButtonsCalc);
    }
}

ShiftRegistersConfig::~ShiftRegistersConfig()
{
    delete ui;
}

void ShiftRegistersConfig::retranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < m_shiftRegsPtrList.size(); ++i) {
        m_shiftRegsPtrList[i]->retranslateUi();
    }
}


void ShiftRegistersConfig::shiftRegButtonsCalc(int currentCount, int previousCount)
{
    m_shiftButtonsCount += currentCount - previousCount;
    emit shiftRegButtonsCountChanged(m_shiftButtonsCount);
}


bool ShiftRegistersConfig::sortByPinNumberAndNullLast(const ShiftRegData_t &lhs, const ShiftRegData_t &rhs)
{
    // sort null last
    if (lhs.pinNumber == 0) {
        return false;
    } else if (rhs.pinNumber == 0) {
        return true;
    } else { // sort ascending
        return lhs.pinNumber < rhs.pinNumber;
    }
}

// bullshit
void ShiftRegistersConfig::addPinAndSort(int pin, const QString &pinGuiName, std::array<ShiftRegData_t, MAX_SHIFT_REG_NUM + 1> &arr)
{
    // add shift reg latch/clk pin
    if (pin > 0) {
        arr.back().pinNumber = pin;
        arr.back().guiName = pinGuiName;
    }
    // delete shift reg latch pin
    else
    {
        pin = -pin;
        for (uint i = 0; i < arr.size(); ++i) {
            if (pin == arr[i].pinNumber) {
                arr[i].pinNumber = 0;
                arr[i].guiName = m_shiftRegsPtrList[0]->defaultText();      //?????????????????
            }
        }
    }

    std::sort(arr.begin(), arr.end(), sortByPinNumberAndNullLast);

    //all unused pins (if (m_latchPinsArray[i].pinNumber == 0)) = bigger pin
    for (int i = arr.size() -1; i >= 0; --i) {                        // bullshit // todo: refactoring
        if (arr[i].pinNumber > 0) {
            // example: we have selected ShiftLatch pins{A0(pinNumber = 1), A2(pinNumber = 3), A6(pinNumber = 7)}
            // in the end it should look like: m_latchPinsArray.pinNumber{1, 3, 7, 7, 7}
            for (uint k = 0; k < arr.size() -1; ++k) {
                if (arr[k].pinNumber == arr[k + 1].pinNumber && arr.back().pinNumber > 0) {
                    for (uint p = 0; p < arr.size() -(k + 2); ++p) {
                        arr[k + p + 1].pinNumber = arr.back().pinNumber;
                        arr[k + p + 1].guiName = arr.back().guiName;
                    }
                    break;
                }
            }
            for (int j = arr.size() -1; j > i; --j) {
                arr[j].pinNumber = arr[i].pinNumber;
                arr[j].guiName = arr[i].guiName;
            }
            break;
        }
    }
}

void ShiftRegistersConfig::shiftRegSelected(int latchPin, int clkPin, int dataPin, const QString &pinGuiName)
{
    // add shift reg latch pin
    if (latchPin != 0) {
        addPinAndSort(latchPin, pinGuiName, m_latchPinsArray);

        // update shiftreg ui
        for (uint i = 0; i < m_latchPinsArray.size() - 1; ++i) {
            m_shiftRegsPtrList[i]->setLatchPin(m_latchPinsArray[i].pinNumber, m_latchPinsArray[i].guiName);
        }
    }
    // add shift reg clk pin
    else if (clkPin != 0) {
        addPinAndSort(clkPin, pinGuiName, m_clkPinsArray);

        // update shiftreg ui
        for (uint i = 0; i < m_clkPinsArray.size() - 1; ++i) {
            m_shiftRegsPtrList[i]->setClkPin(m_clkPinsArray[i].pinNumber, m_clkPinsArray[i].guiName);
        }
    }
    // add shift reg data pin
    else if (dataPin != 0) {
        if (dataPin > 0) {
            m_dataPinsArray[m_dataPinsArray.size() - 1].pinNumber = dataPin;
            m_dataPinsArray[m_dataPinsArray.size() - 1].guiName = pinGuiName;
        } else {
            dataPin = -dataPin;
            for (uint i = 0; i < m_dataPinsArray.size(); ++i) {
                if (dataPin == m_dataPinsArray[i].pinNumber){
                    m_dataPinsArray[i].pinNumber = 0;
                    m_dataPinsArray[i].guiName = m_shiftRegsPtrList[i]->defaultText();      //?????????????????
                }
            }
        }

        std::sort(m_dataPinsArray.begin(), m_dataPinsArray.end(), sortByPinNumberAndNullLast);

        for (uint i = 0; i < m_dataPinsArray.size() - 1; ++i) {
            m_shiftRegsPtrList[i]->setDataPin(m_dataPinsArray[i].pinNumber, m_dataPinsArray[i].guiName);
        }

    }
}

void ShiftRegistersConfig::readFromConfig()
{
    for (int i = 0; i < m_shiftRegsPtrList.size(); ++i) {
        m_shiftRegsPtrList[i]->readFromConfig();
    }
}

void ShiftRegistersConfig::writeToConfig()
{
    for (int i = 0; i < m_shiftRegsPtrList.size(); ++i) {
        m_shiftRegsPtrList[i]->writeToConfig();
    }
}
