#include "pinconfig.h"
#include "ui_pinconfig.h"
#include "signalhandler.h"

#define PINS_IN_GROUP_RANGE 16          // range A0-A15 = 16
#include <QDebug>
PinConfig::PinConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinConfig)
{
    ui->setupUi(this);

    int number = 1;
    QString name_template_pinA("widgetPin_PA%1");
    for(uint8_t i = 0; i < 16; i++) {                   // 16 to PINS_IN_GROUP_RANGE
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinA.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            pinComboBox->InitializationPins(number++);
            PinComboBoxPtrList.append(pinComboBox);
        }
    }
    // search and initializate pins PB
    QString name_template_pinB("widgetPin_PB%1");
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinB.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            pinComboBox->InitializationPins(number++);
            PinComboBoxPtrList.append(pinComboBox);
        }
    }
    // search and initializate pins PC
    QString name_template_pinC("widgetPin_PC%1");
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinC.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            pinComboBox->InitializationPins(number++);
            PinComboBoxPtrList.append(pinComboBox);
        }
    }
    //qDebug()<<"PIN COUNT = "<<PinComboBoxPtrList.size();    //currentIndexChanged a2bCountChanged

    for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            connect(PinComboBoxPtrList[i], SIGNAL(valueChangedForInteraction(int, int, int)),
                        this, SLOT(PinInteraction(int, int, int)));
            connect(PinComboBoxPtrList[i], SIGNAL(currentIndexChanged(int, int)),
                        this, SLOT(SetCurrentConfig(int, int)));
    }
    connect(gEnv.pSignalHandler, SIGNAL(a2bCountChanged(int, int)),     // временно
                this, SLOT(SetCurrentConfig(int, int)));
//    connect(dynamic_cast<SignalHandler*>(qApp), &SignalHandler::a2bCountChanged,
//            this, &PinConfig::SetCurrentConfig);
}

PinConfig::~PinConfig()
{
    delete ui;
}

void PinConfig::PinInteraction(int index, int sender_index, int pin)
{
    if (index != NOT_USED)//current_enum_index
    {
        for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            for (size_t j = 0; j < PinComboBoxPtrList[i]->enum_gui_index.size(); ++j) {
                if (PinComboBoxPtrList[i]->enum_gui_index[j] == index)
                {
                    if(PinComboBoxPtrList[i]->interact_count_ == 0){
                        PinComboBoxPtrList[i]->interact_count_+= pin;
                        PinComboBoxPtrList[i]->SetIndex(j, sender_index);
                    }
                    else if (PinComboBoxPtrList[i]->is_interacts_ == true){
                        PinComboBoxPtrList[i]->interact_count_+= pin;
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            if (PinComboBoxPtrList[i]->is_interacts_ == true)
            {
                for (size_t j = 0; j < PinComboBoxPtrList[i]->enum_gui_index.size(); ++j) {
                    if (PinComboBoxPtrList[i]->enum_gui_index[j] == sender_index)
                    {
                        if(PinComboBoxPtrList[i]->interact_count_ > 0){
                            PinComboBoxPtrList[i]->interact_count_-= pin;
                        }
                        if (PinComboBoxPtrList[i]->interact_count_ <= 0) {
                            PinComboBoxPtrList[i]->SetIndex(0, sender_index);
                        }
                    }
                }
            }
        }
    }
}

// 678 - так се реализация
void PinConfig::SetCurrentConfig(int current_device_enum, int previous_device_enum) // mutex
{
    qDebug()<< current_device_enum;
    if (current_device_enum == 678){            // 678 в DeviceConfig
        ui->label_ButtonFromAxes->setNum(previous_device_enum);
    } else {

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < PIN_TYPE_COUNT; ++j) {
                if(source[i].pin_type[j] == 0){
                    break;
                }
                else if(source[i].pin_type[j] == current_device_enum || source[i].pin_type[j] == previous_device_enum){

                    int tmp;
                    if (source[i].pin_type[j] == current_device_enum){
                        tmp = 1;
                    } else {
                        tmp = -1;
                    }

                    if (i == AXIS_SOURCE){
                        axis_sources_+=tmp;
                        ui->label_AxisSources->setNum(axis_sources_);
                    }
//                    else if (i == BUTTON_FROM_AXES){
//                        buttons_from_axes_+=tmp;
//                    }
                    else if (i == SINGLE_BUTTON){
                        single_buttons_+=tmp;
                        ui->label_SingleButtons->setNum(single_buttons_);
                        ui->label_TotalButtons->setNum(single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                        emit totalButtonsValueChanged(single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                    }
                    else if (i == ROW_OF_BUTTONS){
                        rows_of_buttons_+=tmp;
                        ui->label_RowsOfButtons->setNum(rows_of_buttons_);
                        ui->label_TotalButtons->setNum(single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                        emit totalButtonsValueChanged(single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                    }
                    else if (i == COLUMN_OF_BUTTONS){
                        columns_of_buttons_+=tmp;
                        ui->label_ColumnsOfButtons->setNum(columns_of_buttons_);
                        ui->label_TotalButtons->setNum(single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                        emit totalButtonsValueChanged(single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                    }
                    else if (i == SINGLE_LED){
                        single_LED_+=tmp;
                        ui->label_TotalLEDs->setNum(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                        emit totalLEDsValueChanged(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                    }
                    else if (i == ROW_OF_LED){
                        rows_of_LED_+=tmp;
                        ui->label_TotalLEDs->setNum(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                        emit totalLEDsValueChanged(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                    }
                    else if (i == COLUMN_OF_LED){
                        columns_of_LED_+=tmp;
                        ui->label_TotalLEDs->setNum(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                        emit totalLEDsValueChanged(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                    }
                }
            }
        }
    }
}


void PinConfig::ReadFromConfig(){
    for (uint i = 0; i < pin_count_; ++i) {
        PinComboBoxPtrList[i]->ReadFromConfig(i);
    }
}

void PinConfig::WriteToConfig(){
    for (uint i = 0; i < pin_count_; ++i) {
        PinComboBoxPtrList[i]->WriteToConfig(i);
    }
}

