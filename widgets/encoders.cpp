#include "encoders.h"
#include "ui_encoders.h"

Encoders::Encoders(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Encoders)
{
    ui->setupUi(this);
}

Encoders::~Encoders()
{
    delete ui;
}
