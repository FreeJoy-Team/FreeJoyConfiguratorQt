#include "axescurvesconfig.h"
#include "ui_axescurvesconfig.h"

//#include "widgets/axes.h"

AxesCurvesConfig::AxesCurvesConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurvesConfig)
{
    ui->setupUi(this);

    ui->layoutV_AxesCurves->setAlignment(Qt::AlignTop);
    // axes curves spawn
    for (int i = 0; i < MAX_AXIS_NUM; i++)
    {
        AxesCurves * axis_curves = new AxesCurves(i, this);
        ui->layoutV_AxesCurves->addWidget(axis_curves);
        AxesCurvAdrList.append(axis_curves);
        //shift_register->hide();

        connect(AxesCurvAdrList[i], SIGNAL(axisCurveIndexChanged(int, int)),
                this, SLOT(axisCurveIndexChanged(int, int)));

        connect(AxesCurvAdrList[i], SIGNAL(curvePointValueChanged(int, int, int)),
                this, SLOT(setCurvesValue(int, int, int)));

        connect(AxesCurvAdrList[i], SIGNAL(resetCurvesProfiles()),
                this, SLOT(resetCurvesProfiles()));
    }

    // set default linear curves_points_value_
    resetCurvesProfiles();
}

AxesCurvesConfig::~AxesCurvesConfig()
{
    delete ui;
}

void AxesCurvesConfig::SetDarkInterface(bool is_dark)
{
    for (int i = 0; i < AxesCurvAdrList.size(); ++i) {
        AxesCurvAdrList[i]->SetDarkIcon(is_dark);
    }
}

void AxesCurvesConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < AxesCurvAdrList.size(); ++i) {
        AxesCurvAdrList[i]->RetranslateUi();
    }
}

void AxesCurvesConfig::axisCurveIndexChanged(int axis_number, int index)
{
    for (int i = 0; i < CURVE_PLOT_POINTS_COUNT; ++i) {
        AxesCurvAdrList[axis_number]->SetPointValue(i, curves_points_value_ [index - 1] [i]);
    }
//    for (int i = 0; i < AxesCurvAdrList.size(); i++)    // может сразу записывать какой индекс у кривых, чуть быстрее или экономия на спичках?
//    {
//        if (AxesCurvAdrList[axis_number]->GetCurrentCurveIndex() == AxesCurvAdrList[i]->GetCurrentCurveIndex())
//        {
//            for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
//                AxesCurvAdrList[axis_number]->SetPointValue(j, AxesCurvAdrList[i]->GetPointValue(j));
//            }
//            break;
//        }
//    }
}

void AxesCurvesConfig::setCurvesValue(int axis_number, int point_number, int value)    // дать указатель на AxesCurvAdrList кривым и не придутся туда-сюда гонять?
{                                                                               // для одного потока норм и быстрее, но хз
    for (int i = 0; i < AxesCurvAdrList.size(); i++)    // может сразу записывать какой индекс у кривых, чуть быстрее или экономия на спичках?
    {
        if (AxesCurvAdrList[axis_number]->GetCurrentCurveIndex() == AxesCurvAdrList[i]->GetCurrentCurveIndex())
        {
            curves_points_value_ [AxesCurvAdrList[i]->GetCurrentCurveIndex() - 1] [point_number] = value;
            AxesCurvAdrList[i]->SetPointValue(point_number, value);
        }
    }
}

void AxesCurvesConfig::resetCurvesProfiles()
{
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {            // при изменении дефайнов добавить округление
        for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
            curves_points_value_[i][j] = CURVES_MIN_VALUE + ((abs(CURVES_MIN_VALUE) + abs(CURVES_MAX_VALUE)) / (CURVE_PLOT_POINTS_COUNT-1)) * j;
        }
    }

    for (int i = 0; i < AxesCurvAdrList.size(); i++)    // может сразу записывать какой индекс у кривых, чуть быстрее или экономия на спичках?
    {
        if (AxesCurvAdrList[i]->GetCurrentCurveIndex() > 0)
        {
            for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
                AxesCurvAdrList[i]->SetPointValue(j, curves_points_value_ [0] [j]); // AxesCurvAdrList[i]->GetCurrentCurveIndex() вместо 0 ?
            }
        }
    }
}


void AxesCurvesConfig::UpdateAxesCurves()
{
    for (int i = 0; i < AxesCurvAdrList.size(); ++i) {
        AxesCurvAdrList[i]->UpdateAxis();
    }
}

void AxesCurvesConfig::DeviceStatus(bool is_connect)
{
    for (int i = 0; i < AxesCurvAdrList.size(); ++i)
    {
        if (gEnv.pDeviceConfig->config.axis_config[i].source_main != -1 && gEnv.pDeviceConfig->config.axis_config[i].out_enabled == 1){   // -1 = None
            AxesCurvAdrList[i]->DeviceStatus(is_connect);
        } else {
            AxesCurvAdrList[i]->DeviceStatus(false);
        }
        //AxesCurvAdrList[i]->DeviceStatus(is_connect);
    }
}

void AxesCurvesConfig::ReadFromConfig()
{
    for (int i = 0; i < AxesCurvAdrList.size(); ++i) {
        AxesCurvAdrList[i]->ReadFromConfig();
    }
}

void AxesCurvesConfig::WriteToConfig()
{
    for (int i = 0; i < AxesCurvAdrList.size(); ++i) {
        AxesCurvAdrList[i]->WriteToConfig();
    }
}
