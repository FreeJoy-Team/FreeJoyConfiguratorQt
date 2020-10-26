#include "centered_cbox.h"

#include <QComboBox>
#include <QStylePainter>
#include <QItemDelegate>
#include <QFontMetricsF>

CenteredCBox::CenteredCBox(QWidget *parent) : QComboBox(parent)
{
    arrow_width = 0;
}

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
      arrow_width = rect.width();
   }

   QFontMetricsF font_metric(property("font").value<QFont>());
   option.rect.setLeft(option.rect.center().x() - arrow_width/2.5 - font_metric.width(option.currentText)/2);
   painter.drawControl(QStyle::CE_ComboBoxLabel, option);
}
