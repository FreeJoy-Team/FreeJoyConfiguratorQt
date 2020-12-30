#include "centered_cbox.h"

#include <QComboBox>
#include <QFontMetricsF>
#include <QItemDelegate>
#include <QStylePainter>

CenteredCBox::CenteredCBox(QWidget *parent)
    : QComboBox(parent)
{
    m_arrowWidth = 0;
}

// setting text approximately centered
void CenteredCBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    QStyleOptionComboBox option;
    initStyleOption(&option);
    painter.drawComplexControl(QStyle::CC_ComboBox, option);

    option.direction = Qt::LeftToRight;

    if (style()) {
        QRect rect = style()->subControlRect(QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxArrow, this);
        m_arrowWidth = rect.width();
    }

    QFontMetricsF font_metric(property("font").value<QFont>());
    // походу width измеряется по центрам букв и она короче, чем есть на самом
    // деле. умножаю на 1.1
    qreal font_width = font_metric.horizontalAdvance(option.currentText);// * 1.1f;
    font_metric.averageCharWidth();

    // если сделать по центру всего комбобокса, то будет ощущение, что слева
    // больше свободного места из-за стрелки справа. Если по центру не считая
    // стрелки, то тоже выглядит криво arrow_width_/3 тут я сделал промежуточный
    // вариант
    int offset = option.rect.center().x() - m_arrowWidth / 3.2f - font_width / 2;

    // если длина комбобокса меньше длины текста, сдвиг слева уменьшается
    if (offset + font_width*1.1f > option.rect.right() - m_arrowWidth) {
        offset -= ((offset + font_width*1.1f) - (option.rect.right() - m_arrowWidth));
        if (offset < 0)
            offset = 0;
    }
    option.rect.setLeft(offset);

    painter.drawControl(QStyle::CE_ComboBoxLabel, option);
}
