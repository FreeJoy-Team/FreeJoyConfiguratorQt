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
    explicit ButtonPhysical(int buttonNumber, QWidget *parent = nullptr);
    ~ButtonPhysical();
    void setButtonState(bool setState);

private:
    Ui::ButtonPhysical *ui;
    bool m_currentState; // private, set?
    QString m_defStyle;
};

#endif // BUTTONPHYSICAL_H
