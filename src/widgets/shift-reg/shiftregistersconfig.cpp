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


bool ShiftRegistersConfig::sortByPinNumber(const ShiftRegData_t& lhs, const ShiftRegData_t& rhs)
{
    return lhs.pinNumber < rhs.pinNumber;
}

bool ShiftRegistersConfig::sortNullLast(const ShiftRegData_t& lhs, const ShiftRegData_t& rhs)
{
    Q_UNUSED(lhs);
    return rhs.pinNumber == 0;
}

void ShiftRegistersConfig::shiftRegSelected(int latchPin, int dataPin, const QString &pinGuiName)
{
    // add shift reg latch pin
    if (latchPin != 0) {
        if (latchPin > 0) {
            m_latchPinsArray.back().pinNumber = latchPin;
            m_latchPinsArray.back().guiName = pinGuiName;
        }
        // delete shift reg latch pin
        else
        {
            latchPin = -latchPin;
            for (uint i = 0; i < m_latchPinsArray.size(); ++i) {
                if (latchPin == m_latchPinsArray[i].pinNumber) {
                    m_latchPinsArray[i].pinNumber = 0;
                    m_latchPinsArray[i].guiName = m_shiftRegsPtrList[0]->defaultText();      //?????????????????
                }
            }
        }
        // sort ascending
        std::sort(m_latchPinsArray.begin(), m_latchPinsArray.end(), sortByPinNumber);           // хз можно ли через 1 сорт
        // sort null last
        std::stable_sort(m_latchPinsArray.begin(), m_latchPinsArray.end(), sortNullLast);
        //all unused pins (if (m_latchPinsArray[i].pinNumber == 0)) = bigger pin
        for (uint i = m_latchPinsArray.size() -1; i >= 0; --i) {                        // bullshit // todo: refactoring

            if (m_latchPinsArray[i].pinNumber > 0) {
                // example: we have selected ShiftLatch pins{A0(pinNumber = 1), A2(pinNumber = 3), A6(pinNumber = 7)}
                // in the end it should look like: m_latchPinsArray.pinNumber{1, 3, 7, 7, 7}
                for (uint k = 0; k < m_latchPinsArray.size() -1; ++k) {
                    if (m_latchPinsArray[k].pinNumber == m_latchPinsArray[k + 1].pinNumber && m_latchPinsArray.back().pinNumber > 0) {
                        for (uint p = 0; p < m_latchPinsArray.size() -(k + 2); ++p) {
                            m_latchPinsArray[k + p + 1].pinNumber = m_latchPinsArray.back().pinNumber;
                            m_latchPinsArray[k + p + 1].guiName = m_latchPinsArray.back().guiName;
                        }
                        break;
                    }
                }

                for (uint j = m_latchPinsArray.size() -1; j > i; --j) {
                    m_latchPinsArray[j].pinNumber = m_latchPinsArray[i].pinNumber;
                    m_latchPinsArray[j].guiName = m_latchPinsArray[i].guiName;
                }
                break;
            }
        }
        // update shiftreg ui
        for (uint i = 0; i < m_latchPinsArray.size() - 1; ++i) {
            m_shiftRegsPtrList[i]->setLatchPin(m_latchPinsArray[i].pinNumber, m_latchPinsArray[i].guiName);
        }
    }
    // add shift reg data pin
    else if (dataPin != 0) {        // data_pin != 0 - необязательно
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

        std::sort(m_dataPinsArray.begin(), m_dataPinsArray.end(), sortByPinNumber);           // хз можно ли через 1 сорт
        std::stable_sort(m_dataPinsArray.begin(), m_dataPinsArray.end(), sortNullLast);

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
