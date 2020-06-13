#ifndef LED_H
#define LED_H

#include <QWidget>

namespace Ui {
class LED;
}

class LED : public QWidget
{
    Q_OBJECT

public:
    explicit LED(QWidget *parent = nullptr);
    ~LED();

private:
    Ui::LED *ui;
};

#endif // LED_H
