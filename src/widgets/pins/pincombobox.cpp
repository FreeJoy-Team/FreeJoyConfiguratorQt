#include "pincombobox.h"
#include "ui_pincombobox.h"

//! pinNumber cannot be less 1 and more than PINS_COUNT
PinComboBox::PinComboBox(uint pinNumber, QWidget *parent) : // пины - первое, что я начал кодить в конфигураторе и спустя время
    QWidget(parent),                                        // заявляю - это говнокод!1 который даже мне тяжело понять
    ui(new Ui::PinComboBox)                                 // мои соболезнования тем кто будет разбираться)
{
    ui->setupUi(this);
    // minimum pinNumber = enum PA_0 = 1
    if (pinNumber < 1 || pinNumber > PINS_COUNT) {
        qFatal("(pinNumber < 1 || pinNumber > PINS_COUNT) in pincombobox.cpp");
    }
    m_pinNumber = pinNumber;
    // set object name for for placement in PinConfig layout
    setObjectName(m_pinList[m_pinNumber-1].objectName);

    m_previousIndex = 0;
    m_interactCount = 0;
    m_currentDevEnum = 0;

    m_isCall_Interaction = false;
    m_isInteracts = false;

    initializationPins(m_pinNumber);

    connect(ui->comboBox_PinsType, SIGNAL(currentIndexChanged(int)),
                this, SLOT(indexChanged(int)));
}

PinComboBox::~PinComboBox()
{
    m_pinTypesIndex.clear();         // Fault tolerant heap??
    m_pinTypesIndex.shrink_to_fit(); // Fault tolerant heap??
    m_enumIndex.clear();
    m_enumIndex.shrink_to_fit();
    delete ui;
}

void PinComboBox::retranslateUi()
{
    ui->retranslateUi(this);
}

const pins *PinComboBox::pinList() const
{
    return m_pinList;
}


const QVector<int> & PinComboBox::pinTypeIndex() const
{
    return m_pinTypesIndex;
}

const QVector<int> & PinComboBox::enumIndex() const
{
    return m_enumIndex;
}

uint PinComboBox::interactCount() const
{
    return m_interactCount;
}

void PinComboBox::setInteractCount(const uint &count)
{
    m_interactCount = count;
}

bool PinComboBox::isInteracts() const
{
    return m_isInteracts;
}

int PinComboBox::currentDevEnum() const
{
    return m_currentDevEnum;
}
//! Set selected index enable or disable
void PinComboBox::setIndexStatus(int index, bool status)
{
    if (status == true){
        ui->comboBox_PinsType->setItemData(index, 1 | 32, Qt::UserRole - 1);
    } else {
        ui->comboBox_PinsType->setItemData(index, 0, Qt::UserRole - 1);
    }
}

void PinComboBox::resetPin()
{
    ui->comboBox_PinsType->setCurrentIndex(0);
    if (m_isInteracts == true)
    {
        ui->comboBox_PinsType->setEnabled(true);
        m_isInteracts = false;
    }
}

void PinComboBox::setIndex_iteraction(int index, int senderIndex)
{
    if(m_isInteracts == false && m_isCall_Interaction == false)     // ui->comboBox_PinsType->isEnabled()
    {
        if(m_pinTypes[m_pinTypesIndex[index]].deviceEnumIndex != TLE5011_GEN)
        {
            ui->comboBox_PinsType->setEnabled(false);
        }
        // change text color
        QPalette pal = ui->comboBox_PinsType->palette();
        pal.setColor(QPalette::ButtonText, m_pinTypes[senderIndex].color);
        ui->comboBox_PinsType->setPalette(pal);

        m_isInteracts = true;
        ui->comboBox_PinsType->setCurrentIndex(index);
    }
    else if (m_isInteracts == true)// && interact_count_ <= 0)
    {
        ui->comboBox_PinsType->setEnabled(true);
        m_isInteracts = false;
        // reset color
        ui->comboBox_PinsType->setPalette(palette());

        ui->comboBox_PinsType->setCurrentIndex(index);
    }
}

