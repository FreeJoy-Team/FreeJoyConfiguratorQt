#include "axes.h"
#include "ui_axes.h"
#include <QTimer>
#include <QTranslator>
#include "converter.h"

const QVector <deviceEnum_guiName_t> &axesList()    // порядок обязан быть как в common_types.h!!!!!!!!!!!
{
    static const QVector <deviceEnum_guiName_t> aL =
    {{
        {0,      ("X")},
        {1,      ("Y")},
        {2,      ("Z")},
        {3,      ("Rx")},
        {4,      ("Ry")},
        {5,      ("Rz")},
        {6,      QCoreApplication::translate("Axes", "Slider 1")},
        {7,      QCoreApplication::translate("Axes", "Slider 2")},
    }};

    return aL;
}

Axes::Axes(int axisNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Axes)
{
    ui->setupUi(this);

    m_a2bButtonsCount = 0;
    m_lastA2bCount = 1;
    m_calibrationStarted = false;
    m_outputEnabled = ui->checkBox_Output->isChecked();

    m_axisNumber = axisNumber;
    ui->groupBox_AxixName->setTitle(axesList()[m_axisNumber].guiName);

    // add main source
    for (int i = 0; i < 2; ++i) {
        ui->comboBox_AxisSource1->addItem(m_axesPinList[i].guiName);
        m_mainSource_enumIndex.push_back(m_axesPinList[i].deviceEnumIndex);
    }

    // set a2b  // двойная работа? readFromConfig()
    ui->spinBox_A2bCount->setMaximum(MAX_A2B_BUTTONS);
    if (ui->spinBox_A2bCount->value() < m_kMinA2bButtons) {
        ui->widget_A2bSlider->setEnabled(false);
        ui->widget_A2bSlider->setPointsCount(m_kMinA2bButtons);//+1
    } else {
        ui->widget_A2bSlider->setEnabled(true);
        ui->widget_A2bSlider->setPointsCount(ui->spinBox_A2bCount->value() + 1);
    }

    // add axes extended settings
    m_axesExtend = new AxesExtended(m_axisNumber, this);
    m_axesExtend->setVisible(false);
    ui->layoutH_AxesExtend->addWidget(m_axesExtend);
    //ui->layoutV_Axes->addWidget(axes_extend);

    // output checked
    connect(ui->checkBox_Output, &QCheckBox::toggled, this, &Axes::outputValueChanged);
    // calibration value changed
    connect(ui->spinBox_CalibMax, qOverload<int>(&QSpinBox::valueChanged), this, &Axes::calibMinMaxValueChanged);
    connect(ui->spinBox_CalibMin, qOverload<int>(&QSpinBox::valueChanged), this, &Axes::calibMinMaxValueChanged);
    // main source changed
    connect(ui->comboBox_AxisSource1, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &Axes::mainSourceIndexChanged);
    // a2b count changed
    connect(ui->spinBox_A2bCount, qOverload<int>(&QSpinBox::valueChanged), this, &Axes::a2bSpinBoxChanged);

    Q_ASSERT(ui->groupBox_AxixName->objectName() == QStringLiteral("groupBox_AxixName"));
}

Axes::~Axes()
{
    m_mainSource_enumIndex.clear();
    m_mainSource_enumIndex.shrink_to_fit();
    delete ui;
}

void Axes::retranslateUi()
{
    ui->retranslateUi(this);
    m_axesExtend->retranslateUi();
}

void Axes::addOrDeleteMainSource(int sourceEnum, QString sourceName, bool isAdd)
{
    if (isAdd == true) {
        ui->comboBox_AxisSource1->addItem(m_axesPinList[Converter::EnumToIndex(sourceEnum, m_axesPinList)].guiName + " - " + sourceName);
        m_mainSource_enumIndex.push_back(m_axesPinList[Converter::EnumToIndex(sourceEnum, m_axesPinList)].deviceEnumIndex);
    } else {
        for (int i = 0; i < m_mainSource_enumIndex.size(); ++i) {
            if (m_mainSource_enumIndex[i] == sourceEnum) {
                if (ui->comboBox_AxisSource1->currentIndex() == i) {
                    ui->comboBox_AxisSource1->setCurrentIndex(0);
                }
                ui->comboBox_AxisSource1->removeItem(i);
                m_mainSource_enumIndex.erase(m_mainSource_enumIndex.begin() + i);
                break;
            }
        }
    }
}

