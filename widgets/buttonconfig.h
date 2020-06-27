#ifndef BUTTONCONFIG_H
#define BUTTONCONFIG_H

#include <QWidget>

#include "buttonlogical.h"      // ?
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

    // private:?
    ButtonPhysical* physical_button_widget;     // нужны ли?
    ButtonLogical* logical_buttons_widget;

public slots:
    void setUiOnOff(int value);


private:
    QList<ButtonLogical*> LogicButtonAdrList;
    QList<ButtonPhysical*> PhysicButtonAdrList;
    Ui::ButtonConfig *ui;

};

#endif // BUTTONCONFIG_H
