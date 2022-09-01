#ifndef CURRENTCONFIG_H
#define CURRENTCONFIG_H

#include <QWidget>

namespace Ui {
class CurrentConfig;
}

class CurrentConfig : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentConfig(QWidget *parent = nullptr);
    ~CurrentConfig();

    void setConfig(int type, int changedHowMuch);
    bool limitIsReached();

signals:
    void totalLEDsValueChanged(int totalLed);
    void totalButtonsValueChanged(int count);
    void limitReached(bool limit);

public slots:
    void a2bCountChanged(int count);
    void shiftRegButtonsCountChanged(int count);
    void totalButtonsChanged(int count);
    void totalLEDsChanged(int count);

private:
    Ui::CurrentConfig *ui;

    QString m_defaultLabelStyle;
    bool m_maxButtonsWarning;
    int m_axisSources;
    int m_buttonsFromAxes;
    int m_buttonsFromShiftRegs;
    int m_singleButtons;
    int m_rowsOfButtons;
    int m_columnsOfButtons;
    int m_singleLed;
    int m_rowsOfLed;
    int m_columnsOfLed;

    bool m_limit;

    enum
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

#endif // CURRENTCONFIG_H
