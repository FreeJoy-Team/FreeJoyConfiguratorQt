#include "axestobuttonsconfig.h"
#include "ui_axestobuttonsconfig.h"

AxesToButtonsConfig::AxesToButtonsConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesToButtonsConfig)
{
    ui->setupUi(this);

    a2b_buttons_count_ = 0;
    ui->layoutV_AxesToButtons->setAlignment(Qt::AlignTop);

//    for (int i = 0; i < MAX_AXIS_NUM; ++i)
//    {
//        AxesToButtons* a2b = new AxesToButtons(this);
//        ui->layoutV_AxesToButtons->addWidget(a2b);
//        A2bAdrList.append(a2b);
//        connect(a2b, SIGNAL(a2bCountChanged(int, int)),
//                this, SLOT(a2bCountCalc(int, int)));
//    }
}

AxesToButtonsConfig::~AxesToButtonsConfig()
{
    delete ui;
}

void AxesToButtonsConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < A2bAdrList.size(); ++i) {
        A2bAdrList[i]->RetranslateUi();
    }
}

void AxesToButtonsConfig::a2bCountCalc(int count, int previous_count)
{
    a2b_buttons_count_ += count - previous_count;
    emit a2bCountChanged(a2b_buttons_count_);
}

void AxesToButtonsConfig::ReadFromConfig()
{
    for (int i = 0; i < A2bAdrList.size(); ++i) {
        A2bAdrList[i]->ReadFromConfig();
    }
}

void AxesToButtonsConfig::WriteToConfig()
{
    for (int i = 0; i < A2bAdrList.size(); ++i) {
        A2bAdrList[i]->WriteToConfig();
    }
}
