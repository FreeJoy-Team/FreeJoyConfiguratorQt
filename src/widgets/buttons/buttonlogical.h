#ifndef BUTTONLOGICAL_H
#define BUTTONLOGICAL_H

#include "common_types.h"
#include <QWidget>

#include "deviceconfig.h"
#include "global.h"
//#include <QThread>

#define TIMER_COUNT 4 // "NO timer" + count
#define SHIFT_COUNT 6

namespace Ui {
class ButtonLogical;
}

class ButtonLogical : public QWidget
{
    Q_OBJECT

public:
    explicit ButtonLogical(int buttonIndex, QWidget *parent = nullptr);
    ~ButtonLogical();
    void readFromConfig();
    void writeToConfig();

    void initialization();

    void setMaxPhysButtons(int maxPhysButtons);
    void setSpinBoxOnOff(int maxPhysButtons);

    void setButtonState(bool setState);

    void setPhysicButton(int buttonIndex);
    void setAutoPhysBut(bool enabled);
    int currentFocus() const;

    void disableButtonType(button_type_t type, bool disable);
    button_type_t currentButtonType();

    void retranslateUi();

signals:
    void functionTypeChanged(button_type_t current, button_type_t previous, int buttonIndex);

private slots:
    void editingOnOff(int value);
    void functionIndexChanged(int index);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::ButtonLogical *ui;
    int m_functionPrevType;
    bool m_currentState;
    bool m_debugState;
    int m_buttonIndex;
    static int m_currentFocus;
    static bool m_autoPhysButEnabled;
    QElapsedTimer m_lastAct;

    QVector<int> m_logicFunc_enumIndex;

    const deviceEnum_guiName_t m_timerList[TIMER_COUNT] = // order must be as in common_types.h!!!!!!!!!!!          // static ?
    {
        {BUTTON_TIMER_OFF,      tr("-")},
        {BUTTON_TIMER_1,        tr("Timer 1")},
        {BUTTON_TIMER_2,        tr("Timer 2")},
        {BUTTON_TIMER_3,        tr("Timer 3")},
    };

    const deviceEnum_guiName_t m_shiftList[SHIFT_COUNT] = // order must be as in common_types.h!!!!!!!!!!!          // static ?
    {
        {0,        tr("-")},
        {1,        tr("Shift 1")},
        {2,        tr("Shift 2")},
        {3,        tr("Shift 3")},
        {4,        tr("Shift 4")},
        {5,        tr("Shift 5")},
    };
    //static deviceEnum_guiName_t logical_function_list_[LOGICAL_FUNCTION_COUNT];

    const QVector <deviceEnum_guiName_t> m_logicFunctionList = // any order          // static ?
    {{
        {BUTTON_NORMAL,        tr("Button normal")},
        {BUTTON_TOGGLE,        tr("Button toggle")},
        {TOGGLE_SWITCH,        tr("Toggle switch ON/OFF")},
        {TOGGLE_SWITCH_ON,     tr("Toggle switch ON")},
        {TOGGLE_SWITCH_OFF,    tr("Toggle switch OFF")},
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
    }};
};

#endif // BUTTONLOGICAL_H
