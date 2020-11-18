#ifndef ENCODERS_H
#define ENCODERS_H

#include <QWidget>

#include "global.h"
#include "deviceconfig.h"

#define ENCODER_TYPE_COUNT 3
namespace Ui {
class Encoders;
}

class Encoders : public QWidget
{
    Q_OBJECT

public:
    explicit Encoders(int encoders_number, QWidget *parent = nullptr);
    ~Encoders();

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

    int GetInputA() const;
    int GetInputB() const;

    void SetInputA(int);
    void SetInputB(int);

private:
    Ui::Encoders *ui;
    void SetUiOnOff();

    int encoders_number_;
    int input_A_;
    int input_B_;
    QString not_defined_;

    const deviceEnum_guiName_t encoder_type_list_[ENCODER_TYPE_COUNT] =      // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {
        {ENCODER_CONF_1x,        tr("Encoder 1x")},
        {ENCODER_CONF_2x,        tr("Encoder 2x")},
        {ENCODER_CONF_4x,        tr("Encoder 4x")},
    };

};

#endif // ENCODERS_H
