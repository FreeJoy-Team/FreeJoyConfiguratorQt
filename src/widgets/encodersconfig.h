#ifndef ENCODERSCONFIG_H
#define ENCODERSCONFIG_H

#include <QWidget>

#include "deviceconfig.h"
#include "encoders.h"
#include "global.h"

namespace Ui {
class EncodersConfig;
}

class EncodersConfig : public QWidget
{
    Q_OBJECT

public:
    explicit EncodersConfig(QWidget *parent = nullptr);
    ~EncodersConfig();

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

public slots:
    void encoderInputChanged(int encoder_A, int encoder_B);
    void fastEncoderSelected(const QString &pinGuiName, bool isSelected);

private:
    Ui::EncodersConfig *ui;
    void setUiOnOff();

    QList<Encoders *> m_encodersPtrList;
    int m_encodersInput_A_count;
    int m_encodersInput_B_count;
    QString m_notDefined;

    int m_fastEncoderInput_A;
    int m_fastEncoderInput_B;

    const deviceEnum_guiName_t m_fastEncoderTypeList[ENCODER_TYPE_COUNT] = // порядов обязан быть как в common_types.h!!!!!!!!!!!
    {
        {ENCODER_CONF_1x, tr("Encoder 1x")},
        {ENCODER_CONF_2x, tr("Encoder 2x")},
        {ENCODER_CONF_4x, tr("Encoder 4x")},
    };
};

#endif // ENCODERSCONFIG_H
