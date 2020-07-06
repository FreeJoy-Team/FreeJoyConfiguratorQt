#ifndef AXESTOBUTTONS_H
#define AXESTOBUTTONS_H

#include <QWidget>
#include "axestobuttonsslider.h"

#include "global.h"
#include "deviceconfig.h"

namespace Ui {
class AxesToButtons;
}

class AxesToButtons : public QWidget
{
    Q_OBJECT

public:
    explicit AxesToButtons(int a2b_number, QWidget *parent = nullptr);
    ~AxesToButtons();

    void ReadFromConfig();
    void WriteToConfig();

private slots:
    void a2bCountChanged(int count);
    void a2bCheckBoxValueChanged(bool value);

private:
    Ui::AxesToButtons *ui;
    int a2b_number_;

    std::vector <deviceEnum_guiName_t> axes_list_ =     // дублируется в axes.h
    {{
        {0,      tr("X")},
        {1,      tr("Y")},
        {2,      tr("Z")},
        {3,      tr("Rx")},
        {4,      tr("Ry")},
        {5,      tr("Rz")},
        {6,      tr("Slider 1")},
        {7,      tr("Slider 2")},
    }};
};

#endif // AXESTOBUTTONS_H
