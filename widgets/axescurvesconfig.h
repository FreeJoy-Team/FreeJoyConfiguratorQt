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


private:
    Ui::AxesCurvesConfig *ui;
    QList<AxesCurves*> AxesCurvAdrList;
};

#endif // AXESCURVESCONFIG_H