void Axes::mainSourceIndexChanged(int index)
{
    if (m_mainSource_enumIndex[index] == I2C) {
        m_axesExtend->setI2CEnabled(true);
    } else {
        m_axesExtend->setI2CEnabled(false);
    }
}

void Axes::calibMinMaxValueChanged(int value)
{
    Q_UNUSED(value)
    if (ui->checkBox_Center->isChecked() == false) {
        ui->spinBox_CalibCenter->setValue((ui->spinBox_CalibMax->value() + ui->spinBox_CalibMin->value()) / 2);
    }
}

void Axes::calibrationStarted(int rawValue)
{
    if (ui->spinBox_CalibMax->value() < rawValue) {
        ui->spinBox_CalibMax->setValue(rawValue);
    } else if (ui->spinBox_CalibMin->value() > rawValue) {
        ui->spinBox_CalibMin->setValue(rawValue);
    }
}

void Axes::on_pushButton_StartCalib_clicked(bool checked)
{
    m_calibrationStarted = checked;
    if (checked == true) {
        ui->pushButton_StartCalib->setText(m_kStopCalStr);
        ui->spinBox_CalibMax->setValue(AXIS_MIN_VALUE); // не перепутано
        ui->spinBox_CalibMin->setValue(AXIS_MAX_VALUE);

        connect(ui->progressBar_Raw, SIGNAL(valueChanged(int)), this, SLOT(calibrationStarted(int)));

    } else {
        disconnect(ui->progressBar_Raw, SIGNAL(valueChanged(int)), nullptr, nullptr);
        ui->pushButton_StartCalib->setText(m_kStartCalStr);
    }
}

void Axes::updateAxisRaw()
{
    ui->progressBar_Raw->setValue(gEnv.pDeviceConfig->paramsReport.raw_axis_data[m_axisNumber]);
}

void Axes::updateAxisOut()
{
    ui->progressBar_Out->setValue(gEnv.pDeviceConfig->paramsReport.axis_data[m_axisNumber]);

    // a2b  axis_number_
    ui->widget_A2bSlider->setAxisOutputValue(gEnv.pDeviceConfig->paramsReport.axis_data[m_axisNumber], m_outputEnabled);
}

void Axes::outputValueChanged(bool isChecked)
{
    m_outputEnabled = isChecked;
    updateAxisOut();
    if (isChecked == true) {
        ui->progressBar_Out->setEnabled(true);
        ui->progressBar_Raw->setEnabled(true);
    } else {
        ui->progressBar_Out->setEnabled(false);
        ui->progressBar_Raw->setEnabled(false);
    }
}

void Axes::on_pushButton_SetCenter_clicked()
{
    ui->checkBox_Center->setChecked(true);
    params_report_t *paramsRep = &gEnv.pDeviceConfig->paramsReport;

    if (paramsRep->raw_axis_data[m_axisNumber] > ui->spinBox_CalibMax->value()) {
        ui->spinBox_CalibCenter->setValue(ui->spinBox_CalibMax->value());
    } else if (paramsRep->raw_axis_data[m_axisNumber] < ui->spinBox_CalibMin->value()) {
        ui->spinBox_CalibCenter->setValue(ui->spinBox_CalibMin->value());
    } else {
        ui->spinBox_CalibCenter->setValue(paramsRep->raw_axis_data[m_axisNumber]);
    }
}

void Axes::on_checkBox_Center_stateChanged(int state)
{
    if (state == 2) // 2 = true
    {
        ui->spinBox_CalibCenter->setEnabled(true);
    } else {
        ui->spinBox_CalibCenter->setEnabled(false);
        calibMinMaxValueChanged(0);
    }
}

