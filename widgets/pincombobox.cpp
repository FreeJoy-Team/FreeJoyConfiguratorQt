#include "pincombobox.h"
#include "ui_pincombobox.h"

PinComboBox::PinComboBox(QWidget *parent) :     // пины - первое, что я начал кодить в конфигураторе и спустя время
    QWidget(parent),                            // заявляю - это говнокод!1 который даже мне тяжело понять
    ui(new Ui::PinComboBox)                     // мои соболезнования тем кто будет разбираться)
{
    ui->setupUi(this);
    m_pinNumber = -1;
    m_previousIndex = 0;
    m_interactCount = 0;
    m_currentDevEnum = 0;

    m_isCall_Interaction = false;
    m_isInteracts = false;

    m_styleSheetDefault = ui->comboBox_PinsType->styleSheet();       // will not work with styles, remove from constructor, вроде пашет

    connect(ui->comboBox_PinsType, SIGNAL(currentIndexChanged(int)),
                this, SLOT(IndexChanged(int)));
}

PinComboBox::~PinComboBox()
{
    m_pinTypesIndex.clear();         // Fault tolerant heap
    m_pinTypesIndex.shrink_to_fit(); // Fault tolerant heap
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

//! номер элемента в pin_types
const QVector<int> & PinComboBox::pinTypeIndex() const
{
    return m_pinTypesIndex;
}
//! device enum присутствующие в комбобоксе
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
        ui->comboBox_PinsType->setStyleSheet(m_styleSheetDefault);
    }
}

void PinComboBox::setIndex_iteraction(int index, int senderIndex)
{
    if(m_isInteracts == false && m_isCall_Interaction == false)     // ui->comboBox_PinsType->isEnabled()
    {
        if(m_pinTypes[m_pinTypesIndex[index]].device_enum_index != TLE5011_GEN)
        {
            ui->comboBox_PinsType->setEnabled(false);
        }
        m_isInteracts = true;
        ui->comboBox_PinsType->setStyleSheet(m_pinTypes[senderIndex].styleSheet);
        ui->comboBox_PinsType->setCurrentIndex(index);
    }
    else if (m_isInteracts == true)// && interact_count_ <= 0)
    {
        ui->comboBox_PinsType->setEnabled(true);
        m_isInteracts = false;
        ui->comboBox_PinsType->setStyleSheet(m_styleSheetDefault);
        ui->comboBox_PinsType->setCurrentIndex(index);
    }
}

//! Set pin items
void PinComboBox::initializationPins(uint pin)      // pin_number_ - 1 так се
{                                                   // это из-за того, что пустые значения структуры const cBox pin_types_[PIN_TYPE_COUNT]
    m_pinNumber = pin;                              // инициализированы 0 и хер поймёшь код. переделаю, если будет не лень

    for (int i = 0; i < PIN_TYPE_COUNT; ++i) {
        // except
        for (int c = 0; c < 10; ++c) {
            if (m_pinTypes[i].pin_except[c] == 0){
                break;
            }
            if (m_pinTypes[i].pin_except[c] == m_pinNumber){
                ++i;
                break;
            }
            for (int k = 0; k < 10; ++k)
            {
                if (m_pinList[m_pinNumber-1].pin_type[k] == 0){
                    break;
                }
                if (m_pinTypes[i].pin_except[c] == m_pinList[m_pinNumber-1].pin_type[k]){
                    ++i;
                    break;
                }
            }
        }
        // add
        for (int j = 0; j < 10; ++j) {
            if (m_pinTypes[i].pin_type[j] == 0) {
                break;
            }
            if (m_pinTypes[i].pin_type[j] == ALL){
                ui->comboBox_PinsType->addItem(m_pinTypes[i].gui_name);
                m_pinTypesIndex.push_back(i);
                m_enumIndex.push_back(m_pinTypes[i].device_enum_index);
                continue;
            }
            if (m_pinTypes[i].pin_type[j] == m_pinNumber){
                ui->comboBox_PinsType->addItem(m_pinTypes[i].gui_name);
                m_pinTypesIndex.push_back(i);
                m_enumIndex.push_back(m_pinTypes[i].device_enum_index);
                continue;
            }
            for (int k = 0; k < 10; ++k)
            {
                if (m_pinList[m_pinNumber-1].pin_type[k] == 0){
                    break;
                }
                if (m_pinTypes[i].pin_type[j] == m_pinList[m_pinNumber-1].pin_type[k]){
                    ui->comboBox_PinsType->addItem(m_pinTypes[i].gui_name);
                    m_pinTypesIndex.push_back(i);
                    m_enumIndex.push_back(m_pinTypes[i].device_enum_index);
                }
            }
        }
    }
}

void PinComboBox::IndexChanged(int index)
{

    if(!m_pinTypesIndex.empty() && m_isInteracts == false)
    {
        ui->comboBox_PinsType->setStyleSheet(m_pinTypes[m_pinTypesIndex[index]].styleSheet);      // временно?

        int tmp = 0;
        for (int i = 0; i < 10; ++i) {
            if (m_isCall_Interaction == true && tmp == 0)
            {
                m_isCall_Interaction = false;
                // add
                if (m_pinTypes[m_pinTypesIndex[index]].interaction[i] > 0)
                {
                    m_isCall_Interaction = true;
                    ui->comboBox_PinsType->setStyleSheet(m_pinTypes[m_pinTypesIndex[index]].styleSheet);          // ?????
                    for (int t = 0; t < 10; ++t)
                    {
                        if (m_pinTypes[m_pinTypesIndex[index]].interaction[t] > 0)
                        {
                            for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                                if(m_pinTypes[k].device_enum_index == m_pinTypes[m_pinTypesIndex[index]].interaction[t]){
                                    emit valueChangedForInteraction(k, m_pinTypesIndex[index], m_pinNumber);
                                    break;
                                }
                            }
                        }
                    }
                }
                // delete
                for (int n = 0; n < 10; ++n) {
                    if (m_pinTypes[m_call_interaction].interaction[n] <= 0){
                        break;
                    }
                    if(m_pinTypes[m_call_interaction].interaction[n] > 0){

                        for (int m = 0; m < PIN_TYPE_COUNT; ++m) {
                            if(m_pinTypes[m].device_enum_index == m_pinTypes[m_call_interaction].interaction[n]){
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
                ui->comboBox_PinsType->setStyleSheet(m_pinTypes[m_pinTypesIndex[index]].styleSheet);        // ????
                for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                    if(m_pinTypes[k].device_enum_index == m_pinTypes[m_pinTypesIndex[index]].interaction[i]){
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
    if(!m_pinTypesIndex.empty()){
        emit currentIndexChanged(m_enumIndex[index], m_previousIndex, m_pinNumber);
        m_previousIndex = m_enumIndex[index];
        m_currentDevEnum = m_enumIndex[index];
    }

}


void PinComboBox::readFromConfig(uint pin)          // try?
{
    for (int i = 0; i < m_enumIndex.size(); ++i) {
        if (gEnv.pDeviceConfig->config.pins[pin] == m_enumIndex[i])
        {
            ui->comboBox_PinsType->setCurrentIndex(int(i));
            break;
        }
    }
}

void PinComboBox::writeToConfig(uint pin)           // if pin = 0 try?
{   
    gEnv.pDeviceConfig->config.pins[pin] = m_currentDevEnum;
}
