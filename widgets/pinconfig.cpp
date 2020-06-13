#include "pinconfig.h"
#include "ui_pinconfig.h"

#define PINS_IN_GROUP_RANGE 16          // range A0-A15 = 16
#include <QDebug>
PinConfig::PinConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinConfig)
{
    ui->setupUi(this);

    //QList<PinComboBox *> allPButtons = this->findChildren<PinComboBox *>();

    // search and initializate pins PA
    QString name_template_pinA("widgetPin_PA%1");
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinA.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            pinComboBox->Initialization_pinA(i);
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
            pinComboBox->Initialization_pinB(i);
            PinComboBoxPtrList.append(pinComboBox);
        }
    }
    // search and initializate pins PC
    QString name_template_pinC("widgetPin_PC%1");
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinC.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            pinComboBox->Initialization_pinC(i);
            PinComboBoxPtrList.append(pinComboBox);
        }
    }
    qDebug()<<"PIN COUNT = "<<PinComboBoxPtrList.size();

    for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            connect(PinComboBoxPtrList[i], SIGNAL(valueChanged()),
                        this, SLOT(setCurrentConfig()));
    }
//    connect(ui->comboBox_PinsType, SIGNAL(currentIndexChanged(int)),
//                this, SLOT(IndexToEnum(int)));
}

PinConfig::~PinConfig()
{
    delete ui;
}

void PinConfig::setCurrentConfig()        // бляя // I STOP HERE
{
    //int qwe;
    for (uint i = 0; i < pin_count_; ++i) {
        qDebug()<< PinComboBoxPtrList[i]->GetEnumValue();
    }
    //PinComboBoxPtrList[1]->
    //currentIndexChanged
    //qDebug()<< ++qwe;
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

