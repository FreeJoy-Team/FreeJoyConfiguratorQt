#include "pinscontrlite.h"
#include "ui_pinscontrlite.h"
#include <QComboBox>

PinsContrLite::PinsContrLite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinsContrLite)
{
    ui->setupUi(this);
}

PinsContrLite::~PinsContrLite()
{
    delete ui;
}

void PinsContrLite::addPinComboBox(QList<PinComboBox *> pinList)
{
    int tmp = 0;
    QComboBox *prevCBox = nullptr;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < ui->layoutG_pins->rowCount(); ++j) {
            if (!ui->layoutG_pins->itemAtPosition(j, i*3)) {
                continue;
            }
            QLabel *label = qobject_cast<QLabel *>(ui->layoutG_pins->itemAtPosition(j, i*3)->widget());
            if (label) {
                for (int k = 0; k < pinList.size(); ++k) {
                    if (pinList[k]->objectName() == label->text()) {// clone pinList and remove if addWidget?
                        ui->layoutG_pins->addWidget(pinList[k], j, i*3+1);
                        tmp++;
                        // tab order
                        QComboBox *child = pinList[k]->findChild<QComboBox*>();
                        if (!prevCBox) {
                            child->setTabOrder(this, child);
                            prevCBox = child;
                        } else {
                            child->setTabOrder(prevCBox, child);
                            prevCBox = child;
                        }
                        break;
                    }
                }
            }
        }
    }
    Q_ASSERT(tmp == PINS_COUNT);
}
