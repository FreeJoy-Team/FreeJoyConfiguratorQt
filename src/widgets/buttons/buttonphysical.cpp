#include "buttonphysical.h"
#include "ui_buttonphysical.h"

const QString ButtonPhysical::m_whiteStyleOff = QStringLiteral(R"(
    QLabel {
        border-radius: 13px;
        min-height: 26px;
        min-width: 26px;
        background-color: rgb(170, 60, 60);
        color: rgb(220, 221, 222);
    }
)");

//background-color: rgb(170, 70, 40); // background-color: rgb(90, 0, 0);
//border: 1px solid #4b4b4b;
const QString ButtonPhysical::m_darkStyleOff = QStringLiteral(R"(
    QLabel {
        border-radius: 13px;
        min-height: 26px;
        min-width: 26px;
        background-color: rgb(90, 90, 90);
        color: rgb(220, 221, 222);
    }
)");

const QString ButtonPhysical::m_styleOn = QStringLiteral(R"(
    QLabel {
        border-radius: 13px;
        min-height: 26px;
        min-width: 26px;
        background-color: rgb(0, 128, 0);
        color: rgb(220, 221, 222);
    }
)");

ButtonPhysical::ButtonPhysical(int buttonNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ButtonPhysical)
{
    ui->setupUi(this);
    m_currentState = false;
    m_buttonIndex = buttonNumber;
    ui->label_PhysicalButton->setNum(m_buttonIndex + 1);

    updateStyleColor();
    installEventFilter(this);
}

ButtonPhysical::~ButtonPhysical()
{
    delete ui;
}

void ButtonPhysical::setButtonState(bool state)
{
    if (state != m_currentState) {
        if (state) {
            ui->label_PhysicalButton->setStyleSheet(m_styleOn);
            emit physButtonPressed(m_buttonIndex);
            m_lastAct.start();
            m_currentState = state;
        } else {
            // sometimes state dont have time to render. e.g. encoder press time 10ms and monitor refresh time 17ms(60fps)
            if (m_lastAct.hasExpired(30)) {
                updateStyleColor();
                m_currentState = state;
            }
        }
    }
}

void ButtonPhysical::updateStyleColor()
{
    if (QApplication::palette().color(QWidget::backgroundRole()).value() < 100) {
        ui->label_PhysicalButton->setStyleSheet(m_darkStyleOff);
    } else {
        ui->label_PhysicalButton->setStyleSheet(m_whiteStyleOff);
    }
    update();
}

bool ButtonPhysical::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)
    if (event->type() == QEvent::ApplicationPaletteChange) {
        updateStyleColor();
        return true;
    }
    return false;
}
