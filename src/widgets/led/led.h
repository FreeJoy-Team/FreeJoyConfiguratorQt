#ifndef LED_H
#define LED_H

#include <QWidget>

#include "deviceconfig.h"
#include "global.h"

#define LED_FUNCTION_COUNT 2
namespace Ui {
class LED;
}

class LED : public QWidget
{
    Q_OBJECT

public:
    explicit LED(int ledNumber, QWidget *parent = nullptr);
    ~LED();

    int currentButtonSelected() const;

    void setLedState(bool state);

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

private:
    Ui::LED *ui;
    int m_ledNumber;

    bool m_currentState;

    const deviceEnum_guiName_t m_ledList[LED_FUNCTION_COUNT] = // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {
        {LED_NORMAL, tr("Normal")},
        {LED_INVERTED, tr("Inverted")},
    };
};

#endif // LED_H
