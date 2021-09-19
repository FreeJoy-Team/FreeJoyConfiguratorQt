#ifndef LEDCONFIG_H
#define LEDCONFIG_H

#include "led.h"
#include <QWidget>

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

private:
    Ui::LedConfig *ui;

    QList<LED *> m_ledPtrList;
    int m_currentLedCount;
};

#endif // LEDCONFIG_H
