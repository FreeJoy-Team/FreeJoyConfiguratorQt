#ifndef CENTEREDCBOX_H
#define CENTEREDCBOX_H

#include <QComboBox>

class CenteredCBox : public QComboBox
{
    //Q_OBJECT
public:
    explicit CenteredCBox(QWidget *parent = nullptr); //, int arrow_width = 18

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_arrowWidth;
};

#endif // CENTEREDCBOX_H
