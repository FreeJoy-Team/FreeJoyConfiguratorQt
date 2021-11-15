#include "infolabel.h"

InfoLabel::InfoLabel(QWidget *parent)
    : QLabel(parent)
{
    setMaximumSize(12, 12);
    setScaledContents(false);
    setPixmap(QPixmap(":/Images/info_icon.png"));
}

void InfoLabel::setPixmap(const QPixmap &p)
{
    pix = p;
    QLabel::setPixmap(scaledPixmap());
}

int InfoLabel::heightForWidth(int width) const
{
    return pix.isNull() ? height() : ((qreal)pix.height()*width)/pix.width();
}

QSize InfoLabel::sizeHint() const
{
    int w = width();
    return QSize(w, heightForWidth(w));
}

QPixmap InfoLabel::scaledPixmap() const
{
    return pix.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void InfoLabel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    if(!pix.isNull()) {
        QLabel::setPixmap(scaledPixmap());
    }
}
