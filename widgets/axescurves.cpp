#include "axescurves.h"
#include "ui_axescurves.h"

AxesCurves::AxesCurves(int axes_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurves)
{
    ui->setupUi(this);
    axes_number_ = axes_number;
}

AxesCurves::~AxesCurves()
{
    delete ui;
}

void AxesCurves::ReadFromConfig()
{
    for (int i = 0; i < ui->widget_AxesCurvesPlot->GetPointCount(); ++i)
    {
        ui->widget_AxesCurvesPlot->SetPointValue(gEnv.pDeviceConfig->config.axis_config[axes_number_].curve_shape[i], i);
    }
}

void AxesCurves::WriteToConfig()
{
    for (int i = 0; i < ui->widget_AxesCurvesPlot->GetPointCount(); ++i)
    {
        gEnv.pDeviceConfig->config.axis_config[axes_number_].curve_shape[i] = ui->widget_AxesCurvesPlot->GetPointValue(i);
    }
}

