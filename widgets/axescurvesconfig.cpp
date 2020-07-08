#include "axescurvesconfig.h"
#include "ui_axescurvesconfig.h"

AxesCurvesConfig::AxesCurvesConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurvesConfig)
{
    ui->setupUi(this);

    ui->layoutV_AxesCurves->setAlignment(Qt::AlignTop);
    // shift registers spawn
    for (int i = 0; i < MAX_AXIS_NUM; i++)
    {
        AxesCurves * axis_curves = new AxesCurves(this);
        ui->layoutV_AxesCurves->addWidget(axis_curves);
        AxesCurvAdrList.append(axis_curves);
        //shift_register->hide();
    }
}

AxesCurvesConfig::~AxesCurvesConfig()
{
    delete ui;
}
