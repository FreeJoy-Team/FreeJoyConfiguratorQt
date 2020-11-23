#ifndef PINCONFIG_H
#define PINCONFIG_H

#include <QWidget>

#include "pincombobox.h"

#define SOURCE_COUNT 8
#define PIN_TYPE_LIMIT_COUNT 2

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

    void retranslateUi();

    void resetAllPins();

signals:
    void totalButtonsValueChanged(int totalButtons);
    void totalLEDsValueChanged(int totalLed);
    void fastEncoderSelected(QString pinGuiName, bool isSelected);
    void shiftRegSelected(int latchPin, int dataPin, QString pinGuiName);
    void i2cSelected(bool i2cSelected);
    void axesSourceChanged(int sourceEnum, bool isAdd);

    //protected:
    //    void resizeEvent(QResizeEvent*) override;

private slots:
    void pinInteraction(int index, int senderIndex, int pin);
    void pinIndexChanged(int currentDeviceEnum, int previousDeviceEnum, int pinNumber);
    void a2bCountChanged(int count);
    void shiftRegButtonsCountChanged(int count);
    void totalButtonsChanged(int count);

private:
    Ui::PinConfig *ui;
    //! 0 if no pins
    uint m_pinCount; // пинов всегда 30, бесполезная хрень
    //! PinComboBox widget list
    QList<PinComboBox *> m_pinCBoxPtrList;

    QString m_defaultLabelStyle;
    bool m_maxButtonsWarning;

    int m_shiftLatchCount;
    int m_shiftDataCount;

    void signalsForWidgets(int currentDeviceEnum, int previousDeviceEnum, int pinNumber);
    void pinTypeLimit(int currentDeviceEnum, int previousDeviceEnum);
    void setCurrentConfig(int currentDeviceEnum, int previousDeviceEnum, int pinNumber);

    int m_axisSources;
    int m_buttonsFromAxes;
    int m_buttonsFromShiftRegs;
    int m_singleButtons;
    int m_rowsOfButtons;
    int m_columnsOfButtons;
    int m_singleLed;
    int m_rowsOfLed;
    int m_columnsOfLed;

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

    const source_t m_source[SOURCE_COUNT] = {
        {AXIS_SOURCE,
         {AXIS_ANALOG, TLE5011_CS, MCP3201_CS, MCP3202_CS, MCP3204_CS, MCP3208_CS, MLX90393_CS}},

        {BUTTON_FROM_AXES, {678}}, // 678 в DeviceConfig

        {SINGLE_BUTTON, {BUTTON_VCC, BUTTON_GND}},
        {ROW_OF_BUTTONS, {BUTTON_ROW}},
        {COLUMN_OF_BUTTONS, {BUTTON_COLUMN}},

        {SINGLE_LED, {LED_SINGLE}},
        {ROW_OF_LED, {LED_ROW}},
        {COLUMN_OF_LED, {LED_COLUMN}},
    };

    const pinTypeLimit_t m_pinTypeLimit[PIN_TYPE_LIMIT_COUNT] = // static ?
        {
            {SHIFT_REG_LATCH, 4},
            {SHIFT_REG_DATA, 4},
        };

    enum {
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
