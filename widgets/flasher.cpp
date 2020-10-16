#include "flasher.h"
#include "ui_flasher.h"

Flasher::Flasher(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Flasher)
{
    ui->setupUi(this);
}

Flasher::~Flasher()
{
    delete ui;
}


// находится в advancedsettings, но потом переедет сюда
