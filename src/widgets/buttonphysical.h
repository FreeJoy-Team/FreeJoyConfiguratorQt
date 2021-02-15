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

signals:
    void physButtonPressed(int number);

private:
    Ui::ButtonPhysical *ui;
    int m_buttonNumber;
    bool m_currentState;
    QString m_defStyle;
};

#endif // BUTTONPHYSICAL_H
