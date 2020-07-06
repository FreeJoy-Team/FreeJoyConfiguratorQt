#include "axestobuttons.h"
#include "ui_axestobuttons.h"

AxesToButtons::AxesToButtons(int a2b_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesToButtons)
{
    ui->setupUi(this);
    a2b_number_ = a2b_number;
    ui->checkBox_A2bIndex->setText(axes_list_[a2b_number_].gui_name);           // temp

    ui->spinBox_A2bCount->setMaximum(MAX_A2B_BUTTONS);
    ui->widget_A2bSlider->SetPointsCount(ui->spinBox_A2bCount->value() + 1);

    connect(ui->spinBox_A2bCount, SIGNAL(valueChanged(int)),
            this, SLOT(a2bCountChanged(int)));
    // a2b checked
    connect(ui->checkBox_A2bIndex, SIGNAL(toggled(bool)),
            this, SLOT(a2bCheckBoxValueChanged(bool)));
}

AxesToButtons::~AxesToButtons()
{
    delete ui;
}

#include <QDebug>
void AxesToButtons::a2bCountChanged(int count)
{
        ui->widget_A2bSlider->SetPointsCount(count + 1);
}

void AxesToButtons::a2bCheckBoxValueChanged(bool value)
{
    if (value == true){
        ui->widget_A2bSlider->setEnabled(true);
        ui->spinBox_A2bCount->setEnabled(true);
    } else {
        ui->widget_A2bSlider->setEnabled(false);
        ui->spinBox_A2bCount->setEnabled(false);
    }
}

void AxesToButtons::ReadFromConfig()
{
    ui->checkBox_A2bIndex->setChecked(gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].is_enabled);
    ui->spinBox_A2bCount->setValue(gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].buttons_cnt);
    //ui->widget_A2bSlider->SetPointsCount(gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].buttons_cnt + 1);

    for (int i = 0; i < gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].buttons_cnt + 1; ++i) {
        ui->widget_A2bSlider->SetPointValue(gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].points[i], i);
    }
}

void AxesToButtons::WriteToConfig()
{
    gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].is_enabled = ui->checkBox_A2bIndex->isChecked();
    gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].buttons_cnt = ui->spinBox_A2bCount->value();

    for (int i = 0; i < ui->spinBox_A2bCount->value() + 1; ++i) {
        gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].points[i] = ui->widget_A2bSlider->GetPointValue(i);
    }
}
