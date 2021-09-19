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
        m_axesCurvPtrList.append(axis_curves);

        connect(m_axesCurvPtrList[i], SIGNAL(curvePointValueChanged(int, int, int)),
                this, SLOT(setCurvesValue(int, int, int)));

    }
    // profiles
    // тупо сделал, надо было делать отдельный виджет для профилей и спавнить в коде, а не в дизайнере.
    // переделаю, если функционал профилей потребуется изменить
    m_profilesCBoxPtrList = ui->groupBox_Profiles->findChildren<QComboBox *> ();
    for (int i = 0; i < m_profilesCBoxPtrList.size(); ++i) {
        m_profilesCBoxPtrList[i]->addItems(m_curvesList);

        connect(m_profilesCBoxPtrList[i], SIGNAL(currentIndexChanged(int)),
                this, SLOT(profileIndexChanged(int)));
    }

    // load curves profiles from app config file
    gEnv.pAppSettings->beginGroup("CurvesProfiles");
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        gEnv.pAppSettings->beginReadArray("Curve_" + QString::number(i + 1));
        for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
            gEnv.pAppSettings->setArrayIndex(j);
            m_curvesPointsValue[i][j] = gEnv.pAppSettings->value("Point_" + QString::number(j), -100 + 20*j).toInt();
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
            gEnv.pAppSettings->setValue("Point_" + QString::number(j), m_curvesPointsValue[i][j]);
        }
        gEnv.pAppSettings->endArray();
    }
    gEnv.pAppSettings->endGroup();
    delete ui;
}


void AxesCurvesConfig::setDarkInterface(bool isDark)
{
    for (int i = 0; i < m_axesCurvPtrList.size(); ++i) {
        m_axesCurvPtrList[i]->setDarkIcon(isDark);
    }
}

void AxesCurvesConfig::retranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < m_axesCurvPtrList.size(); ++i) {
        m_axesCurvPtrList[i]->retranslateUi();
    }
}

void AxesCurvesConfig::setCurvesValue(int axisNumber, int pointNumber, int value)    // дать указатель на AxesCurvAdrList кривым и не придётся туда-сюда гонять?
{                                                                               // для одного потока норм и быстрее, но хз
    for (int i = 0; i < m_axesCurvPtrList.size(); i++)
    {
        if (m_profilesCBoxPtrList[axisNumber]->currentIndex() == m_profilesCBoxPtrList[i]->currentIndex())
        {
            m_curvesPointsValue [m_profilesCBoxPtrList[i]->currentIndex() - 1] [pointNumber] = value;
            m_axesCurvPtrList[i]->setPointValue(pointNumber, value);
        }
    }
}

void AxesCurvesConfig::profileIndexChanged(int index)
{
    int axis_number = 0;
    while (axis_number < m_profilesCBoxPtrList.size())
    {
        if (sender() == m_profilesCBoxPtrList[axis_number]){
            break;
        }
        ++axis_number;
    }

    m_axesCurvPtrList[axis_number]->setCurveProfile(index);
    if (index > 0){
        for (int i = 0; i < CURVE_PLOT_POINTS_COUNT; ++i) {
            m_axesCurvPtrList[axis_number]->setPointValue(i, m_curvesPointsValue [index - 1] [i]);
        }
    }
}

void AxesCurvesConfig::on_pushButton_ResetProfiles_clicked()
{
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {            // при изменении дефайнов добавить округление
        for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
            m_curvesPointsValue[i][j] = CURVES_MIN_VALUE + ((abs(CURVES_MIN_VALUE) + abs(CURVES_MAX_VALUE)) / (CURVE_PLOT_POINTS_COUNT-1)) * j;
        }
    }

    for (int i = 0; i < m_profilesCBoxPtrList.size(); i++)
    {
        if (m_profilesCBoxPtrList[i]->currentIndex() > 0)
        {
            for (int j = 0; j < CURVE_PLOT_POINTS_COUNT; ++j) {
                m_axesCurvPtrList[i]->setPointValue(j, m_curvesPointsValue [0] [j]); // AxesCurvAdrList[i]->GetCurrentCurveIndex() вместо 0 ?
            }
        }
    }
    for (int i = 0; i < m_profilesCBoxPtrList.size(); ++i) {
        m_profilesCBoxPtrList[i]->setCurrentIndex(0);
    }
}


void AxesCurvesConfig::updateAxesCurves()
{
    for (int i = 0; i < m_axesCurvPtrList.size(); ++i) {
        m_axesCurvPtrList[i]->updateAxis();
    }
}

void AxesCurvesConfig::deviceStatus(bool isConnect)
{
    for (int i = 0; i < m_axesCurvPtrList.size(); ++i)
    {
        if (gEnv.pDeviceConfig->config.axis_config[i].source_main != -1 && gEnv.pDeviceConfig->config.axis_config[i].out_enabled == 1){   // -1 = None
            m_axesCurvPtrList[i]->deviceStatus(isConnect);
        } else {
            m_axesCurvPtrList[i]->deviceStatus(false);
        }
    }
}

void AxesCurvesConfig::readFromConfig()
{
    for (int i = 0; i < m_profilesCBoxPtrList.size(); ++i) {
        m_profilesCBoxPtrList[i]->setCurrentIndex(0);
    }

    for (int i = 0; i < m_axesCurvPtrList.size(); ++i) {
        m_axesCurvPtrList[i]->readFromConfig();
    }
}

void AxesCurvesConfig::writeToConfig()
{
    for (int i = 0; i < m_axesCurvPtrList.size(); ++i) {
        m_axesCurvPtrList[i]->writeToConfig();
    }
}
