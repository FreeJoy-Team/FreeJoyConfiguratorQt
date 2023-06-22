#ifndef PINTYPEHELPER_H
#define PINTYPEHELPER_H

#include <QWidget>
#include "common_types.h"

namespace Ui {
class PinTypeHelper;
}

class PinTypeHelper : public QWidget
{
    Q_OBJECT

public:
    explicit PinTypeHelper(QWidget *parent = nullptr);
    ~PinTypeHelper();

signals:
    void helpHovered(pin_t pinType, bool hovered);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::PinTypeHelper *ui;
};

#endif // PINTYPEHELPER_H
