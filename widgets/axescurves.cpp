#include "axescurves.h"
#include "ui_axescurves.h"
#include <cmath>


AxesCurves::AxesCurves(int axis_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurves)
{
    ui->setupUi(this);
    axis_number_ = axis_number;
    current_profile_ = 0;
    ui->groupBox_Curve->setTitle(axes_list_[axis_number_].gui_name);

    connect(ui->widget_AxesCurvesPlot, SIGNAL(pointValueChanged(const int*, const int*)),
            this, SLOT(pointValueChanged(const int*, const int*)));

}

AxesCurves::~AxesCurves()
{
    delete ui;
}
#include <QTimer>
void AxesCurves::on_groupBox_Curve_clicked(bool checked)
{
    if (checked == false){
        for (auto&& widget: this->findChildren<QWidget *>())
        {
            if (widget != ui->groupBox_Curve){
                widget->setVisible(false);
            }
        }
        QTimer::singleShot(10, [&]{
//            ui->frame->setMinimumHeight(0);
//            ui->groupBox_Curve->setMinimumHeight(0);
            this->setMinimumHeight(0);
//            ui->frame->setMaximumHeight(0);
//            ui->groupBox_Curve->setMaximumHeight(0);
        });
    } else {
        for (auto&& widget: this->findChildren<QWidget *>())
        {
            widget->setVisible(true);
        }
        QTimer::singleShot(10, [&]{
//            ui->frame->setMinimumHeight(0);
//            ui->groupBox_Curve->setMinimumHeight(0);
            this->setMinimumHeight(600);
//            ui->frame->setMaximumHeight(0);
//            ui->groupBox_Curve->setMaximumHeight(0);
        });
    }
}

void AxesCurves::SetDarkIcon(bool is_dark)
{
    if (is_dark == true){
        ui->pushButton_Linear->setIcon(QIcon(":/Images/linear_dark.png"));
       //ui->pushButton_LinearInvert->setIcon(QIcon(":/Images/linearInvert_dark.png"));
        ui->pushButton_Exponent->setIcon(QIcon(":/Images/Exponent_dark.png"));
        ui->pushButton_ExponentInvert->setIcon(QIcon(":/Images/ExponentInvert_dark.png"));
        ui->pushButton_Shape->setIcon(QIcon(":/Images/Shape_dark.png"));
    } else {
        ui->pushButton_Linear->setIcon(QIcon(":/Images/linear.png"));
        //ui->pushButton_LinearInvert->setIcon(QIcon(":/Images/linearInvert.png"));
        ui->pushButton_Exponent->setIcon(QIcon(":/Images/Exponent.png"));
        ui->pushButton_ExponentInvert->setIcon(QIcon(":/Images/ExponentInvert.png"));
        ui->pushButton_Shape->setIcon(QIcon(":/Images/Shape.png"));
    }
}

void AxesCurves::RetranslateUi()
{
    ui->retranslateUi(this);
}


void AxesCurves::pointValueChanged(const int *point_number, const int *value)
{
    if (current_profile_ > 0){
        emit curvePointValueChanged(axis_number_, *point_number, *value);
    }
    else if (current_profile_ == 0)
    {
        curve_points_value_[*point_number] = *value;
    }
}

void AxesCurves::SetCurveProfile(int profile)
{
    current_profile_ = profile;
    if (profile <= 0)
    {
        for (int i = 0; i < ui->widget_AxesCurvesPlot->GetPointCount(); ++i)
        {
            ui->widget_AxesCurvesPlot->SetPointValue(i, curve_points_value_[i]);
        }
    }
}


int AxesCurves::GetPointValue(int point_number) const
{
    return ui->widget_AxesCurvesPlot->GetPointValue(point_number);
}

void AxesCurves::SetPointValue(int point_number, int value)
{
    ui->widget_AxesCurvesPlot->SetPointValue(point_number, value);
}

void AxesCurves::UpdateAxis()
{
//    static int tmp_raw_data = 0;      // оптимизация или?
//    static int tmp_data = 0;
//    if ((tmp_raw_data != gEnv.pDeviceConfig->gamepad_report.raw_axis_data[axis_number_] || tmp_data != gEnv.pDeviceConfig->gamepad_report.axis_data[axis_number_]))
//    {
//        tmp_raw_data = gEnv.pDeviceConfig->gamepad_report.raw_axis_data[axis_number_];
//        tmp_data = gEnv.pDeviceConfig->gamepad_report.axis_data[axis_number_];
//    }

//    if (gEnv.pDeviceConfig->config.axis_config[axis_number_].out_enabled == 1)
//    {

    int min = gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_min;
    int max = gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_max;
    int value_x;
    int value_y;

    if (gEnv.pDeviceConfig->gamepad_report.axis_data[axis_number_] < AXIS_CENTER_VALUE){
        value_y = round(gEnv.pDeviceConfig->gamepad_report.axis_data[axis_number_] / (float)AXIS_MIN_VALUE * CURVES_MIN_VALUE);
    } else {
        value_y = round(gEnv.pDeviceConfig->gamepad_report.axis_data[axis_number_] / (float)AXIS_MAX_VALUE * CURVES_MAX_VALUE);
    }

    value_x = abs(round((gEnv.pDeviceConfig->gamepad_report.raw_axis_data[axis_number_]  - min)/ (float)(max - min) * 200));

    ui->widget_AxesCurvesPlot->UpdateAxis(value_x, value_y);
//    }
}


void AxesCurves::on_pushButton_Linear_clicked()
{
    ui->widget_AxesCurvesPlot->SetLinear();
}

//void AxesCurves::on_pushButton_LinearInvert_clicked()
//{
//    ui->widget_AxesCurvesPlot->SetLinearInvert();
//}

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

void AxesCurves::DeviceStatus(bool is_connect)
{
    ui->widget_AxesCurvesPlot->DeviceStatus(is_connect);
}


void AxesCurves::ReadFromConfig()
{
    for (int i = 0; i < ui->widget_AxesCurvesPlot->GetPointCount(); ++i)
    {
        ui->widget_AxesCurvesPlot->SetPointValue(i, gEnv.pDeviceConfig->config.axis_config[axis_number_].curve_shape[i]);
        curve_points_value_[i] = gEnv.pDeviceConfig->config.axis_config[axis_number_].curve_shape[i];
    }
}

void AxesCurves::WriteToConfig()
{
    for (int i = 0; i < ui->widget_AxesCurvesPlot->GetPointCount(); ++i)
    {
        gEnv.pDeviceConfig->config.axis_config[axis_number_].curve_shape[i] = ui->widget_AxesCurvesPlot->GetPointValue(i);
    }
}
