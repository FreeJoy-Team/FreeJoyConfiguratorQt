#ifndef PINCONFIG_H
#define PINCONFIG_H

#include <QWidget>

#include "pincombobox.h"
//#include "currentconfig.h"

QT_BEGIN_NAMESPACE
class QGridLayout;
QT_END_NAMESPACE

#define SOURCE_COUNT 8
#define PIN_TYPE_LIMIT_COUNT 3

class PinsBluePill;
class PinsContrLite;

namespace Ui {
class PinConfig;
}

class PinConfig : public QWidget
{
    Q_OBJECT

public:
    explicit PinConfig(QWidget *parent = nullptr);
    ~PinConfig();
    void writeToConfig();
    void readFromConfig();

    bool limitIsReached();

    void retranslateUi();

    void resetAllPins();

signals:
    void totalButtonsValueChanged(int count);
    void totalLEDsValueChanged(int totalLed);
    void fastEncoderSelected(const QString &pinGuiName, bool isSelected);
    void shiftRegSelected(int latchPin, int clkPin, int dataPin, const QString &pinGuiName);
    void i2cSelected(bool i2cSelected);
    void axesSourceChanged(int sourceEnum, const QString &sourceName, bool isAdd);
    void limitReached(bool limit);
    void ledPwmSelected(Pin pin, bool selected);
    void ledRgbSelected(Pin pin, bool selected);
    //void pinTypeSelected(Pin pin, pin_types_t type, bool selected);

    //protected:
    //    void resizeEvent(QResizeEvent*) override;

public slots:
    void a2bCountChanged(int);
    void shiftRegButtonsCountChanged(int count);
    void highlightPins(pin_t pinType, bool enable);
private slots:
    void pinInteraction(int index, int senderIndex, int pin);
    void pinIndexChanged(int currentDeviceEnum, int previousDeviceEnum, int pinNumber, QString pinName);
    void boardChanged(int index);

private:
    Ui::PinConfig *ui;

    PinsBluePill *m_bluePill;
    PinsContrLite *m_contrLite;
    int m_lastBoard;

    //! PinComboBox widget list
    QList<PinComboBox *> m_pinCBoxPtrList;

    QString m_defaultLabelStyle;
    bool m_maxButtonsWarning;

    int m_shiftLatchCount;
    int m_shiftDataCount;
    int m_shiftClkCount;

    void signalsForWidgets(int currentDeviceEnum, int previousDeviceEnum, int pinNumber, QString pinName);
    void pinTypeLimit(int currentDeviceEnum, int previousDeviceEnum);
    void setCurrentConfig(int currentDeviceEnum, int previousDeviceEnum, int pinNumber, QString pinName);
    void blockPA8PWM(int currentDeviceEnum, int previousDeviceEnum);
    void blockPA10RGB(int currentDeviceEnum, int previousDeviceEnum, int pinNumber);

    struct source_t
    {
        int type;
        int pinType[PIN_TYPE_COUNT];
    };

    struct pinTypeLimit_t
    {
        int deviceEnumIndex;
        int maxCount;
    };

    const source_t m_source[SOURCE_COUNT] =
    {
        {AXIS_SOURCE,        {AXIS_ANALOG, TLE5011_CS, MCP3201_CS, MCP3202_CS, MCP3204_CS, MCP3208_CS, MLX90393_CS, MLX90363_CS, AS5048A_CS, TLE5012_CS}},

        {BUTTON_FROM_AXES,   {678}},        // 678 в DeviceConfig

        {SINGLE_BUTTON,      {BUTTON_VCC, BUTTON_GND}},
        {ROW_OF_BUTTONS,     {BUTTON_ROW}},
        {COLUMN_OF_BUTTONS,  {BUTTON_COLUMN}},

        {SINGLE_LED,         {LED_SINGLE}},
        {ROW_OF_LED,         {LED_ROW}},
        {COLUMN_OF_LED,      {LED_COLUMN}},
    };

    const pinTypeLimit_t m_pinTypeLimit[PIN_TYPE_LIMIT_COUNT] =       // static ?
    {
        {SHIFT_REG_LATCH,        4},
        {SHIFT_REG_DATA,         4},
        {SHIFT_REG_CLK,          4},
    };

    enum        // and in current config
    {
        AXIS_SOURCE = 0,
        BUTTON_FROM_AXES,
        SINGLE_BUTTON,
        ROW_OF_BUTTONS,
        COLUMN_OF_BUTTONS,
        SINGLE_LED,
        ROW_OF_LED,
        COLUMN_OF_LED,
    };
};

#endif // PINCONFIG_H
