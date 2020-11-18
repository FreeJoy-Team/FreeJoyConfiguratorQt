#ifndef AXESCURVES_H
#define AXESCURVES_H

#include <QWidget>
#include "widgets/axescurvesplot.h"

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

    int GetPointValue(int point_number) const;
    void SetPointValue(int point_number, int value);

    void SetCurveProfile(int profile);

    void UpdateAxis();
    void DeviceStatus(bool is_connect);

signals:
    void deviceStatusChanged(bool is_connect);

    void axisCurveIndexChanged(int axis_number, int index);
    void curvePointValueChanged(int axis_number, int point_number, int value);
    //void resetCurvesProfiles();

private slots:
    void on_pushButton_Linear_clicked();
    void on_pushButton_Exponent_clicked();
    void on_pushButton_ExponentInvert_clicked();
    void on_pushButton_Shape_clicked();

    void pointValueChanged(const int *point_number, const int *value);

    void on_groupBox_Curve_clicked(bool checked);

private:
    Ui::AxesCurves *ui;
    int axis_number_;
    int current_profile_;
    int curve_points_value_[CURVE_PLOT_POINTS_COUNT];

    QVector <deviceEnum_guiName_t> axes_list_ =     // дублируется в axestobuttons.h , axes.h // ОНО ВЕЗДЕ, ВЫНЕСТИ В ГЛОБАЛ
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
