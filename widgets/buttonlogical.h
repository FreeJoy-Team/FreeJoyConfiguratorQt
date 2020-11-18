#ifndef BUTTONLOGICAL_H
#define BUTTONLOGICAL_H

#include <QWidget>
#include "common_types.h"

#include "global.h"
#include "deviceconfig.h"
//#include <QThread>

#define TIMER_COUNT 4       // "NO timer" + count
#define SHIFT_COUNT 6
#define LOGICAL_FUNCTION_COUNT 31

namespace Ui {
class ButtonLogical;
}

class ButtonLogical : public QWidget
{
    Q_OBJECT

public:
    explicit ButtonLogical(int button_number, QWidget *parent = nullptr);
    ~ButtonLogical();
    void ReadFromConfig();
    void WriteToConfig();

    void Initialization();

    void SetMaxPhysButtons(int max_phys_buttons);
    void SetSpinBoxOnOff(int max_phys_buttons);

    void SetButtonState(bool set_state);

    void RetranslateUi();

//private: ??
    int button_number_;

    const deviceEnum_guiName_t timer_list_[TIMER_COUNT] =       // порядов обязан быть как в common_types.h!!!!!!!!!!!          // static ?
    {
        {BUTTON_TIMER_OFF,      tr("No")},
        {BUTTON_TIMER_1,        tr("Timer 1")},
        {BUTTON_TIMER_2,        tr("Timer 2")},
        {BUTTON_TIMER_3,        tr("Timer 3")},
    };

    const deviceEnum_guiName_t shift_list_[SHIFT_COUNT] =       // порядов обязан быть как в common_types.h!!!!!!!!!!!          // static ?
    {
        {0,        tr("No shift")},
        {1,        tr("Shift 1")},
        {2,        tr("Shift 2")},
        {3,        tr("Shift 3")},
        {4,        tr("Shift 4")},
        {5,        tr("Shift 5")},
    };
    // ну статик даже 0.1мб выигрыша не дал, хз. С ним правильнее, но пока так
    //static deviceEnum_guiName_t logical_function_list_[LOGICAL_FUNCTION_COUNT];

    const deviceEnum_guiName_t logical_function_list_[LOGICAL_FUNCTION_COUNT] =     // порядов обязан быть как в common_types.h!!!!!!!!!!!          // static ?
    {
        {BUTTON_NORMAL,        tr("Button normal")},
        {BUTTON_TOGGLE,        tr("Button toggle")},
        {TOGGLE_SWITCH,        tr("Toggle switch")},
        {TOGGLE_SWITCH_ON,     tr("Toggle switch ON")},
        {TOGGLE_SWITCH_OFF,    tr("Toggle switch ON/OFF")},
        {POV1_UP,              tr("POV1 Up")},
        {POV1_RIGHT,           tr("POV1 Right")},
        {POV1_DOWN,            tr("POV1 Down")},
        {POV1_LEFT,            tr("POV1 Left")},
        {POV1_CENTER,          tr("POV1 Center")},
        {POV2_UP,              tr("POV2 Up")},
        {POV2_RIGHT,           tr("POV2 Right")},
        {POV2_DOWN,            tr("POV2 Down")},
        {POV2_LEFT,            tr("POV2 Left")},
        {POV2_CENTER,          tr("POV2 Center")},
        {POV3_UP,              tr("POV3 Up")},
        {POV3_RIGHT,           tr("POV3 Right")},
        {POV3_DOWN,            tr("POV3 Down")},
        {POV3_LEFT,            tr("POV3 Left")},
        {POV4_UP,              tr("POV4 Up")},
        {POV4_RIGHT,           tr("POV4 Right")},
        {POV4_DOWN,            tr("POV4 Down")},
        {POV4_LEFT,            tr("POV4 Left")},
        {ENCODER_INPUT_A,      tr("Encoder A")},
        {ENCODER_INPUT_B,      tr("Encoder B")},
        {RADIO_BUTTON1,        tr("Radio button 1")},
        {RADIO_BUTTON2,        tr("Radio button 2")},
        {RADIO_BUTTON3,        tr("Radio button 3")},
        {RADIO_BUTTON4,        tr("Radio button 4")},
        {SEQUENTIAL_TOGGLE,    tr("Sequential toggle")},
        {SEQUENTIAL_BUTTON,    tr("Sequential button")},
    };

signals:
    void functionIndexChanged(int index, int previous_index, int button_number);

private slots:
    void editingOnOff (int value);
    void functionTypeChanged(int index);

private:
    Ui::ButtonLogical *ui;
    int function_previous_index_;
    bool current_state_;
};

#endif // BUTTONLOGICAL_H

