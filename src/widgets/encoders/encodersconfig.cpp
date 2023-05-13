#include "encodersconfig.h"
#include "ui_encodersconfig.h"

#define FAST_ENCODER_COUNT 1

EncodersConfig::EncodersConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EncodersConfig)
{
    ui->setupUi(this);
    m_encodersInput_A_count = 0;
    m_encodersInput_B_count = 0;
    m_notDefined = tr("Not defined");

    m_fastEncoderInput_A = 0;
    m_fastEncoderInput_B = 0;

    for (int i = 1; i < ENCODER_TYPE_COUNT; ++i) {      // i = 1 - fast encoder without ENCODER_CONF_1x
        ui->comboBox_EncoderType->addItem(m_fastEncoderTypeList[i].guiName);
        ui->label_ButtonNumberA->setText(m_notDefined);
        ui->label_ButtonNumberB->setText(m_notDefined);
    }

    ui->layoutV_Encoders->setAlignment(Qt::AlignTop);
    // encoders spawn
    for (int i = 0; i < MAX_ENCODERS_NUM - FAST_ENCODER_COUNT; i++)
    {
        Encoders * encoder = new Encoders(i, this);
        ui->layoutV_Encoders->addWidget(encoder);
        m_encodersPtrList.append(encoder);
        //encoder->hide();
    }
}

EncodersConfig::~EncodersConfig()
{
    delete ui;
}

void EncodersConfig::retranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < m_encodersPtrList.size(); ++i) {
        m_encodersPtrList[i]->retranslateUi();
    }
}

void EncodersConfig::fastEncoderSelected(const QString &pinGuiName, bool isSelected)
{
    if (isSelected == true){
        if (ui->label_ButtonNumberA->text() == m_notDefined){
            ui->label_ButtonNumberA->setText(pinGuiName);
            m_fastEncoderInput_A++;
        } else {
            ui->label_ButtonNumberB->setText(pinGuiName);
            m_fastEncoderInput_B++;
        }
    } else {
        if (ui->label_ButtonNumberA->text() == pinGuiName){
            ui->label_ButtonNumberA->setText(m_notDefined);
            m_fastEncoderInput_A--;
        } else {
            ui->label_ButtonNumberB->setText(m_notDefined);
            m_fastEncoderInput_B--;
        }
    }
    setUiOnOff();
}

void EncodersConfig::setUiOnOff()
{
    if (m_fastEncoderInput_A > 0 && m_fastEncoderInput_B > 0){
        for(auto&& child:ui->groupBox_FastEncoder->findChildren<QWidget *>()){
        child->setEnabled(true);
        }
    } else {
        for(auto&& child:ui->groupBox_FastEncoder->findChildren<QWidget *>()){
        child->setEnabled(false);
        }
    }
}


void EncodersConfig::encoderInputChanged(int encoder_A, int encoder_B)      // говнокод
{
    int tmp_add = 0;

    // add encoder A input
    if (encoder_A > 0)
    {
        m_encodersInput_A_count++;
        for (int i = 0; i < m_encodersInput_A_count; ++i)
        {
            if (encoder_A < m_encodersPtrList[i]->inputA() || m_encodersPtrList[i]->inputA() == 0)    // encoder_A != 0 && ( legacy
            {
                if (m_encodersPtrList[i]->inputA() != 0)
                {
                    tmp_add = m_encodersPtrList[i]->inputA();
                    m_encodersPtrList[i]->setInputA(encoder_A);
                    encoder_A = tmp_add;
                }
                else if (m_encodersPtrList[i]->inputA() == 0)
                {
                    m_encodersPtrList[i]->setInputA(encoder_A);
                }
            }
        }
    }
    // delete encoder A input
    else if (encoder_A < 0)
    {
        encoder_A = -encoder_A;
        for (int i = 0; i < m_encodersInput_A_count; ++i)
        {
            if (m_encodersPtrList[i]->inputA() == encoder_A)   //encoder_A != 0 && (
            {
                for (int j = i; j < m_encodersInput_A_count; ++j)
                {
                    if (j+1 < m_encodersPtrList.size()) {
                        m_encodersPtrList[j]->setInputA(m_encodersPtrList[j+1]->inputA());
                    } else {
                        m_encodersPtrList[j]->setInputA(0);
                    }
                }
                break;
            }
        }
        m_encodersInput_A_count--;
    }

    // add encoder B input
    if (encoder_B > 0)              // else?
    {
        m_encodersInput_B_count++;

        for (int i = 0; i < m_encodersInput_B_count; ++i)
        {
            if (encoder_B < m_encodersPtrList[i]->inputB() || m_encodersPtrList[i]->inputB() == 0)        //encoder_B != 0 && (
            {
                if (m_encodersPtrList[i]->inputB() != 0)
                {
                    tmp_add = m_encodersPtrList[i]->inputB();
                    m_encodersPtrList[i]->setInputB(encoder_B);
                    encoder_B = tmp_add;
                }
                else if (m_encodersPtrList[i]->inputB() == 0)
                {
                    m_encodersPtrList[i]->setInputB(encoder_B);
                }
            }
        }
    }
    // delete encoder B input
    else if (encoder_B < 0)
    {
        encoder_B = -encoder_B;
        for (int i = 0; i < m_encodersInput_B_count; ++i)
        {
            if (m_encodersPtrList[i]->inputB() == encoder_B)       //encoder_B != 0 &&
            {
                for (int j = i; j < m_encodersInput_B_count; ++j)
                {
                    if (j+1 < m_encodersPtrList.size()) {
                        m_encodersPtrList[j]->setInputB(m_encodersPtrList[j+1]->inputB());
                    } else {
                        m_encodersPtrList[j]->setInputB(0);
                    }
                }
                break;
            }
        }
        m_encodersInput_B_count--;
    }
}

void EncodersConfig::readFromConfig()
{
    ui->comboBox_EncoderType->setCurrentIndex(gEnv.pDeviceConfig->config.encoders[0] - 1);      // - 1 - fast encoder without ENCODER_CONF_1x

    for (int i = 0; i < MAX_ENCODERS_NUM - FAST_ENCODER_COUNT; i++)
    {
        m_encodersPtrList[i]->readFromConfig();
    }
}

void EncodersConfig::writeToConfig()
{
    gEnv.pDeviceConfig->config.encoders[0] = ui->comboBox_EncoderType->currentIndex() + 1;      // + 1 - fast encoder without ENCODER_CONF_1x
    for (int i = 0; i < MAX_ENCODERS_NUM - FAST_ENCODER_COUNT; i++)
    {
        m_encodersPtrList[i]->writeToConfig();
    }
}
