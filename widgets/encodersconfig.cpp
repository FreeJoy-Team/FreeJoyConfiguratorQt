#include "encodersconfig.h"
#include "ui_encodersconfig.h"

#define FAST_ENCODER_COUNT 1
EncodersConfig::EncodersConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EncodersConfig)
{
    ui->setupUi(this);

    for (int i = 1; i < ENCODER_TYPE_COUNT; ++i) {      // i = 1 - fast encoder without ENCODER_CONF_1x
        ui->comboBox_EncoderType->addItem(fastEncoder_type_list_[i].gui_name);
        ui->label_ButtonNumberA->setText(not_defined);
        ui->label_ButtonNumberB->setText(not_defined);
    }

    ui->layoutV_Encoders->setAlignment(Qt::AlignTop);
    // encoders spawn
    for (int i = 0; i < MAX_ENCODERS_NUM - FAST_ENCODER_COUNT; i++)
    {
        Encoders * encoder = new Encoders(i, this);
        ui->layoutV_Encoders->addWidget(encoder);
        EncodersAdrList.append(encoder);
        //encoder->hide();
    }
}

EncodersConfig::~EncodersConfig()
{
    delete ui;
}

void EncodersConfig::fastEncoderSelected(QString pin_gui_name, bool is_selected)
{
    if (is_selected == true){
        if (ui->label_ButtonNumberA->text() == not_defined){        // хз чё будет с другим языком
            ui->label_ButtonNumberA->setText(pin_gui_name);
            fast_encoder_input_A_++;
        } else {
            ui->label_ButtonNumberB->setText(pin_gui_name);
            fast_encoder_input_B_++;
        }
    } else {
        if (ui->label_ButtonNumberA->text() == pin_gui_name){
            ui->label_ButtonNumberA->setText(not_defined);
            fast_encoder_input_A_--;
        } else {
            ui->label_ButtonNumberB->setText(not_defined);
            fast_encoder_input_B_--;
        }
    }
    SetUiOnOff();
}

void EncodersConfig::SetUiOnOff()
{
    if (fast_encoder_input_A_ > 0 && fast_encoder_input_B_ > 0){
        for(auto&& child:ui->groupBox_FastEncoder->findChildren<QWidget *>()){
        child->setEnabled(true);
        }
    } else {
        for(auto&& child:ui->groupBox_FastEncoder->findChildren<QWidget *>()){
        child->setEnabled(false);
        }
    }
}

// WARNING GOVNOKOD !!!!
void EncodersConfig::encoderInputChanged(int encoder_A, int encoder_B)      // ограничитель на макс энкодер или через сортировку
{
    int tmp_add = 0;
    int tmp_delete = 0;

    // add encoder A input
    if (encoder_A > 0)
    {
        encoders_input_A_count_++;
        for (int i = 0; i < encoders_input_A_count_; ++i)
        {
            if (encoder_A < EncodersAdrList[i]->GetInputA() || EncodersAdrList[i]->GetInputA() == 0)    // encoder_A != 0 && ( legacy
            {
                if (EncodersAdrList[i]->GetInputA() != 0)
                {
                    tmp_add = EncodersAdrList[i]->GetInputA();
                    EncodersAdrList[i]->SetInputA(encoder_A);
                    encoder_A = tmp_add;
                }
                else if (EncodersAdrList[i]->GetInputA() == 0)
                {
                    EncodersAdrList[i]->SetInputA(encoder_A);
                }
            }
        }
    }
    // delete encoder A input
    else if (encoder_A < 0)
    {
        encoder_A = -encoder_A;
        for (int i = 0; i < encoders_input_A_count_; ++i)
        {
            if (EncodersAdrList[i]->GetInputA() == encoder_A)   //encoder_A != 0 && (
            {
                for (int j = i; j < encoders_input_A_count_; ++j)
                {
                    tmp_delete = EncodersAdrList[j+1]->GetInputA();
                    EncodersAdrList[j]->SetInputA(EncodersAdrList[j+1]->GetInputA());
                    encoder_A = tmp_delete;
                }
                break;
            }
        }
        encoders_input_A_count_--;
    }

    // add encoder B input
    if (encoder_B > 0)              // else?
    {
        encoders_input_B_count_++;

        for (int i = 0; i < encoders_input_B_count_; ++i)
        {
            if (encoder_B < EncodersAdrList[i]->GetInputB() || EncodersAdrList[i]->GetInputB() == 0)        //encoder_B != 0 && (
            {
                if (EncodersAdrList[i]->GetInputB() != 0)
                {
                    tmp_add = EncodersAdrList[i]->GetInputB();
                    EncodersAdrList[i]->SetInputB(encoder_B);
                    encoder_B = tmp_add;
                }
                else if (EncodersAdrList[i]->GetInputB() == 0)
                {
                    EncodersAdrList[i]->SetInputB(encoder_B);
                }
            }
        }
    }
    // delete encoder B input
    else if (encoder_B < 0)
    {
        encoder_B = -encoder_B;
        for (int i = 0; i < encoders_input_B_count_; ++i)
        {
            if (EncodersAdrList[i]->GetInputB() == encoder_B)       //encoder_B != 0 &&
            {
                for (int j = i; j < encoders_input_B_count_; ++j)
                {
                    tmp_delete = EncodersAdrList[j+1]->GetInputB();
                    EncodersAdrList[j]->SetInputB(EncodersAdrList[j+1]->GetInputB());
                    encoder_B = tmp_delete;
                }
                break;
            }
        }
        encoders_input_B_count_--;
    }
}

void EncodersConfig::ReadFromConfig()
{
    ui->comboBox_EncoderType->setCurrentIndex(gEnv.pDeviceConfig->config.encoders[0] - 1);      // - 1 - fast encoder without ENCODER_CONF_1x

    for (int i = 0; i < MAX_ENCODERS_NUM - FAST_ENCODER_COUNT; i++)
    {
        EncodersAdrList[i]->ReadFromConfig();
    }
}

void EncodersConfig::WriteToConfig()
{
    gEnv.pDeviceConfig->config.encoders[0] = ui->comboBox_EncoderType->currentIndex() + 1;      // + 1 - fast encoder without ENCODER_CONF_1x
    for (int i = 0; i < MAX_ENCODERS_NUM - FAST_ENCODER_COUNT; i++)
    {
        EncodersAdrList[i]->WriteToConfig();
    }
}
