#include "buttonphysical.h"
#include "ui_buttonphysical.h"

ButtonPhysical::ButtonPhysical(int buttonNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ButtonPhysical)
{
    ui->setupUi(this);
    m_currentState = false;
    ui->label_PhysicalButton->setNum(buttonNumber + 1);
}

ButtonPhysical::~ButtonPhysical()
{
    delete ui;
}

void ButtonPhysical::setButtonState(bool setState)
{
    if (setState != m_currentState) {
        if (setState) { // QStringLiteral // QLatin1String
            ui->label_PhysicalButton->setStyleSheet("QLabel {  border-radius: 14px; min-height: 28px; min-width: 28px; "
                                                    "background-color: rgb(0, 128, 0); color: rgb(230, 230, 230);}");
        } else {
            ui->label_PhysicalButton->setStyleSheet("QLabel {  border-radius: 14px; min-height: 28px; min-width: 28px; "
                                                    "background-color: rgb(170, 0, 0); color: rgb(230, 230, 230);}");
        }
        m_currentState = setState;
    }
}
