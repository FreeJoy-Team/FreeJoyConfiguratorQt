#include "led.h"
#include "ui_led.h"

LED::LED(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LED)
{
    ui->setupUi(this);
}

LED::~LED()
{
    delete ui;
}
