#include "axescurvesconfig.h"
#include "ui_axescurvesconfig.h"
#include <QSettings>

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
        AxesCurvPtrList_.append(axis_curves);

        connect(AxesCurvPtrList_[i], SIGNAL(curvePointValueChanged(int, int, int)),
                this, SLOT(setCurvesValue(int, int, int)));

    }
    // profiles
    // тупо сделал, надо было делать отдельный виджет для профилей и спавнить в коде, а не в дизайнере.
    // переделаю, если функционал профилей потребуется изменить
    ProfilesCBoxPtrList_ = ui->groupBox_Profiles->findChildren<QComboBox *> ();
    for (int i = 0; i < ProfilesCBoxPtrList_.size(); ++i) {
        ProfilesCBoxPtrList_[i]->addItems(curves_list_);

        connect(ProfilesCBoxPtrList_[i], SIGNAL(currentIndexChanged(int)),
                this, SLOT(profileIndexChanged(int)));
    }

    // load curves profiles from app config file
    gEnv.pAppSettings->beginGroup("CurvesProfiles");
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        gEnv.pAppSettings->beginReadArray("Curve_" + QString::number(i + 1));
        for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
            gEnv.pAppSettings->setArrayIndex(j);
            curves_points_value_[i][j] = gEnv.pAppSettings->value("Point_" + QString::number(j), -100 + 20*j).toInt();
        }
        gEnv.pAppSettings->endArray();
    }
    gEnv.pAppSettings->endGroup();
}

AxesCurvesConfig::~AxesCurvesConfig()
{
    // save curves profiles to app config file
    gEnv.pAppSettings->beginGroup("CurvesProfiles");
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        gEnv.pAppSettings->beginWriteArray("Curve_" + QString::number(i + 1));
        for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
            gEnv.pAppSettings->setArrayIndex(j);
            gEnv.pAppSettings->setValue("Point_" + QString::number(j), curves_points_value_[i][j]);
        }
        gEnv.pAppSettings->endArray();
    }
    gEnv.pAppSettings->endGroup();
    delete ui;
}

void AxesCurvesConfig::SetDarkInterface(bool is_dark)
{
    for (int i = 0; i < AxesCurvPtrList_.size(); ++i) {
        AxesCurvPtrList_[i]->SetDarkIcon(is_dark);
    }
}

void AxesCurvesConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < AxesCurvPtrList_.size(); ++i) {
        AxesCurvPtrList_[i]->RetranslateUi();
    }
}

void AxesCurvesConfig::setCurvesValue(int axis_number, int point_number, int value)    // дать указатель на AxesCurvAdrList кривым и не придутся туда-сюда гонять?
{                                                                               // для одного потока норм и быстрее, но хз
    for (int i = 0; i < AxesCurvPtrList_.size(); i++)
    {
        if (ProfilesCBoxPtrList_[axis_number]->currentIndex() == ProfilesCBoxPtrList_[i]->currentIndex())
        {
            curves_points_value_ [ProfilesCBoxPtrList_[i]->currentIndex() - 1] [point_number] = value;
            AxesCurvPtrList_[i]->SetPointValue(point_number, value);
        }
    }
}

void AxesCurvesConfig::profileIndexChanged(int index)
{
    int axis_number = 0;
    while (axis_number < ProfilesCBoxPtrList_.size())
    {
        if (sender() == ProfilesCBoxPtrList_[axis_number]){
            break;
        }
        ++axis_number;
    }

    AxesCurvPtrList_[axis_number]->SetCurveProfile(index);
    if (index > 0){
        for (int i = 0; i < CURVE_PLOT_POINTS_COUNT; ++i) {
            AxesCurvPtrList_[axis_number]->SetPointValue(i, curves_points_value_ [index - 1] [i]);
        }
    }
}

void AxesCurvesConfig::on_pushButton_ResetProfiles_clicked()
{
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {            // при изменении дефайнов добавить округление
        for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
            curves_points_value_[i][j] = CURVES_MIN_VALUE + ((abs(CURVES_MIN_VALUE) + abs(CURVES_MAX_VALUE)) / (CURVE_PLOT_POINTS_COUNT-1)) * j;
        }
    }

    for (int i = 0; i < ProfilesCBoxPtrList_.size(); i++)
    {
        if (ProfilesCBoxPtrList_[i]->currentIndex() > 0)
        {
            for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
                AxesCurvPtrList_[i]->SetPointValue(j, curves_points_value_ [0] [j]); // AxesCurvAdrList[i]->GetCurrentCurveIndex() вместо 0 ?
            }
        }
    }
    for (int i = 0; i < ProfilesCBoxPtrList_.size(); ++i) {
        ProfilesCBoxPtrList_[i]->setCurrentIndex(0);
    }
}


void AxesCurvesConfig::UpdateAxesCurves()
{
    for (int i = 0; i < AxesCurvPtrList_.size(); ++i) {
        AxesCurvPtrList_[i]->UpdateAxis();
    }
}

void AxesCurvesConfig::DeviceStatus(bool is_connect)
{
    for (int i = 0; i < AxesCurvPtrList_.size(); ++i)
    {
        if (gEnv.pDeviceConfig->config.axis_config[i].source_main != -1 && gEnv.pDeviceConfig->config.axis_config[i].out_enabled == 1){   // -1 = None
            AxesCurvPtrList_[i]->DeviceStatus(is_connect);
        } else {
            AxesCurvPtrList_[i]->DeviceStatus(false);
        }
    }
}

void AxesCurvesConfig::ReadFromConfig()
{
    for (int i = 0; i < ProfilesCBoxPtrList_.size(); ++i) {
        ProfilesCBoxPtrList_[i]->setCurrentIndex(0);
    }

    for (int i = 0; i < AxesCurvPtrList_.size(); ++i) {
        AxesCurvPtrList_[i]->ReadFromConfig();
    }
}

void AxesCurvesConfig::WriteToConfig()
{
    for (int i = 0; i < AxesCurvPtrList_.size(); ++i) {
        AxesCurvPtrList_[i]->WriteToConfig();
    }
}
