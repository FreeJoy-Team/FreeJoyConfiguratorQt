#include "pinconfig.h"
#include "ui_pinconfig.h"

//#include <QtConcurrent/QtConcurrent>
//#include <QFuture>

#define PINS_IN_GROUP_RANGE 16          // range A0-A15 = 16
#include <QDebug>
PinConfig::PinConfig(QWidget *parent) :         // пины - первое, что я начал кодить в конфигураторе и спустя время
    QWidget(parent),                            // заявляю - это говнокод!1 который даже мне тяжело понять
    ui(new Ui::PinConfig)                       // мои соболезнования тем кто будет разбираться)
{
    ui->setupUi(this);
    m_maxButtonsWarning = false;
    m_pinCount = 0;
    m_shiftLatchCount = 0;
    m_shiftDataCount = 0;

    m_axisSources = 0;
    m_buttonsFromAxes = 0;
    m_buttonsFromShiftRegs = 0;
    m_singleButtons = 0;
    m_rowsOfButtons = 0;
    m_columnsOfButtons = 0;
    m_singleLed = 0;
    m_rowsOfLed = 0;
    m_columnsOfLed = 0;

    int number = 1;
    QString name_template_pinA(QStringLiteral("widgetPin_PA%1"));//QtConcurrent::run()
    for(uint8_t i = 0; i < 16; i++) {                   // 16 to PINS_IN_GROUP_RANGE
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinA.arg(i));
        if (pinComboBox != nullptr){
            m_pinCount++;
            //QFuture<void> future = QtConcurrent::run(pinComboBox, &PinComboBox::InitializationPins, number++);      // hz
            pinComboBox->initializationPins(number++);
            m_pinCBoxPtrList.append(pinComboBox);
        }
    }
    // search and initializate pins PB
    QString name_template_pinB(QStringLiteral("widgetPin_PB%1"));
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinB.arg(i));
        if (pinComboBox != nullptr){
            m_pinCount++;
            //QFuture<void> future = QtConcurrent::run(pinComboBox, &PinComboBox::InitializationPins, number++);      // hz
            pinComboBox->initializationPins(number++);
            m_pinCBoxPtrList.append(pinComboBox);
        }
    }
    // search and initializate pins PC
    QString name_template_pinC(QStringLiteral("widgetPin_PC%1"));
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinC.arg(i));
        if (pinComboBox != nullptr){
            m_pinCount++;
            //QFuture<void> future = QtConcurrent::run(pinComboBox, &PinComboBox::InitializationPins, number++);      // hz
            pinComboBox->initializationPins(number++);
            m_pinCBoxPtrList.append(pinComboBox);
        }
    }

    for (int i = 0; i < m_pinCBoxPtrList.size(); ++i) {
            connect(m_pinCBoxPtrList[i], SIGNAL(valueChangedForInteraction(int, int, int)),       // valgrind сообщает о утечке, но почему?
                        this, SLOT(pinInteraction(int, int, int)));
            connect(m_pinCBoxPtrList[i], SIGNAL(currentIndexChanged(int, int, int)),
                        this, SLOT(pinIndexChanged(int, int, int)));
    }
    connect(this, SIGNAL(totalButtonsValueChanged(int)),
            this, SLOT(totalButtonsChanged(int)));

    if (m_pinCount != PINS_COUNT){
        qCritical()<<"pinconfig.cpp pin_count_ != PINS_COUNT";
    }
}

PinConfig::~PinConfig()
{
    delete ui;
}

void PinConfig::retranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < m_pinCBoxPtrList.size(); ++i) {
        m_pinCBoxPtrList[i]->retranslateUi();
    }
}

