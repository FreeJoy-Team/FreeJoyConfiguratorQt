#ifndef AXESCURVESCONFIG_H
#define AXESCURVESCONFIG_H

#include <QWidget>
#include "axescurves.h"

#include "global.h"
#include "deviceconfig.h"


namespace Ui {
class AxesCurvesConfig;
}

class AxesCurvesConfig : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurvesConfig(QWidget *parent = nullptr);
    ~AxesCurvesConfig();

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

    void SetDarkInterface(bool is_dark);

    void UpdateAxesCurves();

    void DeviceStatus(bool is_connect);

private slots:
    void axisCurveIndexChanged(int axis_number, int index);
    void setCurvesValue(int axis_number, int point_number, int value); // может указатели?
    void resetCurvesProfiles();

private:
    Ui::AxesCurvesConfig *ui;
    QList<AxesCurves*> AxesCurvAdrList;

    int curves_points_value_[MAX_AXIS_NUM][CURVE_PLOT_POINTS_COUNT];
};

#endif // AXESCURVESCONFIG_H
