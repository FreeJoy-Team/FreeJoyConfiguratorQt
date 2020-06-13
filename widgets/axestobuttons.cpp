#include "axestobuttons.h"
#include "ui_axestobuttons.h"

AxesToButtons::AxesToButtons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesToButtons)
{
    ui->setupUi(this);
}

AxesToButtons::~AxesToButtons()
{
    delete ui;
}
