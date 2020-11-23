#ifndef AXESCURVES_H
#define AXESCURVES_H

#include "widgets/axescurvesplot.h"
#include <QWidget>

#include "deviceconfig.h"
#include "global.h"

namespace Ui {
class AxesCurves;
}

class AxesCurves : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurves(int axisNumber, QWidget *parent = nullptr);
    ~AxesCurves();

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

    void setDarkIcon(bool isDark);

    int getPointValue(int pointNumber) const;
    void setPointValue(int point_number, int value);

    void setCurveProfile(int profile);

    void updateAxis();
    void deviceStatus(bool isConnect);

signals:
    void deviceStatusChanged(bool is_connect);

    void axisCurveIndexChanged(int axisNumber, int index);
    void curvePointValueChanged(int axisNumber, int pointNumber, int value);
    //void resetCurvesProfiles();

private slots:
    void on_pushButton_Linear_clicked();
    void on_pushButton_Exponent_clicked();
    void on_pushButton_ExponentInvert_clicked();
    void on_pushButton_Shape_clicked();

    void pointValueChanged(const int *pointNumber, const int *value);

    void on_groupBox_Curve_clicked(bool checked);

private:
    Ui::AxesCurves *ui;
    int m_axisNumber;
    int m_currentProfile;
    int m_curvePointsValue[CURVE_PLOT_POINTS_COUNT];

    QVector<deviceEnum_guiName_t> m_axesList = // дублируется в axestobuttons.h , axes.h // ОНО ВЕЗДЕ, ВЫНЕСТИ В ГЛОБАЛ
        {{
            {0, tr("X")},
            {1, tr("Y")},
            {2, tr("Z")},
            {3, tr("Rx")},
            {4, tr("Ry")},
            {5, tr("Rz")},
            {6, tr("Slider 1")},
            {7, tr("Slider 2")},
        }};
};

#endif // AXESCURVES_H
