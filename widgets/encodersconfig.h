#ifndef ENCODERSCONFIG_H
#define ENCODERSCONFIG_H

#include <QWidget>

#include "encoders.h"
#include "global.h"
#include "deviceconfig.h"

namespace Ui {
class EncodersConfig;
}

class EncodersConfig : public QWidget
{
    Q_OBJECT

public:
    explicit EncodersConfig(QWidget *parent = nullptr);
    ~EncodersConfig();

    void ReadFromConfig();
    void WriteToConfig();

public slots:       // private?
    void encoderInputChanged(int encoder_A, int encoder_B);

private:
    Ui::EncodersConfig *ui;
    QList<Encoders*> EncodersAdrList;
    int encoders_input_A_count_ = 0;
    int encoders_input_B_count_ = 0;

    deviceEnum_guiName_t const fastEncoder_type_list_[ENCODER_TYPE_COUNT] =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {
        {ENCODER_CONF_1x,        tr("Encoder 1x")},
        {ENCODER_CONF_2x,        tr("Encoder 2x")},
        {ENCODER_CONF_4x,        tr("Encoder 4x")},
    };
};

#endif // ENCODERSCONFIG_H
