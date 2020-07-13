#include "axescurves.h"
#include "ui_axescurves.h"

AxesCurves::AxesCurves(int axes_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurves)
{
    ui->setupUi(this);
    axes_number_ = axes_number;
    ui->groupBox->setTitle(axes_list_[axes_number_].gui_name);
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


void AxesCurves::on_pushButton_Linear_clicked()
{
    ui->widget_AxesCurvesPlot->SetLinear();
}

void AxesCurves::on_pushButton_LinearInvert_clicked()
{
    ui->widget_AxesCurvesPlot->SetLinearInvert();
}

void AxesCurves::on_pushButton_Exponent_clicked()
{
    ui->widget_AxesCurvesPlot->SetExponent();
}

void AxesCurves::on_pushButton_ExponentInvert_clicked()
{
    ui->widget_AxesCurvesPlot->SetExponentInvert();
}

void AxesCurves::on_pushButton_Shape_clicked()
{
    ui->widget_AxesCurvesPlot->SetShape();
}
