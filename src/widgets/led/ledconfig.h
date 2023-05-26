#ifndef LEDCONFIG_H
#define LEDCONFIG_H

#include <QWidget>
#include "global.h"

class LED;
class LedRGBConfig;

namespace Ui {
class LedConfig;
}

class LedConfig : public QWidget
{
    Q_OBJECT

public:
    explicit LedConfig(QWidget *parent = nullptr);
    ~LedConfig();

    void setLedsState();

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

    //void ButtonLEDStateChanged();     // future

public slots:
    void spawnLeds(int ledCount);
    void ledPwmSelected(Pin pin, bool selected);
    void ledRgbSelected(Pin pin, bool selected);

private:
    Ui::LedConfig *ui;

    LedRGBConfig *m_ledsRgb;

    QList<LED *> m_ledPtrList;
    int m_currentLedCount;

    void setEnabledTimers(bool enabled);
};

#endif // LEDCONFIG_H
