#include "altspinbox.h"

// set the value of QSpinBox to max when value above max is entered
// author https://stackoverflow.com/a/48481568

void AltSpinBox::fixup(QString &input) const
{
    QString textVal(input.mid(prefix().size(), input.size() - prefix().size() - suffix().size()));
    auto isOk = false;
    auto value = textVal.toInt(&isOk, displayIntegerBase());
    if (isOk) {
        value = qBound(minimum(), value, maximum());
        input = QString::number(value, displayIntegerBase());
    } else {
        QSpinBox::fixup(input);
    }
}

QValidator::State AltSpinBox::validate(QString &text, int &pos) const
{
    QString textVal(text.mid(prefix().size(), text.size() - prefix().size() - suffix().size()));
    auto isOk = false;
    auto value = textVal.toInt(&isOk, displayIntegerBase());
    if (isOk) {
        if (value >= minimum() && value <= maximum())
            return QValidator::Acceptable;
        return QValidator::Intermediate;
    } else {
        return QSpinBox::validate(text, pos);
    }
}

