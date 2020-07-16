#include "encoders.h"
#include "ui_encoders.h"

Encoders::Encoders(int encoders_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Encoders)
{
    ui->setupUi(this);
    encoders_number_ = encoders_number + 1;         // fast encoder index 0
    ui->label_EncoderIndex->setNum(encoders_number_);

    for (int i = 0; i < ENCODER_TYPE_COUNT; ++i) {
        ui->comboBox_EncoderType->addItem(encoder_type_list_[i].gui_name);
        ui->label_ButtonNumberA->setText(not_defined);
        ui->label_ButtonNumberB->setText(not_defined);
    }
}

Encoders::~Encoders()
{
    delete ui;
}

void Encoders::RetranslateUi()
{
    ui->retranslateUi(this);
}

int Encoders::GetInputA()
{
    return input_A_;
}

int Encoders::GetInputB()
{
    return input_B_;
}

void Encoders::SetInputA(int input_A)
{
    if (input_A != 0){
        input_A_ = input_A;
        QString name_template("Button № %1");
        ui->label_ButtonNumberA->setText(name_template.arg(input_A_));
    } else {
        input_A_ = 0;
        ui->label_ButtonNumberA->setText(not_defined);
    }
    SetUiOnOff();
}

void Encoders::SetInputB(int input_B)
{
    if (input_B != 0){
        input_B_ = input_B;
        QString name_template("Button № %1");
        ui->label_ButtonNumberB->setText(name_template.arg(input_B_));
    } else {
        input_B_ = 0;
        ui->label_ButtonNumberB->setText(not_defined);
    }
    SetUiOnOff();
}

void Encoders::SetUiOnOff()
{
    if (input_A_ > 0 && input_B_ > 0){
        for(auto&& child:this->findChildren<QWidget *>()){
        child->setEnabled(true);
        }
    } else {
        for(auto&& child:this->findChildren<QWidget *>()){
        child->setEnabled(false);
        }
    }
}

void Encoders::ReadFromConfig()
{
    ui->comboBox_EncoderType->setCurrentIndex(gEnv.pDeviceConfig->config.encoders[encoders_number_]);
}

void Encoders::WriteToConfig()
{
    gEnv.pDeviceConfig->config.encoders[encoders_number_] = ui->comboBox_EncoderType->currentIndex();
}
