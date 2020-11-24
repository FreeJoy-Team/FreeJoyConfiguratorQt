#include "encoders.h"
#include "ui_encoders.h"

Encoders::Encoders(int encodersNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Encoders)
{
    ui->setupUi(this);
    m_input_A = 0;
    m_input_B = 0;
    m_notDefined = tr("Not defined");

    m_encodersNumber = encodersNumber + 1; // fast encoder index 0
    ui->label_EncoderIndex->setNum(m_encodersNumber);

    for (int i = 0; i < ENCODER_TYPE_COUNT; ++i) {
        ui->comboBox_EncoderType->addItem(m_encoderTypeList[i].guiName);
        ui->label_ButtonNumberA->setText(m_notDefined);
        ui->label_ButtonNumberB->setText(m_notDefined);
    }
}

Encoders::~Encoders()
{
    delete ui;
}

void Encoders::retranslateUi()
{
    ui->retranslateUi(this);
}

int Encoders::inputA() const
{
    return m_input_A;
}

int Encoders::inputB() const
{
    return m_input_B;
}

void Encoders::setInputA(int input_A)
{
    if (input_A != 0) {
        m_input_A = input_A;
        QString name_template(tr("Button № %1"));
        ui->label_ButtonNumberA->setText(name_template.arg(m_input_A));
    } else {
        m_input_A = 0;
        ui->label_ButtonNumberA->setText(m_notDefined);
    }
    setUiOnOff();
}

void Encoders::setInputB(int input_B)
{
    if (input_B != 0) {
        m_input_B = input_B;
        QString name_template(tr("Button № %1"));
        ui->label_ButtonNumberB->setText(name_template.arg(m_input_B));
    } else {
        m_input_B = 0;
        ui->label_ButtonNumberB->setText(m_notDefined);
    }
    setUiOnOff();
}

void Encoders::setUiOnOff()
{
    if (m_input_A > 0 && m_input_B > 0) {
        for (auto &&child : this->findChildren<QWidget *>()) {
            child->setEnabled(true);
        }
    } else {
        for (auto &&child : this->findChildren<QWidget *>()) {
            child->setEnabled(false);
        }
    }
}

void Encoders::readFromConfig()
{
    ui->comboBox_EncoderType->setCurrentIndex(gEnv.pDeviceConfig->config.encoders[m_encodersNumber]);
}

void Encoders::writeToConfig()
{
    gEnv.pDeviceConfig->config.encoders[m_encodersNumber] = ui->comboBox_EncoderType->currentIndex();
}
