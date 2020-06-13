#ifndef ENCODERS_H
#define ENCODERS_H

#include <QWidget>

namespace Ui {
class Encoders;
}

class Encoders : public QWidget
{
    Q_OBJECT

public:
    explicit Encoders(QWidget *parent = nullptr);
    ~Encoders();

private:
    Ui::Encoders *ui;
};

#endif // ENCODERS_H
