#include "axes.h"
#include "ui_axes.h"
#include <QTimer>

Axes::Axes(int axisNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Axes)
{
    ui->setupUi(this);

    m_a2bButtonsCount = 0;
    m_calibrationStarted = false;
    m_outputEnabled = ui->checkBox_Output->isChecked();

    m_axisNumber = axisNumber;
    ui->groupBox_AxixName->setTitle(
        m_axesList[m_axisNumber].guiName); // для 64бит в будущем axis_number_ должен быть 64 бита для большого массива
    // т.к. 32 бит переменная переполнится и отсчёт пойдёт с 0 и .size() return 64 bit

    // add main source
    for (int i = 0; i < 2; ++i) {
        ui->comboBox_AxisSource1->addItem(m_axesPinList[i].guiName);
        m_mainSourceEnumIndex.push_back(m_axesPinList[i].deviceEnumIndex);
    }

    // set a2b
    ui->spinBox_A2bCount->setMaximum(MAX_A2B_BUTTONS);
    if (ui->spinBox_A2bCount->value() < m_kMinA2bButtons) {
        ui->widget_A2bSlider->setEnabled(false);
        ui->widget_A2bSlider->setPointsCount(m_kMinA2bButtons + 1);
        //count = kMinA2bButtons;
    } else {
        ui->widget_A2bSlider->setEnabled(true);
        ui->widget_A2bSlider->setPointsCount(ui->spinBox_A2bCount->value() + 1);
    }

    // add axes extended settings
    m_axesExtend = new AxesExtended(m_axisNumber, this); //check
    m_axesExtend->setVisible(false);
    ui->layoutH_AxesExtend->addWidget(m_axesExtend);
    //ui->layoutV_Axes->addWidget(axes_extend);

    // output checked
    connect(ui->checkBox_Output, SIGNAL(toggled(bool)),
            this, SLOT(outputValueChanged(bool)));
    // calibration value changed
    connect(ui->spinBox_CalibMax, SIGNAL(valueChanged(int)),
            this, SLOT(calibMinMaxValueChanged(int)));
    connect(ui->spinBox_CalibMin, SIGNAL(valueChanged(int)),
            this, SLOT(calibMinMaxValueChanged(int)));
    // main source changed
    connect(ui->comboBox_AxisSource1, SIGNAL(currentIndexChanged(int)),
            this, SLOT(mainSourceIndexChanged(int)));
    // a2b count changed
    connect(ui->spinBox_A2bCount, SIGNAL(valueChanged(int)),
            this, SLOT(a2bSpinBoxChanged(int)));
}

Axes::~Axes()
{
    m_mainSourceEnumIndex.clear();
    m_mainSourceEnumIndex.shrink_to_fit();
    delete ui;
}

void Axes::retranslateUi()
{
    ui->retranslateUi(this);
    m_axesExtend->retranslateUi();
}

void Axes::addOrDeleteMainSource(int sourceEnum, bool isAdd)
{
    if (isAdd == true) {
        ui->comboBox_AxisSource1->addItem(m_axesPinList[Converter::EnumToIndex(sourceEnum, m_axesPinList)].guiName);
        m_mainSourceEnumIndex.push_back(m_axesPinList[Converter::EnumToIndex(sourceEnum, m_axesPinList)].deviceEnumIndex);
    } else {
        for (int i = 0; i < m_mainSourceEnumIndex.size(); ++i) {
            if (m_mainSourceEnumIndex[i] == sourceEnum) {
                if (ui->comboBox_AxisSource1->currentIndex() == (int) i) {
                    ui->comboBox_AxisSource1->setCurrentIndex(0);
                }
                ui->comboBox_AxisSource1->removeItem(i);
                m_mainSourceEnumIndex.erase(m_mainSourceEnumIndex.begin() + i);
                break;
            }
        }
    }
}

