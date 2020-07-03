#include "axestobuttons.h"
#include "ui_axestobuttons.h"

AxesToButtons::AxesToButtons(int a2b_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesToButtons)
{
    ui->setupUi(this);
    a2b_number_ = a2b_number;
}

AxesToButtons::~AxesToButtons()
{
    delete ui;
}


