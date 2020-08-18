#include "axes.h"
#include "ui_axes.h"
#include "widgets/axesextended.h"
#include <QTimer>

Axes::Axes(int axis_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Axes)
{
    ui->setupUi(this);

    a2b_buttons_count_ = 0;
    calibration_started_ = false;

    //pDev_config = &gEnv.pDeviceConfig->config;        // чуть короче запись, но надо ли?
    axis_number_ = axis_number;
    ui->groupBox_AxixName->setTitle(axes_list_[axis_number_].gui_name);     // для 64бит в будущем axis_number_ должен быть 64 бита для большого массива
                                                                            // т.к. 32 бит переменная переполнится и отсчёт пойдёт с 0 и .size() return 64 bit

    // add main source
    for (int i = 0; i < 2; ++i) {
        ui->comboBox_AxisSource1->addItem(axes_pin_list_[i].gui_name);
        main_source_enum_index_.push_back(axes_pin_list_[i].device_enum_index);
    }

    // set a2b
    ui->spinBox_A2bCount->setMaximum(MAX_A2B_BUTTONS);
    if (ui->spinBox_A2bCount->value() < kMinA2bButtons){
        ui->widget_A2bSlider->setEnabled(false);
        ui->widget_A2bSlider->SetPointsCount(kMinA2bButtons + 1);
        //count = kMinA2bButtons;
    } else {
        ui->widget_A2bSlider->setEnabled(true);
        ui->widget_A2bSlider->SetPointsCount(ui->spinBox_A2bCount->value() + 1);
    }

    // add axes extended settings
    axes_extend = new AxesExtended(axis_number_, this); //check
    axes_extend->setVisible(false);
    ui->layoutH_AxesExtend->addWidget(axes_extend);
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
    main_source_enum_index_.clear();
    main_source_enum_index_.shrink_to_fit();
    delete ui;
}

void Axes::RetranslateUi()
{
    ui->retranslateUi(this);
    axes_extend->RetranslateUi();
}

void Axes::AddOrDeleteMainSource(int source_enum, bool is_add)
{
    if (is_add == true){
        ui->comboBox_AxisSource1->addItem(axes_pin_list_[Converter::EnumToIndex(source_enum, axes_pin_list_)].gui_name);
        main_source_enum_index_.push_back(axes_pin_list_[Converter::EnumToIndex(source_enum, axes_pin_list_)].device_enum_index);
    } else {
        for (uint i = 0; i < main_source_enum_index_.size(); ++i) {
            if (main_source_enum_index_[i] == source_enum){
                if(ui->comboBox_AxisSource1->currentIndex() == (int)i){
                    ui->comboBox_AxisSource1->setCurrentIndex(0);
                }
                ui->comboBox_AxisSource1->removeItem(i);
                main_source_enum_index_.erase(main_source_enum_index_.begin() + i);
                break;
            }
        }
    }
}

void Axes::mainSourceIndexChanged(int index)
{
    if (main_source_enum_index_[index] == I2C){
        axes_extend->SetI2CEnabled(true);
    } else {
        axes_extend->SetI2CEnabled(false);
    }
}

void Axes::calibMinMaxValueChanged(int value)
{
    Q_UNUSED(value) 
    if(ui->checkBox_Center->isChecked() == false){
        ui->spinBox_CalibCenter->setValue((ui->spinBox_CalibMax->value() + ui->spinBox_CalibMin->value()) / 2);
    }
}

void Axes::calibrationStarted(int raw_value)
{
    if (ui->spinBox_CalibMax->value() < raw_value){
        ui->spinBox_CalibMax->setValue(raw_value);
    }
    else if (ui->spinBox_CalibMin->value() > raw_value){
        ui->spinBox_CalibMin->setValue(raw_value);
    }
}

void Axes::on_pushButton_StartCalib_clicked(bool checked)
{
    calibration_started_ = checked;
    if (checked == true)
    {
        ui->pushButton_StartCalib->setText(stop_calibration);
        ui->spinBox_CalibMax->setValue(AXIS_MIN_VALUE);     // не перепутано
        ui->spinBox_CalibMin->setValue(AXIS_MAX_VALUE);

        connect (ui->progressBar_Raw, SIGNAL(valueChanged(int)),
                 this, SLOT(calibrationStarted(int)));

    } else {
        disconnect(ui->progressBar_Raw, SIGNAL(valueChanged(int)), nullptr, nullptr);
        ui->pushButton_StartCalib->setText(start_calibration);
    }
}

void Axes::UpdateAxisRaw()
{
    ui->progressBar_Raw->setValue(gEnv.pDeviceConfig->gamepad_report.raw_axis_data[axis_number_]);
    ui->widget_A2bSlider->SetAxisRawValue(gEnv.pDeviceConfig->gamepad_report.raw_axis_data[axis_number_]);
}

void Axes::UpdateAxisOut()
{
    ui->progressBar_Out->setValue(gEnv.pDeviceConfig->gamepad_report.axis_data[axis_number_]);
}