void PinConfig::pinInteraction(int index, int senderIndex, int pin)
{
    if (index != NOT_USED)//current_enum_index
    {
        for (int i = 0; i < m_pinCBoxPtrList.size(); ++i) {
            for (int j = 0; j < m_pinCBoxPtrList[i]->pinTypeIndex().size(); ++j) {
                if (m_pinCBoxPtrList[i]->pinTypeIndex()[j] == index)
                {
                    if(m_pinCBoxPtrList[i]->interactCount() == 0){
                        // как-то некрасиво
                        m_pinCBoxPtrList[i]->setInteractCount(m_pinCBoxPtrList[i]->interactCount() + pin);
                        m_pinCBoxPtrList[i]->setIndex_iteraction(j, senderIndex);
                    }
                    else if (m_pinCBoxPtrList[i]->isInteracts() == true){
                        m_pinCBoxPtrList[i]->setInteractCount(m_pinCBoxPtrList[i]->interactCount() + pin);
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < m_pinCBoxPtrList.size(); ++i) {
            if (m_pinCBoxPtrList[i]->isInteracts() == true)
            {
                for (int j = 0; j < m_pinCBoxPtrList[i]->pinTypeIndex().size(); ++j) {
                    if (m_pinCBoxPtrList[i]->pinTypeIndex()[j] == senderIndex)
                    {
                        if(m_pinCBoxPtrList[i]->interactCount() > 0){
                            m_pinCBoxPtrList[i]->setInteractCount(m_pinCBoxPtrList[i]->interactCount() - pin);
                        }
                        if (m_pinCBoxPtrList[i]->interactCount() <= 0) {
                            m_pinCBoxPtrList[i]->setIndex_iteraction(0, senderIndex);
                        }
                    }
                }
            }
        }
    }
}


void PinConfig::pinIndexChanged(int currentDeviceEnum, int previousDeviceEnum, int pinNumber)              // мб сделать сразу запись в конфиг из пинов
{                                                                                                               // или отдельный класс для их состояний
    // signals for another widgets
    signalsForWidgets(currentDeviceEnum, previousDeviceEnum, pinNumber);

    // pin type limit           // переизбыток функционала(изи менять в структуре), не думаю, что понадобится в будущем, можно было и захардкодить
    pinTypeLimit(currentDeviceEnum, previousDeviceEnum);

    // set current config and generate signals for another widgets
//    else {
    setCurrentConfig(currentDeviceEnum, previousDeviceEnum, pinNumber);
    //}
}


void PinConfig::signalsForWidgets(int currentDeviceEnum, int previousDeviceEnum, int pinNumber)
{
    //fast encoder selected
    if (currentDeviceEnum == FAST_ENCODER){
        emit fastEncoderSelected(m_pinCBoxPtrList[0]->pinList()[pinNumber - PA_0].gui_name, true);    // hz
    } else if (previousDeviceEnum == FAST_ENCODER){
        emit fastEncoderSelected(m_pinCBoxPtrList[0]->pinList()[pinNumber - PA_0].gui_name, false);    // hz
    }
    // shift register latch selected
    if (currentDeviceEnum == SHIFT_REG_LATCH){
        m_shiftLatchCount++;
        emit shiftRegSelected(pinNumber, 0, m_pinCBoxPtrList[0]->pinList()[pinNumber - PA_0].gui_name);    // hz
    } else if (previousDeviceEnum == SHIFT_REG_LATCH){
        m_shiftLatchCount--;
        emit shiftRegSelected((pinNumber)*-1, 0, m_pinCBoxPtrList[0]->pinList()[pinNumber - PA_0].gui_name);    // hz
    }
    // shift register data selected
    if (currentDeviceEnum == SHIFT_REG_DATA){
        m_shiftDataCount++;
        emit shiftRegSelected(0, pinNumber, m_pinCBoxPtrList[0]->pinList()[pinNumber - PA_0].gui_name);    // hz
    } else if (previousDeviceEnum == SHIFT_REG_DATA){
        m_shiftDataCount--;
        emit shiftRegSelected(0, (pinNumber)*-1, m_pinCBoxPtrList[0]->pinList()[pinNumber - PA_0].gui_name);    // hz
    }
    // I2C selected
    if (currentDeviceEnum == I2C_SCL){// || current_device_enum == I2C_SDA){
        emit axesSourceChanged(-2, true);                                            // -2 enum I2C в axes.h
    } else if (previousDeviceEnum == I2C_SCL){// || previous_device_enum == I2C_SDA){
        emit axesSourceChanged(-2, false);
    }
}

void PinConfig::pinTypeLimit(int currentDeviceEnum, int previousDeviceEnum)
{
    static int limit_count_array[PIN_TYPE_LIMIT_COUNT]{};
    static bool limit_is_enable[PIN_TYPE_LIMIT_COUNT]{};

    for (int i = 0; i < PIN_TYPE_LIMIT_COUNT; ++i)
    {
        if (currentDeviceEnum == m_pinTypeLimit[i].deviceEnumIndex)
        {
            limit_count_array[i]++;
        }
        if (previousDeviceEnum == m_pinTypeLimit[i].deviceEnumIndex)
        {
            limit_count_array[i]--;
        }

        if (limit_count_array[i] >= m_pinTypeLimit[i].maxCount && limit_is_enable[i] == false)
        {
            limit_is_enable[i] = true;
            for (int j = 0; j < m_pinCBoxPtrList.size(); ++j)
            {
                for (int k = 0; k < m_pinCBoxPtrList[j]->enumIndex().size(); ++k) {
                    if (m_pinCBoxPtrList[j]->enumIndex()[k] == m_pinTypeLimit[i].deviceEnumIndex &&
                        m_pinCBoxPtrList[j]->currentDevEnum() != currentDeviceEnum)
                    {
                        m_pinCBoxPtrList[j]->setIndexStatus(int(k), false);
                    }
                }
            }
        }

        if (limit_is_enable[i] == true && limit_count_array[i] < m_pinTypeLimit[i].maxCount)
        {
            limit_is_enable[i] = false;
            for (int j = 0; j < m_pinCBoxPtrList.size(); ++j)
            {
                for (int k = 0; k < m_pinCBoxPtrList[j]->enumIndex().size(); ++k) {
                    if (m_pinCBoxPtrList[j]->enumIndex()[k] == m_pinTypeLimit[i].deviceEnumIndex)
                    {
                        m_pinCBoxPtrList[j]->setIndexStatus(int(k), true);
                    }
                }
            }
        }
    }
}

void PinConfig::setCurrentConfig(int currentDeviceEnum, int previousDeviceEnum, int pinNumber)
{
    for (int i = 0; i < SOURCE_COUNT; ++i) {
        for (int j = 0; j < PIN_TYPE_COUNT; ++j) {
            if(m_source[i].pinType[j] == 0){
                break;
            }
            else if(m_source[i].pinType[j] == currentDeviceEnum || m_source[i].pinType[j] == previousDeviceEnum){

                int tmp;
                if (m_source[i].pinType[j] == currentDeviceEnum){
                    tmp = 1;
                } else {
                    tmp = -1;
                }

                if (i == AXIS_SOURCE){      //int source_enum, bool is_add      axesSourceChanged
                    m_axisSources+=tmp;
                    ui->label_AxisSources->setNum(m_axisSources);
                    if (tmp > 0){
                        emit axesSourceChanged(pinNumber - 1, true);
                    } else {
                        emit axesSourceChanged(pinNumber - 1, false);
                    }
                }
                else if (i == SINGLE_BUTTON){
                    m_singleButtons+=tmp;
                    ui->label_SingleButtons->setNum(m_singleButtons);
                    ui->label_TotalButtons->setNum(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
                    emit totalButtonsValueChanged(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
                }
                else if (i == ROW_OF_BUTTONS){
                    m_rowsOfButtons+=tmp;
                    ui->label_RowsOfButtons->setNum(m_rowsOfButtons);
                    ui->label_TotalButtons->setNum(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
                    emit totalButtonsValueChanged(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
                }
                else if (i == COLUMN_OF_BUTTONS){
                    m_columnsOfButtons+=tmp;
                    ui->label_ColumnsOfButtons->setNum(m_columnsOfButtons);
                    ui->label_TotalButtons->setNum(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
                    emit totalButtonsValueChanged(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
                }
                else if (i == SINGLE_LED){
                    m_singleLed+=tmp;
                    ui->label_TotalLEDs->setNum(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
                    emit totalLEDsValueChanged(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
                }
                else if (i == ROW_OF_LED){
                    m_rowsOfLed+=tmp;
                    ui->label_TotalLEDs->setNum(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
                    emit totalLEDsValueChanged(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
                }
                else if (i == COLUMN_OF_LED){
                    m_columnsOfLed+=tmp;
                    ui->label_TotalLEDs->setNum(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
                    emit totalLEDsValueChanged(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
                }
            }
        }
    }
}



void PinConfig::resetAllPins()
{
    for (int i = 0; i < m_pinCBoxPtrList.size(); ++i) {
        m_pinCBoxPtrList[i]->resetPin();
    }
}

void PinConfig::a2bCountChanged(int count)
{
    m_buttonsFromAxes = count;
    ui->label_ButtonFromAxes->setNum(m_buttonsFromAxes);
    int total_buttons = m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons);
    ui->label_TotalButtons->setNum(total_buttons);
    emit totalButtonsValueChanged(total_buttons);
}

void PinConfig::shiftRegButtonsCountChanged(int count)
{
    m_buttonsFromShiftRegs = count;
    ui->label_ButtonsFromShiftRegs->setNum(m_buttonsFromShiftRegs);
    int total_buttons = m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons);
    ui->label_TotalButtons->setNum(total_buttons);
    emit totalButtonsValueChanged(total_buttons);
}

void PinConfig::totalButtonsChanged(int count)
{
    if (count > MAX_BUTTONS_NUM){
        m_defaultLabelStyle = ui->label_TotalButtons->styleSheet();
        //ui->label_TotalButtons->setStyleSheet(default_label_style_ + "background-color: rgb(200, 0, 0);");
        ui->text_TotalButtons->setStyleSheet(m_defaultLabelStyle + QStringLiteral("background-color: rgb(200, 0, 0);"));
        m_maxButtonsWarning = true;
    } else if (m_maxButtonsWarning == true){   // && count <= MAX_BUTTONS_NUM
        // а не будет ли тут проблемы, если кнопка активна и в это время изменился стиль приложения
        // то default_label_style_ будет же со старым стилем?
        ui->text_TotalButtons->setStyleSheet(m_defaultLabelStyle);
        m_maxButtonsWarning = false;
    }
}

void PinConfig::readFromConfig(){
    for (uint i = 0; i < m_pinCount; ++i) {
        m_pinCBoxPtrList[i]->readFromConfig(i);
    }
}

void PinConfig::writeToConfig(){
    for (uint i = 0; i < m_pinCount; ++i) {
        m_pinCBoxPtrList[i]->writeToConfig(i);
    }
}

