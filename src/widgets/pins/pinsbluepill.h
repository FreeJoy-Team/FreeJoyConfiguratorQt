#ifndef PINBLUEPILL_H
#define PINBLUEPILL_H

#include <QWidget>
#include "pincombobox.h"

QT_BEGIN_NAMESPACE
class QGridLayout;
QT_END_NAMESPACE

namespace Ui {
class PinsBluePill;
}

class PinsBluePill : public QWidget
{
    Q_OBJECT

public:
    explicit PinsBluePill(QWidget *parent = nullptr);
    ~PinsBluePill();

    void addPinComboBox (QList<PinComboBox *> pinList);

private:
    Ui::PinsBluePill *ui;
};

#endif // PINBLUEPILL_H
