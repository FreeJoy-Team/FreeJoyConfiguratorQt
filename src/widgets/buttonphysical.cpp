#include "buttonphysical.h"
#include "ui_buttonphysical.h"

ButtonPhysical::ButtonPhysical(int buttonNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ButtonPhysical)
{
    ui->setupUi(this);
    m_currentState = false;
    m_buttonIndex = buttonNumber;
    ui->label_PhysicalButton->setNum(m_buttonIndex + 1);
    m_defStyle = QStringLiteral("QLabel {  border-radius: 13px; min-height: 26px; min-width: 26px; "
                                "background-color: rgb(170, 0, 0); color: rgb(210, 210, 210);}");
    ui->label_PhysicalButton->setStyleSheet(m_defStyle);
}

ButtonPhysical::~ButtonPhysical()
{
    delete ui;
}

void ButtonPhysical::setButtonState(bool state)
{
    //qDebug()<<"button ="<<m_buttonIndex<<state;
    if (state != m_currentState) {
        if (state) {
            ui->label_PhysicalButton->setStyleSheet(
                QStringLiteral("QLabel {  border-radius: 13px; min-height: 26px; min-width: 26px; "
                               "background-color: rgb(0, 128, 0); color: rgb(210, 210, 210);}"));
            emit physButtonPressed(m_buttonIndex);
            m_lastAct.start();
            m_currentState = state;
        } else {
            // sometimes state dont have time to render. e.g. encoder press time 10ms and monitor refresh time 17ms(60fps)
            if (m_lastAct.hasExpired(30)) {
                ui->label_PhysicalButton->setStyleSheet(m_defStyle);
                m_currentState = state;
            }
        }
    }
}
