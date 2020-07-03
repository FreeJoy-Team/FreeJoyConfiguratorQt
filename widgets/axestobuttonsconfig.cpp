#include "axestobuttonsconfig.h"
#include "ui_axestobuttonsconfig.h"

AxesToButtonsConfig::AxesToButtonsConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesToButtonsConfig)
{
    ui->setupUi(this);

    ui->layoutV_AxesToButtons->setAlignment(Qt::AlignTop);
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        AxesToButtons* a2b = new AxesToButtons(i, this);
        ui->layoutV_AxesToButtons->addWidget(a2b);
        A2bAdrList.append(a2b);
    }
}

AxesToButtonsConfig::~AxesToButtonsConfig()
{
    delete ui;
}
