#include "axestobuttons.h"
#include "ui_axestobuttons.h"

AxesToButtons::AxesToButtons(int a2b_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesToButtons)
{
    ui->setupUi(this);
    a2b_number_ = a2b_number;
    ui->checkBox_A2bIndex->setText(QString::number(a2b_number_ + 1));           // temp

    ui->spinBox_A2bCount->setMaximum(MAX_A2B_BUTTONS);
    ui->widget_A2bSlider->SetPointsCount(ui->spinBox_A2bCount->value() + 1);

    connect(ui->spinBox_A2bCount, SIGNAL(valueChanged(int)),
            this, SLOT(a2bCountChanged(int)));
}

AxesToButtons::~AxesToButtons()
{
    delete ui;
}


void AxesToButtons::a2bCountChanged(int count)
{
        ui->widget_A2bSlider->SetPointsCount(count + 1);
}

void AxesToButtons::ReadFromConfig()
{
    ui->widget_A2bSlider->SetPointsCount(gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].buttons_cnt + 1);
    for (int i = 0; i < gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].buttons_cnt + 1; ++i) {
        ui->widget_A2bSlider->SetPointValue(gEnv.pDeviceConfig->config.axes_to_buttons[a2b_number_].points[i], i);
    }
}

void AxesToButtons::WriteToConfig()
{

}
