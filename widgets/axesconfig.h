#ifndef AXESCONFIG_H
#define AXESCONFIG_H

#include "axes.h"
#include <QWidget>

#include "deviceconfig.h"
#include "global.h"

namespace Ui {
class AxesConfig;
}

class AxesConfig : public QWidget
{
    Q_OBJECT

public:
    explicit AxesConfig(QWidget *parent = nullptr);
    ~AxesConfig();

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

    void axesValueChanged();

signals:
    void axisRawValueChanged(int);
    void axisOutValueChanged(int);
    void a2bCountChanged(int count);

private slots:
    void addOrDeleteMainSource(int sourceEnum, bool isAdd);
    void a2bCountCalc(int count, int previousCount);
    //    void axesValueChanged(int value);

private:
    Ui::AxesConfig *ui;
    int m_a2bButtonsCount;

    QList<Axes *> m_axesPtrList;
};

#endif // AXESCONFIG_H
