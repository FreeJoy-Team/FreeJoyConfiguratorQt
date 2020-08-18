#ifndef AXES_H
#define AXES_H

#include <QWidget>
#include "converter.h"
#include "widgets/axesextended.h"
#include "widgets/axestobuttonsslider.h"

#include "global.h"
#include "deviceconfig.h"

namespace Ui {
class Axes;
}

class Axes : public QWidget     // наверно надо было назвать axis
{
    Q_OBJECT

public:
    explicit Axes(int axis_number, QWidget *parent = nullptr);
    ~Axes();

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

    void UpdateAxisRaw();
    void UpdateAxisOut();

    void AddOrDeleteMainSource(int source_enum, bool is_add);

signals:
    void a2bCountChanged(int count, int previous_count);

private slots:
    void calibMinMaxValueChanged(int value);
    void calibrationStarted(int raw_value);
    void mainSourceIndexChanged(int index);
//    void axisRawValueChanged(int);
//    void axisOutValueChanged(int);
    void a2bSpinBoxChanged(int count);

    void outputValueChanged(bool is_checked);

    void on_pushButton_StartCalib_clicked(bool checked);

    void on_pushButton_SetCenter_clicked();

    void on_checkBox_Center_stateChanged(int state);

    void on_pushButton_clicked();

    void on_checkBox_ShowExtend_stateChanged(int state);

private:
    Ui::Axes *ui;
    const int kMinA2bButtons = 2;
    bool calibration_started_;
    int a2b_buttons_count_;
    QString start_calibration = tr("Calibrate");    // const? а они переименовываются?
    QString stop_calibration = tr("Stop and Save");
    //dev_config_t* pDev_config;
    AxesExtended * axes_extend;


    std::vector<int> main_source_enum_index_;

    enum
    {
        Encoder = -3,
        I2C = -2,
        None = -1,
        A0 = 0,
        A1,
        A2,
        A3,
        A4,
        A5,
        A6,
        A7,
        A8,
        A9,
        A10,
        A15,
        B0,
        B1,
        B3,
        B4,
        B5,
        B6,
        B7,
        B8,
        B9,
        B10,
        B11,
        B12,
        B13,
        B14,
        B15,
        C13,
        C14,
        C15,
    };

    // const?
    std::vector <deviceEnum_guiName_t> axes_pin_list_ =      // любая последовательность, но первые 2 добавляются в конструкторе
    {{
        {None,     tr("None")},
        {Encoder,  tr("Encoder")},
        {I2C,      "I2C"},
        {A0,       "A0"},
        {A1,       "A1"},
        {A2,       "A2"},
        {A3,       "A3"},
        {A4,       "A4"},
        {A5,       "A5"},
        {A6,       "A6"},
        {A7,       "A7"},
        {A8,       "A8"},
        {A9,       "A9"},
        {A10,      "A10"},
        {A15,      "A15"},
        {B0,       "B0"},
        {B1,       "B1"},
        {B3,       "B3"},
        {B4,       "B4"},
        {B5,       "B5"},
        {B6,       "B6"},
        {B7,       "B7"},
        {B8,       "B8"},
        {B9,       "B9"},
        {B10,      "B10"},
        {B11,      "B11"},
        {B12,      "B12"},
        {B13,      "B13"},
        {B14,      "B14"},
        {B15,      "B15"},
        {C13,      "C13"},
        {C14,      "C14"},
        {C15,      "C15"},
    }};

    std::vector <deviceEnum_guiName_t> axis_source_main_ =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {{
        {None,          tr("None")},
        {Encoder,       tr("Encoder")},
    }};

    std::vector <deviceEnum_guiName_t> axes_list_ =     // дублируется в axestobuttons.h , axescurves.h, axes.h, axesextended.h // да бля вынести его уже в глобал
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

    int axis_number_;
};

#endif // AXES_H
