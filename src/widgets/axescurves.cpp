#include "axescurves.h"
#include "ui_axescurves.h"
#include <QTimer>
#include <cmath>

AxesCurves::AxesCurves(int axisNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AxesCurves)
{
    ui->setupUi(this);
    m_axisNumber = axisNumber;
    m_currentProfile = 0;
    ui->groupBox_Curve->setTitle(m_axesList[m_axisNumber].guiName);

    connect(ui->widget_AxesCurvesPlot,
            SIGNAL(pointValueChanged(const int *, const int *)),
            this,
            SLOT(pointValueChanged(const int *, const int *)));
}

AxesCurves::~AxesCurves()
{
    delete ui;
}
void AxesCurves::on_groupBox_Curve_clicked(bool checked)
{
    if (checked == false) {
        for (auto &&widget : this->findChildren<QWidget *>()) {
            if (widget != ui->groupBox_Curve) {
                widget->setVisible(false);
            }
        }
        QTimer::singleShot(10, [&] {
            //            ui->frame->setMinimumHeight(0);
            //            ui->groupBox_Curve->setMinimumHeight(0);
            this->setMinimumHeight(0);
            //            ui->frame->setMaximumHeight(0);
            //            ui->groupBox_Curve->setMaximumHeight(0);
        });
    } else {
        for (auto &&widget : this->findChildren<QWidget *>()) {
            widget->setVisible(true);
        }
        QTimer::singleShot(10, [&] {
            //            ui->frame->setMinimumHeight(0);
            //            ui->groupBox_Curve->setMinimumHeight(0);
            this->setMinimumHeight(600);
            //            ui->frame->setMaximumHeight(0);
            //            ui->groupBox_Curve->setMaximumHeight(0);
        });
    }
}

void AxesCurves::setDarkIcon(bool isDark)
{
    if (isDark == true) {
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

void AxesCurves::retranslateUi()
{
    ui->retranslateUi(this);
}

void AxesCurves::pointValueChanged(const int *pointNumber, const int *value)
{
    if (m_currentProfile > 0) {
        emit curvePointValueChanged(m_axisNumber, *pointNumber, *value);
    } else if (m_currentProfile == 0) {
        m_curvePointsValue[*pointNumber] = *value;
    }
}

void AxesCurves::setCurveProfile(int profile)
{
    m_currentProfile = profile;
    if (profile <= 0) {
        for (int i = 0; i < ui->widget_AxesCurvesPlot->pointCount(); ++i) {
            ui->widget_AxesCurvesPlot->setPointValue(i, m_curvePointsValue[i]);
        }
    }
}

int AxesCurves::getPointValue(int pointNumber) const
{
    return ui->widget_AxesCurvesPlot->pointValue(pointNumber);
}

void AxesCurves::setPointValue(int point_number, int value)
{
    ui->widget_AxesCurvesPlot->setPointValue(point_number, value);
}

void AxesCurves::updateAxis()
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

    params_report_t *paramsRep = &gEnv.pDeviceConfig->paramsReport;
    int min = gEnv.pDeviceConfig->config.axis_config[m_axisNumber].calib_min;
    int max = gEnv.pDeviceConfig->config.axis_config[m_axisNumber].calib_max;
    int value_x;
    int value_y;

    if (paramsRep->axis_data[m_axisNumber] < AXIS_CENTER_VALUE) {
        value_y = round(paramsRep->axis_data[m_axisNumber] / (float) AXIS_MIN_VALUE * CURVES_MIN_VALUE);
    } else {
        value_y = round(paramsRep->axis_data[m_axisNumber] / (float) AXIS_MAX_VALUE * CURVES_MAX_VALUE);
    }

    value_x = abs(round((paramsRep->raw_axis_data[m_axisNumber] - min) / (float) (max - min) * 200));

    ui->widget_AxesCurvesPlot->setCurAxisPos(value_x, value_y);
    //    }
}

void AxesCurves::on_pushButton_Linear_clicked()
{
    ui->widget_AxesCurvesPlot->setLinear();
}

//void AxesCurves::on_pushButton_LinearInvert_clicked()
//{
//    ui->widget_AxesCurvesPlot->SetLinearInvert();
//}

void AxesCurves::on_pushButton_Exponent_clicked()
{
    ui->widget_AxesCurvesPlot->setExponent();
}

void AxesCurves::on_pushButton_ExponentInvert_clicked()
{
    ui->widget_AxesCurvesPlot->setExponentInvert();
}

void AxesCurves::on_pushButton_Shape_clicked()
{
    ui->widget_AxesCurvesPlot->setShape();
}

void AxesCurves::deviceStatus(bool isConnect)
{
    ui->widget_AxesCurvesPlot->deviceStatus(isConnect);
}

void AxesCurves::readFromConfig()
{
    for (int i = 0; i < ui->widget_AxesCurvesPlot->pointCount(); ++i) {
        ui->widget_AxesCurvesPlot->setPointValue(i, gEnv.pDeviceConfig->config.axis_config[m_axisNumber].curve_shape[i]);
        m_curvePointsValue[i] = gEnv.pDeviceConfig->config.axis_config[m_axisNumber].curve_shape[i];
    }
}

void AxesCurves::writeToConfig()
{
    for (int i = 0; i < ui->widget_AxesCurvesPlot->pointCount(); ++i) {
        gEnv.pDeviceConfig->config.axis_config[m_axisNumber].curve_shape[i] = ui->widget_AxesCurvesPlot->pointValue(i);
    }
}
