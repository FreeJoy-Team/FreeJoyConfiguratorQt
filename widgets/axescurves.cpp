#include "axescurves.h"
#include "ui_axescurves.h"

AxesCurves::AxesCurves(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurves)
{
    ui->setupUi(this);
}

AxesCurves::~AxesCurves()
{
    delete ui;
}
