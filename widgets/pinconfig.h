#ifndef PINCONFIG_H
#define PINCONFIG_H

#include <QWidget>

#include "pincombobox.h"

namespace Ui {
class PinConfig;
}

class PinConfig : public QWidget
{
    Q_OBJECT

public:
    explicit PinConfig(QWidget *parent = nullptr);
    ~PinConfig();
    void WriteToConfig();
    void ReadFromConfig();

    // make valueChanged for singleButtons_count and etc

    //private:?
    //! 0 if no pins
    uint pin_count_ = 0;

    //! PinComboBox widget list
    QList<PinComboBox *> PinComboBoxPtrList;

private slots:
    void setCurrentConfig();

private:
    Ui::PinConfig *ui;
};

#endif // PINCONFIG_H
