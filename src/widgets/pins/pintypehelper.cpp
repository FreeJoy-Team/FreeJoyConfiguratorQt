#include "pintypehelper.h"
#include "ui_pintypehelper.h"
#include <QHoverEvent>
#include <QDebug>

PinTypeHelper::PinTypeHelper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinTypeHelper)
{
    ui->setupUi(this);

    // не уверен как лучше сделать, так или создать новый класс HoverLabel унаследованный от QLabel
    for (auto &&c : ui->groupBox->children()) {
        QLabel *label = qobject_cast<QLabel*>(c);
        if (label) {
            label->installEventFilter(this);
            label->setMouseTracking(true);
            label->setAttribute(Qt::WA_Hover, true);
        }
    }

    ui->label_analogAxis->setProperty("pinType", AXIS_ANALOG);
    ui->label_buttons->setProperty("pinType", BUTTON_GND);
    ui->label_shiftReg->setProperty("pinType", SHIFT_REG_LATCH);
    ui->label_spi->setProperty("pinType", MCP3201_CS);
    ui->label_i2c->setProperty("pinType", I2C_SDA);
    ui->label_rgbLed->setProperty("pinType", LED_RGB);
    ui->label_pwmLed->setProperty("pinType", LED_PWM);
    ui->label_monoLed->setProperty("pinType", LED_SINGLE);
    ui->label_fastEncoder->setProperty("pinType", FAST_ENCODER);
}

PinTypeHelper::~PinTypeHelper()
{
    delete ui;
}


bool PinTypeHelper::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter) {
        bool ok;
        pin_t pinType = obj->property("pinType").toInt(&ok);
        if (ok) {
            emit helpHovered(pinType, true);
        }
    } else if (event->type() == QEvent::HoverLeave) {
        bool ok;
        pin_t pinType = obj->property("pinType").toInt(&ok);
        if (ok) {
            emit helpHovered(pinType, false);
        }
    }
    return false;
}
