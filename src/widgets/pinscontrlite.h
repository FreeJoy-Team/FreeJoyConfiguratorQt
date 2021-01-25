#ifndef PINCONTRLITE_H
#define PINCONTRLITE_H

#include <QWidget>
#include "pincombobox.h"

QT_BEGIN_NAMESPACE
class QGridLayout;
QT_END_NAMESPACE

namespace Ui {
class PinsContrLite;
}

class PinsContrLite : public QWidget
{
    Q_OBJECT

public:
    explicit PinsContrLite(QWidget *parent = nullptr);
    ~PinsContrLite();

    void addPinComboBox (QList<PinComboBox *> pinList);

private:
    Ui::PinsContrLite *ui;
};

#endif // PINCONTRLITE_H
