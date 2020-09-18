#ifndef BUTTONPHYSICAL_H
#define BUTTONPHYSICAL_H

#include <QWidget>

namespace Ui {
class ButtonPhysical;
}

class ButtonPhysical : public QWidget
{
    Q_OBJECT

public:
    explicit ButtonPhysical(int button_number, QWidget *parent = nullptr);
    ~ButtonPhysical();
    void SetButtonState(bool set_state);

private:
    Ui::ButtonPhysical *ui;
    bool current_state_; // private, set?
};

#endif // BUTTONPHYSICAL_H
