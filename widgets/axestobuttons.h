#ifndef AXESTOBUTTONS_H
#define AXESTOBUTTONS_H

#include <QWidget>

namespace Ui {
class AxesToButtons;
}

class AxesToButtons : public QWidget
{
    Q_OBJECT

public:
    explicit AxesToButtons(QWidget *parent = nullptr);
    ~AxesToButtons();

private:
    Ui::AxesToButtons *ui;
};

#endif // AXESTOBUTTONS_H