void Axes::on_pushButton_ResetCalib_clicked()
{
    ui->spinBox_CalibMax->setValue(AXIS_MAX_VALUE);
    ui->spinBox_CalibMin->setValue(AXIS_MIN_VALUE);
}

void Axes::a2bSpinBoxChanged(int count)
{
    if (count < m_kMinA2bButtons) {
        ui->widget_A2bSlider->setEnabled(false);
        ui->widget_A2bSlider->setPointsCount(0);
        //count = kMinA2bButtons;
    } else {
        ui->widget_A2bSlider->setEnabled(true);
        ui->widget_A2bSlider->setPointsCount(count + 1);
    }

    if (ui->widget_A2bSlider->isEnabled() == true) {
        emit a2bCountChanged(count, m_a2bButtonsCount);
        m_a2bButtonsCount = count;
    } else { // необязательно?
        emit a2bCountChanged(0, m_a2bButtonsCount);
        m_a2bButtonsCount = 0;
    }
}

void Axes::on_checkBox_ShowExtend_stateChanged(int state)
{
    // QTimer::singleShot(10 - antiblink
    if (state == 2) { // 2 = true
        m_axesExtend->setMinimumHeight(115);
        QTimer::singleShot(10, this, [this] {
            m_axesExtend->setVisible(true);
        });
    } else {
        m_axesExtend->setVisible(false);
        QTimer::singleShot(10, this, [this] {
            m_axesExtend->setMinimumHeight(0);
            ui->frame->setMinimumHeight(0);
        });
    }
}

void Axes::readFromConfig()
{
    axis_config_t *axCfg = &gEnv.pDeviceConfig->config.axis_config[m_axisNumber];
    axis_to_buttons_t *a2bCfg = &gEnv.pDeviceConfig->config.axes_to_buttons[m_axisNumber];
    // output, inverted
    ui->checkBox_Output->setChecked(axCfg->out_enabled);
    ui->checkBox_Inverted->setChecked(axCfg->inverted);
    int index = Converter::EnumToIndex(axCfg->source_main, m_mainSource_enumIndex);
    if (index == -1) index = 0;
    ui->comboBox_AxisSource1->setCurrentIndex(index);
    // calibration
    ui->spinBox_CalibMin->setValue(axCfg->calib_min);
    ui->spinBox_CalibCenter->setValue(axCfg->calib_center);
    ui->checkBox_Center->setChecked(axCfg->is_centered);
    ui->spinBox_CalibMax->setValue(axCfg->calib_max);
    // axes to buttons
    ui->spinBox_A2bCount->setValue(a2bCfg->buttons_cnt);
    if (a2bCfg->buttons_cnt > 0) {
        for (int i = 0; i < a2bCfg->buttons_cnt + 1; ++i) {
            ui->widget_A2bSlider->setPointValue(a2bCfg->points[i], i);
        }
    }
    // axes extended settings
    m_axesExtend->readFromConfig();
}

void Axes::writeToConfig()
{
    axis_config_t *axCfg = &gEnv.pDeviceConfig->config.axis_config[m_axisNumber];
    axis_to_buttons_t *a2bCfg = &gEnv.pDeviceConfig->config.axes_to_buttons[m_axisNumber];
    // output, inverted
    axCfg->out_enabled = ui->checkBox_Output->isChecked();
    axCfg->inverted = ui->checkBox_Inverted->isChecked();
    // I2C, sources, function
    axCfg->source_main = m_mainSource_enumIndex[ui->comboBox_AxisSource1->currentIndex()];
    // calibration
    axCfg->calib_min = ui->spinBox_CalibMin->value();
    axCfg->calib_center = ui->spinBox_CalibCenter->value();
    axCfg->is_centered = ui->checkBox_Center->isChecked();
    axCfg->calib_max = ui->spinBox_CalibMax->value();
    // axes to buttons
    a2bCfg->buttons_cnt = ui->spinBox_A2bCount->value();
    for (int i = 0; i < ui->spinBox_A2bCount->value() + 1; ++i) {
        a2bCfg->points[i] = ui->widget_A2bSlider->pointValue(i);
    }
    // axes extended settings
    m_axesExtend->writeToConfig();
}
