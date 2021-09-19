#include "infolabel.h"

InfoLabel::InfoLabel(QWidget *parent)
    : QLabel(parent)
{
    setMaximumSize(12, 12);
    setScaledContents(true);
    setPixmap(QPixmap(":/Images/info_icon.png"));
}
