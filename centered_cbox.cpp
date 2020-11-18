#include "centered_cbox.h"

#include <QComboBox>
#include <QStylePainter>
#include <QItemDelegate>
#include <QFontMetricsF>

CenteredCBox::CenteredCBox(QWidget *parent) : QComboBox(parent)
{
    arrow_width_ = 0;
}

// setting text approximately centered
void CenteredCBox::paintEvent (QPaintEvent * event)
{
    Q_UNUSED(event)

   QStylePainter painter(this);
   painter.setPen(palette().color(QPalette::Text));

   QStyleOptionComboBox option;
   initStyleOption(&option);
   painter.drawComplexControl(QStyle::CC_ComboBox, option);

   option.direction = Qt::LeftToRight;

   if( style() )
   {
      QRect rect = style()->subControlRect(QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxArrow, this);
      arrow_width_ = rect.width();
   }

   QFontMetricsF font_metric(property("font").value<QFont>());
   // если сделать по центру всего комбобокса, то будет ощущение, что слева больше свободного места
   // из-за стрелки справа. Если по центру не считая стрелки, то тоже выглядит криво
   // arrow_width_/2.7f тут я сделал промежуточный вариант
   option.rect.setLeft(option.rect.center().x() - arrow_width_/2.7f - font_metric.width(option.currentText)/2);
   painter.drawControl(QStyle::CE_ComboBoxLabel, option);
}
