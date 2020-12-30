#include "shiftregistersconfig.h"
#include "ui_shiftregistersconfig.h"

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
        //shift_register->hide();
        connect(shift_register, SIGNAL(buttonCountChanged(int, int)),
                this, SLOT(shiftRegButtonsCalc(int, int)));
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


void ShiftRegistersConfig::shiftRegButtonsCalc(int count, int previousCount)
{
    m_shiftButtonsCount += count - previousCount;
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
    if (latchPin != 0){
        if (latchPin > 0){
            m_latchPinsArray[m_latchPinsArray.size() - 1].pinNumber = latchPin;       // latch_pins_array.size() - 1// мб сделать отдельную переменную?
            m_latchPinsArray[m_latchPinsArray.size() - 1].guiName = pinGuiName;
        }
        // delete shift reg latch pin
        else
        {
            latchPin = -latchPin;
            for (uint i = 0; i < m_latchPinsArray.size(); ++i) {
                if (latchPin == m_latchPinsArray[i].pinNumber){

                    m_latchPinsArray[i].pinNumber = 0;
                    m_latchPinsArray[i].guiName = m_shiftRegsPtrList[0]->defaultText();      // hz
                }
            }
        }
        // sort ascending
        std::sort(m_latchPinsArray.begin(), m_latchPinsArray.end(), sortByPinNumber);           // хз можно ли через 1 сорт
        // sort null last
        std::stable_sort(m_latchPinsArray.begin(), m_latchPinsArray.end(), sortNullLast);
        //all unused pins = bigger pin
        for (int i = (int)m_latchPinsArray.size() - 1; i >= 0; --i) {         // бесконечный цикл, доработать
            if (m_latchPinsArray[i].pinNumber > 0){
                for (int j = (int)m_latchPinsArray.size() - 1; j > i; --j) {
                    m_latchPinsArray[j].pinNumber = m_latchPinsArray[i].pinNumber;
                    m_latchPinsArray[j].guiName = m_latchPinsArray[0].guiName;
                }
                break;
            }/* else if (i == 0){    // сделать нормально
                break;
            }*/
        }
        // update shiftreg ui
        for (uint i = 0; i < m_latchPinsArray.size() - 1; ++i) {
            m_shiftRegsPtrList[i]->setLatchPin(m_latchPinsArray[i].pinNumber, m_latchPinsArray[i].guiName);
        }

    }
    // add shift reg data pin
    else if (dataPin != 0){        // data_pin != 0 необязательно

        if (dataPin > 0){
            m_dataPinsArray[m_dataPinsArray.size() - 1].pinNumber = dataPin;
            m_dataPinsArray[m_dataPinsArray.size() - 1].guiName = pinGuiName;
        }
        else
        {
            dataPin = -dataPin;
            for (uint i = 0; i < m_dataPinsArray.size(); ++i) {
                if (dataPin == m_dataPinsArray[i].pinNumber){
                    m_dataPinsArray[i].pinNumber = 0;
                    m_dataPinsArray[i].guiName = m_shiftRegsPtrList[i]->defaultText();      // hz
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
