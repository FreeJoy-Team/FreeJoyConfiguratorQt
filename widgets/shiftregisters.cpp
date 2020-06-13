#include "shiftregisters.h"
#include "ui_shiftregisters.h"

ShiftRegisters::ShiftRegisters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShiftRegisters)
{
    ui->setupUi(this);
}

ShiftRegisters::~ShiftRegisters()
{
    delete ui;
}
