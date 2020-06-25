#include "pinconfig.h"
#include "ui_pinconfig.h"

#define PINS_IN_GROUP_RANGE 16          // range A0-A15 = 16
#include <QDebug>
PinConfig::PinConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinConfig)
{
    ui->setupUi(this);

    int number = 1;
    QString name_template_pinA("widgetPin_PA%1");
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinA.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            pinComboBox->InitializationPins(number++);
            PinComboBoxPtrList.append(pinComboBox);
        }
        //       connect(pinComboBox, SIGNAL(item_changed()),  SLOT(pinConfChanged()));
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
    qDebug()<<"PIN COUNT = "<<PinComboBoxPtrList.size();

    for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            connect(PinComboBoxPtrList[i], SIGNAL(valueChanged(int, int, int)),
                        this, SLOT(PinInteraction(int, int, int)));
    }
}

PinConfig::~PinConfig()
{
    delete ui;
}

void PinConfig::PinInteraction(int index, int sender_index, int pin)
{
    //sender();
    if (index != NOT_USED)//current_enum_index
    {
        for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            for (size_t j = 0; j < PinComboBoxPtrList[i]->enum_gui_index.size(); ++j) {
                if (PinComboBoxPtrList[i]->enum_gui_index[j] == index)
                {
                    //qDebug()<< index;
                    if(PinComboBoxPtrList[i]->interact_count_ == 0){
                        PinComboBoxPtrList[i]->interact_count_+= pin;
                        PinComboBoxPtrList[i]->SetIndex(j, sender_index);
                    }
                    else if (PinComboBoxPtrList[i]->is_interacts_ == true){
                        PinComboBoxPtrList[i]->interact_count_+= pin;
                    }
                    qDebug()<< PinComboBoxPtrList[i]->interact_count_;
                    //PinComboBoxPtrList[i]->SetIndex(j, sender_index);
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            if (PinComboBoxPtrList[i]->is_interacts_ == true)
            {
                //qDebug()<<"pin "<< PinComboBoxPtrList[i]->pin_number_ << i;
                for (size_t j = 0; j < PinComboBoxPtrList[i]->enum_gui_index.size(); ++j) {
                    //qDebug()<<"enum_gui " <<PinComboBoxPtrList[i]->enum_gui_index[j];
                    if (PinComboBoxPtrList[i]->enum_gui_index[j] == sender_index)
                    {
                        //qDebug()<<"enum_gui " <<PinComboBoxPtrList[i]->enum_gui_index[j];
                        //qDebug()<<"sender " <<sender_index;
                        //qDebug()<<"before "<< PinComboBoxPtrList[i]->interact_count_;
                        if(PinComboBoxPtrList[i]->interact_count_ > 0){
                            PinComboBoxPtrList[i]->interact_count_-= pin;
                        }
                        if (PinComboBoxPtrList[i]->interact_count_ <= 0) {
                            //PinComboBoxPtrList[i]->interact_count_-=sender_index;
                            PinComboBoxPtrList[i]->SetIndex(0, sender_index);
                        }
                        //qDebug()<<"after "<< PinComboBoxPtrList[i]->interact_count_;
                    }
                }
            }
        }
    }
}

void PinConfig::ReadFromConfig(){
//    for (uint i = 0; i < pin_count_; ++i) {
//        PinComboBoxPtrList[i]->ReadFromConfig(i);
//    }
}

void PinConfig::WriteToConfig(){
//    for (uint i = 0; i < pin_count_; ++i) {
//        PinComboBoxPtrList[i]->WriteToConfig(i);
//    }
}

