#include "buttonphysical.h"
#include "ui_buttonphysical.h"

ButtonPhysical::ButtonPhysical(int button_number, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ButtonPhysical)
{
    ui->setupUi(this);
    current_state_ = false;
    ui->label_PhysicalButton->setNum(button_number + 1);
}

ButtonPhysical::~ButtonPhysical()
{
    delete ui;
}

void ButtonPhysical::SetButtonState(bool set_state)
{   
    if (set_state != current_state_)
    {
        if (set_state){ // QStringLiteral // QLatin1String
            ui->label_PhysicalButton->setStyleSheet("QLabel {  border-radius: 14px; min-height: 28px; min-width: 28px; "
                                                    "background-color: rgb(0, 128, 0); color: rgb(230, 230, 230);}");
        } else {
            ui->label_PhysicalButton->setStyleSheet("QLabel {  border-radius: 14px; min-height: 28px; min-width: 28px; "
                                                    "background-color: rgb(170, 0, 0); color: rgb(230, 230, 230);}");
        }
        current_state_ = set_state;
    }
}
