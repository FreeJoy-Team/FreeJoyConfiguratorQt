#ifndef AXESTOBUTTONSCONFIG_H
#define AXESTOBUTTONSCONFIG_H

#include <QWidget>
#include "axestobuttons.h"

namespace Ui {
class AxesToButtonsConfig;
}

class AxesToButtonsConfig : public QWidget
{
    Q_OBJECT

public:
    explicit AxesToButtonsConfig(QWidget *parent = nullptr);
    ~AxesToButtonsConfig();

    void ReadFromConfig();
    void WriteToConfig();

private:
    Ui::AxesToButtonsConfig *ui;
    QList<AxesToButtons*> A2bAdrList;
};

#endif // AXESTOBUTTONSCONFIG_H
