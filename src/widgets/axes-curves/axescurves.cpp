#include "axescurves.h"
#include "ui_axescurves.h"
#include <QTimer>
#include <QKeyEvent>
#include <cmath>
#include "axes.h"

#include <QDebug>

AxesCurves::AxesCurves(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AxesCurves)
    , m_ctrlActivated(false)
    , m_lastSelectedIndex(-1)
{
    ui->setupUi(this);
    Q_ASSERT(sizeof(gEnv.pDeviceConfig->config.axis_config[0].curve_shape) == CURVE_PLOT_POINTS_COUNT);

    connect(ui->widget_AxesCurvesPlot, SIGNAL(pointValueChanged(int, int)),
            this, SLOT(pointValueChanged(int, int)));


    // should be X-Y-Z-Rx-Ry-Rz-Slider1-Slider2 !!!
    m_curvesList << ui->widget_CurveX << ui->widget_CurveY << ui->widget_CurveZ <<
                    ui->widget_CurveRx << ui->widget_CurveRy << ui->widget_CurveRz <<
                    ui->widget_CurveSlider1 << ui->widget_CurveSlider2;

    for (int i = 0; i < m_curvesList.size(); ++i)
    {
        m_curvesList[i]->setProperty("index", i);
        m_curvesList[i]->setAutoExclusive(true);
        m_curvesList[i]->setCheckable(true);

        connect(m_curvesList[i], &AxesCurvesButton::toggled, this, [this](bool checked){
            if (checked) {
                int index = sender()->property("index").toInt();
                m_lastSelectedIndex = index;
                ui->widget_AxesCurvesPlot->setPointValues(m_curvesList[index]->pointValues());
                //ui->label_CurrentAxis->setText(axesList()[index].guiName);
            }
        });
    }

    ui->widget_CurveX->setChecked(true);
}

AxesCurves::~AxesCurves()
{
    delete ui;
}


void AxesCurves::retranslateUi()
{
    ui->retranslateUi(this);
}

void AxesCurves::pointValueChanged(int pointNumber, int value)
{
    Q_UNUSED(pointNumber)
    Q_UNUSED(value)
    for (int i = 0; i < m_curvesList.size(); ++i) {
        if (m_curvesList[i]->isChecked()) {
            for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {     // can be optimized
                m_curvesList[i]->setPointValue(j, ui->widget_AxesCurvesPlot->pointValue(j));
            }
        }
    }
}


int AxesCurves::pointValue(int pointNumber) const
{
    return ui->widget_AxesCurvesPlot->pointValue(pointNumber);
}

QVector<int> AxesCurves::pointValues() const
{
    return ui->widget_AxesCurvesPlot->pointValues();
}

void AxesCurves::setPointValue(int pointNumber, int value)
{
    ui->widget_AxesCurvesPlot->setPointValue(pointNumber, value);
}

void AxesCurves::setPointValues(const QVector<int> &values)
{
    ui->widget_AxesCurvesPlot->setPointValues(values);
}

void AxesCurves::setExclusive(bool exclusive)
{
    m_ctrlActivated = !exclusive;
    for (int i = 0; i < m_curvesList.size(); ++i) {
        m_curvesList[i]->setAutoExclusive(exclusive);
    }
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

    if (m_lastSelectedIndex > -1) {
        params_report_t *paramsRep = &gEnv.pDeviceConfig->paramsReport;
        int min = gEnv.pDeviceConfig->config.axis_config[m_lastSelectedIndex].calib_min;
        int max = gEnv.pDeviceConfig->config.axis_config[m_lastSelectedIndex].calib_max;
        int value_x;
        int value_y;

        if (paramsRep->axis_data[m_lastSelectedIndex] < AXIS_CENTER_VALUE) {
            value_y = round(paramsRep->axis_data[m_lastSelectedIndex] / (float) AXIS_MIN_VALUE * CURVES_MIN_VALUE);
        } else {
            value_y = round(paramsRep->axis_data[m_lastSelectedIndex] / (float) AXIS_MAX_VALUE * CURVES_MAX_VALUE);
        }

        value_x = abs(round((paramsRep->raw_axis_data[m_lastSelectedIndex] - min) / (float)(max - min) * 200.0f));

        ui->widget_AxesCurvesPlot->setCurAxisPos(value_x, value_y);
    }
    //    }
}

void AxesCurves::deviceStatus(bool isConnect)
{
    if (gEnv.pDeviceConfig->config.axis_config[m_lastSelectedIndex].source_main != -1 &&
            gEnv.pDeviceConfig->config.axis_config[m_lastSelectedIndex].out_enabled == 1)
    {
        ui->widget_AxesCurvesPlot->deviceStatus(isConnect);
    } else {
        ui->widget_AxesCurvesPlot->deviceStatus(false);
    }
}


void AxesCurves::readFromConfig()
{
    dev_config_t *cfg = &gEnv.pDeviceConfig->config;

    for (int i = 0; i < m_curvesList.size(); ++i)
    {
        AxesCurvesButton *curve = m_curvesList[i];

        if (i == 0) {
            curve->setChecked(true);
        } else {
            curve->setChecked(false);
        }

        for (int j = 0; j < curve->pointCount(); ++j) {
            curve->setPointValue(j, cfg->axis_config[i].curve_shape[j]);
            if (curve->isChecked()) {
                ui->widget_AxesCurvesPlot->setPointValue(j, cfg->axis_config[i].curve_shape[j]);
            }
        }
    }
}

void AxesCurves::writeToConfig()
{
    dev_config_t *cfg = &gEnv.pDeviceConfig->config;
    for (int i = 0; i < m_curvesList.size(); ++i) {
        for (int j = 0; j < m_curvesList[i]->pointCount(); ++j) {
            cfg->axis_config[i].curve_shape[j] = m_curvesList[i]->pointValue(j);
        }
    }
}
