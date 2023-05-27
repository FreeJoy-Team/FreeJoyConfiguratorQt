#ifndef ALTSPINBOX_H
#define ALTSPINBOX_H

#include <QSpinBox>

// set the value of QSpinBox to max when value above max is entered
// author https://stackoverflow.com/a/48481568

class AltSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    using QSpinBox::QSpinBox;

protected:
    void fixup(QString &input) const;
    QValidator::State validate(QString &text, int &pos) const;
};

#endif // ALTSPINBOX_H
