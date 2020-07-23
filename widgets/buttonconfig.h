#ifndef BUTTONCONFIG_H
#define BUTTONCONFIG_H

#include <QWidget>

#include "buttonlogical.h"
#include "buttonphysical.h"

#include "common_defines.h"
#include "common_types.h"

#include "global.h"
#include "deviceconfig.h"


namespace Ui {
class ButtonConfig;
}

class ButtonConfig : public QWidget
{
    Q_OBJECT

public:
    explicit ButtonConfig(QWidget *parent = nullptr);
    ~ButtonConfig();

    void ReadFromConfig();
    void WriteToConfig();
    void ButtonStateChanged();

    void RetranslateUi();

    // private:?
    //ButtonPhysical* physical_button_widget;     // нужны ли?
    //ButtonLogical* logical_buttons_widget;

signals:
    void encoderInputChanged(int ecoder_A, int ecoder_B);
    //void buttonLEDStateChanged(int button_number);            // for future

public slots:   // private?
    void setUiOnOff(int value);

private slots:
    void functionTypeChanged(int index, int function_previous_index, int button_number);

private:
    Ui::ButtonConfig *ui;
    void PhysicalButtonsSpawn(int count);
    QString default_shift_style_;
    bool is_shifts_activated_;
    bool shift1_activated_;
    bool shift2_activated_;
    bool shift3_activated_;
    bool shift4_activated_;
    bool shift5_activated_;


    QList<ButtonLogical*> LogicButtonAdrList;
    QList<ButtonPhysical*> PhysicButtonAdrList;

};

#endif // BUTTONCONFIG_H
