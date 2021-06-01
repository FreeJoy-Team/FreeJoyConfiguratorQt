#ifndef BUTTONPHYSICAL_H
#define BUTTONPHYSICAL_H

#include <QWidget>
#include <QElapsedTimer>

namespace Ui {
class ButtonPhysical;
}

class ButtonPhysical : public QWidget
{
    Q_OBJECT

public:
    explicit ButtonPhysical(int buttonNumber, QWidget *parent = nullptr);
    ~ButtonPhysical();
    void setButtonState(bool state);

signals:
    void physButtonPressed(int number);

private:
    Ui::ButtonPhysical *ui;
    int m_buttonIndex;
    bool m_currentState;
    QString m_defStyle;
    QElapsedTimer m_lastAct;
};

#endif // BUTTONPHYSICAL_H