//! Set pin items
void PinComboBox::initializationPins(uint pin)      // pin_number_ - 1 так се
{                                                   // это из-за того, что пустые значения структуры const cBox pin_types_[PIN_TYPE_COUNT]
    m_pinNumber = pin;                              // инициализированы 0 и хер поймёшь код. возможно переделаю

    int typeExceptSize = sizeof(m_pinTypes->pinExcept) / sizeof(m_pinTypes->pinExcept[0]);
    int typeSize = sizeof(m_pinTypes->pinType) / sizeof(m_pinTypes->pinType[0]);
    int pinListTypeSize = sizeof(m_pinList->pinType) / sizeof(m_pinList->pinType[0]);
    bool tmp = false;

    for (int i = 0; i < PIN_TYPE_COUNT; ++i) {
        // except
        tmp = false;
        for (int c = 0; c < typeExceptSize; ++c) {
            if (m_pinTypes[i].pinExcept[c] == 0){
                break;
            }
            if (m_pinTypes[i].pinExcept[c] == m_pinNumber){
                //++i;  // нахера?
                tmp = true;
                break;
            }
            for (int k = 0; k < pinListTypeSize; ++k)
            {
                if (m_pinList[m_pinNumber-1].pinType[k] == 0){
                    break;
                }
                if (m_pinTypes[i].pinExcept[c] == m_pinList[m_pinNumber-1].pinType[k]){
                    //++i;  // нахера?
                    tmp = true;
                    break;
                }
            }
        }
        if (tmp){
            continue;
        }

        // add
        for (int j = 0; j < typeSize; ++j) {
            if (m_pinTypes[i].pinType[j] == 0) {
                break;
            }
            if (m_pinTypes[i].pinType[j] == ALL){
                ui->comboBox_PinsType->addItem(m_pinTypes[i].guiName);
                m_pinTypesIndex.push_back(i);
                m_enumIndex.push_back(m_pinTypes[i].deviceEnumIndex);
                continue;
            }
            if (m_pinTypes[i].pinType[j] == m_pinNumber){
                ui->comboBox_PinsType->addItem(m_pinTypes[i].guiName);
                m_pinTypesIndex.push_back(i);
                m_enumIndex.push_back(m_pinTypes[i].deviceEnumIndex);
                continue;
            }
            for (int k = 0; k < pinListTypeSize; ++k)
            {
                if (m_pinList[m_pinNumber-1].pinType[k] == 0){
                    break;
                }
                if (m_pinTypes[i].pinType[j] == m_pinList[m_pinNumber-1].pinType[k]){
                    ui->comboBox_PinsType->addItem(m_pinTypes[i].guiName);
                    m_pinTypesIndex.push_back(i);
                    m_enumIndex.push_back(m_pinTypes[i].deviceEnumIndex);
                }
            }
        }
    }
    // change items text color // i = 1 -skip "NOT_USED"
    for (int i = 1; i < m_pinTypesIndex.size(); ++i) {
        ui->comboBox_PinsType->setItemData(i, QBrush(m_pinTypes[m_pinTypesIndex[i]].color), Qt::ForegroundRole);
    }
}

void PinComboBox::indexChanged(int index)
{
    if(m_pinTypesIndex.empty() == false && m_isInteracts == false)
    {
        // change text color
        if (index == 0) {
            ui->comboBox_PinsType->setPalette(palette());
        } else {
            QPalette pal = ui->comboBox_PinsType->palette();
            pal.setColor(QPalette::ButtonText, m_pinTypes[m_pinTypesIndex[index]].color);
            ui->comboBox_PinsType->setPalette(pal);
        }

        int iteractionSize = sizeof(m_pinTypes->interaction) / sizeof(m_pinTypes->interaction[0]);
        int tmp = 0;
        for (int i = 0; i < iteractionSize; ++i) {
            if (m_isCall_Interaction == true && tmp == 0)
            {
                m_isCall_Interaction = false;
                // add
                if (m_pinTypes[m_pinTypesIndex[index]].interaction[i] > 0)
                {
                    m_isCall_Interaction = true;
                    for (int t = 0; t < 10; ++t)
                    {
                        if (m_pinTypes[m_pinTypesIndex[index]].interaction[t] > 0)
                        {
                            for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                                if(m_pinTypes[k].deviceEnumIndex == m_pinTypes[m_pinTypesIndex[index]].interaction[t]){
                                    emit valueChangedForInteraction(k, m_pinTypesIndex[index], m_pinNumber);
                                    break;
                                }
                            }
                        }
                    }
                }
                // delete
                for (int n = 0; n < iteractionSize; ++n) {
                    if (m_pinTypes[m_call_interaction].interaction[n] <= 0){
                        break;
                    }
                    if(m_pinTypes[m_call_interaction].interaction[n] > 0){

                        for (int m = 0; m < PIN_TYPE_COUNT; ++m) {
                            if(m_pinTypes[m].deviceEnumIndex == m_pinTypes[m_call_interaction].interaction[n]){
                                emit valueChangedForInteraction(NOT_USED, m, m_pinNumber);
                            }
                        }
                    }
                }

                m_call_interaction = m_pinTypesIndex[index];
                break;
            }

            // add
            else if (m_pinTypes[m_pinTypesIndex[index]].interaction[i] > 0)
            {
                m_isCall_Interaction = true;
                for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                    if(m_pinTypes[k].deviceEnumIndex == m_pinTypes[m_pinTypesIndex[index]].interaction[i]){
                        m_call_interaction = m_pinTypesIndex[index];
                        emit valueChangedForInteraction(k, m_pinTypesIndex[index], m_pinNumber);
                        tmp++;
                        break;
                    }
                }
            }
        }
    }

    // set current config
    if(m_pinTypesIndex.empty() == false){
        emit currentIndexChanged(m_enumIndex[index], m_previousIndex, m_pinNumber, ui->comboBox_PinsType->currentText());
        m_previousIndex = m_enumIndex[index];
        m_currentDevEnum = m_enumIndex[index];
    }

}


void PinComboBox::readFromConfig(uint pin)
{
    for (int i = 0; i < m_enumIndex.size(); ++i) {
        if (gEnv.pDeviceConfig->config.pins[pin] == m_enumIndex[i])
        {
            ui->comboBox_PinsType->setCurrentIndex(int(i));
            break;
        }
    }
}

void PinComboBox::writeToConfig(uint pin)
{   
    gEnv.pDeviceConfig->config.pins[pin] = m_currentDevEnum;
}
