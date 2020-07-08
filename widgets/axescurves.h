#ifndef AXESCURVES_H
#define AXESCURVES_H

#include <QWidget>

namespace Ui {
class AxesCurves;
}

class AxesCurves : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurves(QWidget *parent = nullptr);
    ~AxesCurves();

private:
    Ui::AxesCurves *ui;

};

#endif // AXESCURVES_H
