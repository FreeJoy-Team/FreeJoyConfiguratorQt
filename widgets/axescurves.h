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
    explicit AxesCurves(int axis_number, QWidget *parent = nullptr);
    ~AxesCurves();

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

    void SetDarkIcon(bool is_dark);

    void UpdateAxis();
    void DeviceStatus(bool is_connect);

signals:
    void deviceStatusChanged(bool is_connect);

private slots:
    void on_pushButton_Linear_clicked();
    void on_pushButton_LinearInvert_clicked();
    void on_pushButton_Exponent_clicked();
    void on_pushButton_ExponentInvert_clicked();
    void on_pushButton_Shape_clicked();

private:
    Ui::AxesCurves *ui;
    int axis_number_;
    bool is_device_connected_ = false;

    std::vector <deviceEnum_guiName_t> axes_list_ =     // дублируется в axestobuttons.h , axes.h // везде, ВЫНЕСТИ ОТДЕЛЬНО
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

#endif // AXESCURVES_H
