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

    void RetranslateUi();

signals:
    void a2bCountChanged(int count);

public slots:
    void a2bCountCalc(int count, int previous_count);

private:
    Ui::AxesToButtonsConfig *ui;

    int a2b_buttons_count_;
    QList<AxesToButtons*> A2bAdrList;
};

#endif // AXESTOBUTTONSCONFIG_H
