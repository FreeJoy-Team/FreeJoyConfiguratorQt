#include "ledconfig.h"
#include "ui_ledconfig.h"

LedConfig::LedConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LedConfig)
{
    ui->setupUi(this);
}

LedConfig::~LedConfig()
{
    delete ui;
}
