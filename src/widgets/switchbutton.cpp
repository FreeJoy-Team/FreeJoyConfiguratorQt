#include "switchbutton.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOption>

//QPoint poin2[50];
SwitchButton::SwitchButton(QWidget *parent)
    : QWidget(parent)
    , m_currentState(false)
    , m_mousePressed(false)
{
    changeColor(m_currentState);
}


bool SwitchButton::isChecked() const
{
    return m_currentState;
}

void SwitchButton::setChecked(bool checked)
{
    if (m_currentState == checked) return;

    m_currentState = checked;
    changeColor(m_currentState);
    update();
    emit stateChanged(m_currentState);
}


void SwitchButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform,true);
    //p.setRenderHint(QPainter::LosslessImageRendering,true);

    int height = this->height();
    int width = this->width();
    int offset = 5;

    QPointF point;
    if (m_currentState == false) {
        // first
        point = QPointF(m_halfHeight, m_halfHeight);
        p.setBrush(m_kSunBackground);
    } else {
        // last
        point = QPointF(width - m_halfHeight, m_halfHeight);
        p.setBrush(m_kMoonBackground);
//        p.setPen(QColor(248, 227, 161));
//        for (int i = 0; i < 10; ++i) {
//            p.drawLine(poin2[i], QPoint(poin2[i].x() + 3, poin2[i].y() -3));
//            p.drawLine(poin2[i].x() + 3, poin2[i].y()-1, poin2[i].x(), poin2[i].y() -3);
//        }
    }

    p.setPen(QColor(90, 90, 90));
    QRectF rect(3, 4, width-6, height-8);
    p.drawRoundedRect(rect, 10.0, 60.0);

    p.setPen(QColor(110, 64, 201));
    p.drawEllipse(point, m_halfHeight -1.0f, m_halfHeight -1.0f);


    QRectF target(offset, offset,
                  height - offset *2, height - offset *2);
    QRectF source(0.0, 0.0, m_sunPix.size().width(), m_sunPix.size().height());
    p.drawPixmap(target, m_sunPix, source);

    QRectF target3(width - height +offset, offset,
                   height - offset *2, height - offset *2);
    QRectF source3(0.0, 0.0, m_moonPix.size().width(), m_moonPix.size().height());
    p.drawPixmap(target3, m_moonPix, source3);
}

void SwitchButton::changeColor(bool checked)
{
    if (checked == false) {
        m_sunPix = changePixmapColor(m_sunPix, m_kSunLightMode);
        m_moonPix = changePixmapColor(m_moonPix, m_kMoonLightMode);
    } else {
        m_sunPix = changePixmapColor(m_sunPix, m_kSunDarkMode);
        m_moonPix = changePixmapColor(m_moonPix, m_kMoonDarkMode);
    }
}

QPixmap SwitchButton::changePixmapColor(const QPixmap &pixmap, const QColor &color)
{
    QPixmap pix = pixmap;
    QPainter painter(&pix);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    // set color
    painter.setBrush(color);
    painter.setPen(color);
    // paint rect
    painter.drawRect(pix.rect());
    return pix;
}

void SwitchButton::mousePressEvent(QMouseEvent *event)
{
   if (event->button() == Qt::LeftButton) {
       m_mousePressed = true;
   }
}

void SwitchButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_mousePressed) {
            setChecked(!m_currentState);
            m_mousePressed = false;
        }
    }
}

void SwitchButton::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    m_halfWidth = width() / 2.0f;
    m_halfHeight = height() / 2.0f;
//    for (int i = 0; i < 50; ++i) {
//        poin2[i].setX(rand() % (width() - 2));
//        poin2[i].setY(rand() % height());
//    }
}
