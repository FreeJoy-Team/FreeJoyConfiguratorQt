#include "axesconfig.h"
#include "ui_axesconfig.h"

AxesConfig::AxesConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesConfig)
{
    ui->setupUi(this);
}

AxesConfig::~AxesConfig()
{
    delete ui;
}
