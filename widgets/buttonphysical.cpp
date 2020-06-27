#include "buttonphysical.h"
#include "ui_buttonphysical.h"

ButtonPhysical::ButtonPhysical(int button_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ButtonPhysical)
{
    ui->setupUi(this);
    button_number_ = button_number;
    ui->label_PhysicalButton->setNum(button_number_ + 1);
}

ButtonPhysical::~ButtonPhysical()
{
    delete ui;
}

void ButtonPhysical::ButtonState(bool is_activated)
{
    is_activated_ = is_activated;
    if (is_activated_){
        ui->label_PhysicalButton->setStyleSheet("QLabel {  border-radius: 11px; min-height: 22px; min-width: 22px; "
                                                "background-color: rgb(0, 128, 0); color: rgb(255, 255, 255);}");
    } else {
        ui->label_PhysicalButton->setStyleSheet("QLabel {  border-radius: 11px; min-height: 22px; min-width: 22px; "
                                                "background-color: rgb(190, 0, 0); color: rgb(255, 255, 255);}");
    }
}