void Axes::outputValueChanged(bool is_checked)
{
    if (is_checked == true){
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

    if (gEnv.pDeviceConfig->gamepad_report.raw_axis_data[axis_number_] > ui->spinBox_CalibMax->value())
    {
        ui->spinBox_CalibCenter->setValue(ui->spinBox_CalibMax->value());
    }
    else if (gEnv.pDeviceConfig->gamepad_report.raw_axis_data[axis_number_] < ui->spinBox_CalibMin->value())
    {
        ui->spinBox_CalibCenter->setValue(ui->spinBox_CalibMin->value());
    }
    else
    {
        ui->spinBox_CalibCenter->setValue(gEnv.pDeviceConfig->gamepad_report.raw_axis_data[axis_number_]);
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
    if (count < kMinA2bButtons){
        ui->widget_A2bSlider->setEnabled(false);
        //count = kMinA2bButtons;
    } else {
        ui->widget_A2bSlider->setEnabled(true);
        ui->widget_A2bSlider->SetPointsCount(count + 1);
    }


    if(ui->widget_A2bSlider->isEnabled() == true){
        emit a2bCountChanged(count, a2b_buttons_count_);
        a2b_buttons_count_ = count;
    } else {    // необязательно?
        emit a2bCountChanged(0, a2b_buttons_count_);
        a2b_buttons_count_ = 0;
    }
}

void Axes::on_checkBox_ShowExtend_stateChanged(int state)
{
    if (state == 2){    // 2 = true
        //ui->frame->setMinimumHeight(100);
        axes_extend->setMinimumHeight(100);
        QTimer::singleShot(10, [&]{     // я долбоёб или да? как ещё это сраное мигание победить????????
            axes_extend->setVisible(true);
        });
//        axes_extend->setVisible(true);
    } else {
        axes_extend->setVisible(false);
        QTimer::singleShot(10, [&]{
            axes_extend->setMinimumHeight(0);
            ui->frame->setMinimumHeight(0);
        });
//        axes_extend->setMinimumHeight(0);
//        ui->frame->setMinimumHeight(0);
    }
}


void Axes::ReadFromConfig()     // Converter::EnumToIndex(device_enum, list)
{
    // output, inverted
    ui->checkBox_Output->setChecked(gEnv.pDeviceConfig->config.axis_config[axis_number_].out_enabled);
    ui->checkBox_Inverted->setChecked(gEnv.pDeviceConfig->config.axis_config[axis_number_].inverted);
    //ui->comboBox_AxisSource1->setCurrentIndex(Converter::EnumToIndex(gEnv.pDeviceConfig->config.axis_config[axis_number_].source_main, axes_pin_list_));
    for (uint i = 0; i < main_source_enum_index_.size(); ++i) {                 // сделать Converter::    ?
        if (main_source_enum_index_[i] == gEnv.pDeviceConfig->config.axis_config[axis_number_].source_main){
            ui->comboBox_AxisSource1->setCurrentIndex(i);
            break;
        }
    }
    // calibration
    ui->spinBox_CalibMin->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_min);
    ui->spinBox_CalibCenter->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_center);
    ui->checkBox_Center->setChecked(gEnv.pDeviceConfig->config.axis_config[axis_number_].is_centered);
    ui->spinBox_CalibMax->setValue(gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_max);

    // axes to buttons
    ui->spinBox_A2bCount->setValue(gEnv.pDeviceConfig->config.axes_to_buttons[axis_number_].buttons_cnt);
    //ui->widget_A2bSlider->SetPointsCount(gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].buttons_cnt + 1);
    for (int i = 0; i < gEnv.pDeviceConfig->config.axes_to_buttons[axis_number_].buttons_cnt + 1; ++i) {
        ui->widget_A2bSlider->SetPointValue(gEnv.pDeviceConfig->config.axes_to_buttons[axis_number_].points[i], i);
    }

    // axes extended settings
    axes_extend->ReadFromConfig();
}

void Axes::WriteToConfig()
{   // output, inverted
    gEnv.pDeviceConfig->config.axis_config[axis_number_].out_enabled = ui->checkBox_Output->isChecked();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].inverted = ui->checkBox_Inverted->isChecked();
    // I2C, sources, function
    gEnv.pDeviceConfig->config.axis_config[axis_number_].source_main = main_source_enum_index_[ui->comboBox_AxisSource1->currentIndex()];
    // calibration
    gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_min = ui->spinBox_CalibMin->value();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_center = ui->spinBox_CalibCenter->value();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].is_centered = ui->checkBox_Center->isChecked();
    gEnv.pDeviceConfig->config.axis_config[axis_number_].calib_max = ui->spinBox_CalibMax->value();

    // axes to buttons
    if (ui->spinBox_A2bCount->value() >= kMinA2bButtons){
        gEnv.pDeviceConfig->config.axes_to_buttons[axis_number_].is_enabled = 1;
    } else {
        gEnv.pDeviceConfig->config.axes_to_buttons[axis_number_].is_enabled = 0;
    }
    gEnv.pDeviceConfig->config.axes_to_buttons[axis_number_].buttons_cnt = ui->spinBox_A2bCount->value();
    for (int i = 0; i < ui->spinBox_A2bCount->value() + 1; ++i) {
        gEnv.pDeviceConfig->config.axes_to_buttons[axis_number_].points[i] = ui->widget_A2bSlider->GetPointValue(i);
    }

    // axes extended settings
    axes_extend->WriteToConfig();
}
