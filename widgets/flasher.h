#ifndef FLASHER_H
#define FLASHER_H

#include <QWidget>

namespace Ui {
class Flasher;
}

class Flasher : public QWidget
{
    Q_OBJECT

public:
    explicit Flasher(QWidget *parent = nullptr);
    ~Flasher();

private:
    Ui::Flasher *ui;
};

#endif // FLASHER_H
