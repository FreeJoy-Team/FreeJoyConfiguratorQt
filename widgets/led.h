#ifndef LED_H
#define LED_H

#include <QWidget>

#include "global.h"
#include "deviceconfig.h"

#define LED_FUNCTION_COUNT 2
namespace Ui {
class LED;
}

class LED : public QWidget
{
    Q_OBJECT

public:
    explicit LED(int led_number, QWidget *parent = nullptr);
    ~LED();

    int CurrentButtonSelected();

    void LEDStateChanged(bool set_state);

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

private:
    Ui::LED *ui;
    int led_number_;

    bool current_state_;

    deviceEnum_guiName_t const led_list_[LED_FUNCTION_COUNT] =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {
        {LED_NORMAL,          tr("Normal")},
        {LED_INVERTED,        tr("Inverted")},
    };
};

#endif // LED_H
