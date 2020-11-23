#ifndef AXESCURVESCONFIG_H
#define AXESCURVESCONFIG_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QComboBox;
QT_END_NAMESPACE
//#include <QComboBox>

#include "axescurves.h"

#include "deviceconfig.h"
#include "global.h"

namespace Ui {
class AxesCurvesConfig;
}

class AxesCurvesConfig : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurvesConfig(QWidget *parent = nullptr);
    ~AxesCurvesConfig();

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

    void setDarkInterface(bool isDark);

    void updateAxesCurves();

    void deviceStatus(bool isConnect);

private slots:
    void setCurvesValue(int axisNumber, int pointNumber, int value); // может указатели?
    void profileIndexChanged(int index);

    void on_pushButton_ResetProfiles_clicked();

private:
    Ui::AxesCurvesConfig *ui;
    QList<AxesCurves *> m_axesCurvPtrList;
    QList<QComboBox *> m_profilesCBoxPtrList;

    int m_curvesPointsValue[MAX_AXIS_NUM][CURVE_PLOT_POINTS_COUNT];

    const QStringList m_curvesList = // курве 8 = MAX_AXIS_NUM ! или менять curves_points_value_ в axescurvesconfig
        {{
            {tr("Not used")},
            {tr("Curve 1")},
            {tr("Curve 2")},
            {tr("Curve 3")},
            {tr("Curve 4")},
            {tr("Curve 5")},
            {tr("Curve 6")},
            {tr("Curve 7")},
            {tr("Curve 8")},
        }};
};

#endif // AXESCURVESCONFIG_H