void Axes::mainSourceIndexChanged(int index)
{
    if (m_mainSourceEnumIndex[index] == I2C) {
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
    ui->progressBar_Raw->setValue(gEnv.pDeviceConfig->gamepadReport.raw_axis_data[m_axisNumber]);
}

void Axes::updateAxisOut()
{
    ui->progressBar_Out->setValue(gEnv.pDeviceConfig->gamepadReport.axis_data[m_axisNumber]);

    // a2b  axis_number_
    ui->widget_A2bSlider->setAxisOutputValue(gEnv.pDeviceConfig->gamepadReport.axis_data[m_axisNumber], m_outputEnabled);
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
    joy_report_t *joyRep = &gEnv.pDeviceConfig->gamepadReport;

    if (joyRep->raw_axis_data[m_axisNumber] > ui->spinBox_CalibMax->value()) {
        ui->spinBox_CalibCenter->setValue(ui->spinBox_CalibMax->value());
    } else if (joyRep->raw_axis_data[m_axisNumber] < ui->spinBox_CalibMin->value()) {
        ui->spinBox_CalibCenter->setValue(ui->spinBox_CalibMin->value());
    } else {
        ui->spinBox_CalibCenter->setValue(joyRep->raw_axis_data[m_axisNumber]);
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

void Axes::on_pushButton_clicked()
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
    if (state == 2) { // 2 = true
        //ui->frame->setMinimumHeight(110);
        m_axesExtend->setMinimumHeight(110);
        QTimer::singleShot(10, [&] { // я долбоёб или да? как ещё это сраное мигание победить????????!"!11112112121
            m_axesExtend->setVisible(true);
        });
        //        axes_extend->setVisible(true);
    } else {
        m_axesExtend->setVisible(false);
        QTimer::singleShot(10, [&] {
            m_axesExtend->setMinimumHeight(0);
            ui->frame->setMinimumHeight(0);
        });
        //        axes_extend->setMinimumHeight(0);
        //        ui->frame->setMinimumHeight(0);
    }
}

void Axes::readFromConfig() // Converter::EnumToIndex(device_enum, list)
{
    axis_config_t *axCfg = &gEnv.pDeviceConfig->config.axis_config[m_axisNumber];
    axis_to_buttons_t *a2bCfg = &gEnv.pDeviceConfig->config.axes_to_buttons[m_axisNumber];
    // output, inverted
    ui->checkBox_Output->setChecked(axCfg->out_enabled);
    ui->checkBox_Inverted->setChecked(axCfg->inverted);
    for (int i = 0; i < m_mainSourceEnumIndex.size(); ++i) { // сделать Converter::    ?
        if (m_mainSourceEnumIndex[i] == axCfg->source_main) {
            ui->comboBox_AxisSource1->setCurrentIndex(i);
            break;
        }
    }
    // calibration
    ui->spinBox_CalibMin->setValue(axCfg->calib_min);
    ui->spinBox_CalibCenter->setValue(axCfg->calib_center);
    ui->checkBox_Center->setChecked(axCfg->is_centered);
    ui->spinBox_CalibMax->setValue(axCfg->calib_max);
    // axes to buttons
    ui->spinBox_A2bCount->setValue(a2bCfg->buttons_cnt);
    for (int i = 0; i < a2bCfg->buttons_cnt + 1; ++i) {
        ui->widget_A2bSlider->setPointValue(a2bCfg->points[i], i);
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
    axCfg->source_main = m_mainSourceEnumIndex[ui->comboBox_AxisSource1->currentIndex()];
    // calibration
    axCfg->calib_min = ui->spinBox_CalibMin->value();
    axCfg->calib_center = ui->spinBox_CalibCenter->value();
    axCfg->is_centered = ui->checkBox_Center->isChecked();
    axCfg->calib_max = ui->spinBox_CalibMax->value();
    // axes to buttons
    if (ui->spinBox_A2bCount->value() >= m_kMinA2bButtons) {
        a2bCfg->is_enabled = 1;
    } else {
        a2bCfg->is_enabled = 0;
    }
    a2bCfg->buttons_cnt = ui->spinBox_A2bCount->value();
    for (int i = 0; i < ui->spinBox_A2bCount->value() + 1; ++i) {
        a2bCfg->points[i] = ui->widget_A2bSlider->pointValue(i);
    }
    // axes extended settings
    m_axesExtend->writeToConfig();
}
