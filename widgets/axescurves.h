#ifndef AXESCURVES_H
#define AXESCURVES_H

#include <QWidget>
#include "global.h"
#include "deviceconfig.h"

namespace Ui {
class AxesCurves;
}

class AxesCurves : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurves(int axes_number, QWidget *parent = nullptr);
    ~AxesCurves();

    void ReadFromConfig();
    void WriteToConfig();

private:
    Ui::AxesCurves *ui;
    int axes_number_;

};

#endif // AXESCURVES_H
