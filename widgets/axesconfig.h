#ifndef AXESCONFIG_H
#define AXESCONFIG_H

#include <QWidget>
#include "axes.h"

#include "global.h"
#include "deviceconfig.h"

namespace Ui {
class AxesConfig;
}

class AxesConfig : public QWidget
{
    Q_OBJECT

public:
    explicit AxesConfig(QWidget *parent = nullptr);
    ~AxesConfig();

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

    void AxesValueChanged();

signals:
    void axisRawValueChanged(int);
    void axisOutValueChanged(int);
    void a2bCountChanged(int count);

private slots:
    void addOrDeleteMainSource(int source_enum, bool is_add);
    void a2bCountCalc(int count, int previous_count);
//    void axesValueChanged(int value);

private:
    Ui::AxesConfig *ui;
    int a2b_buttons_count_;

    QList<Axes*> AxesAdrList;
};

#endif // AXESCONFIG_H
