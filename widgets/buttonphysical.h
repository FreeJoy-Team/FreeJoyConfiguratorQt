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
    void ButtonState(bool is_activated);
    bool is_activated_;

//private: ??
    int button_number_;     // удалить?

private:
    Ui::ButtonPhysical *ui;
};

#endif // BUTTONPHYSICAL_H
