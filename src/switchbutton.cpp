#include "switchbutton.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOption>
#include <QDebug>

QPoint poin2[50];
SwitchButton::SwitchButton(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    m_currentSwitch = m_oldSwitch = 1;
    changeColor(m_currentSwitch);
    m_pressPos = 0;
    m_mouseOnSwitchNumber = -1;
    m_mousePressed = false;
}


void SwitchButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform,true);
    //p.setRenderHint(QPainter::LosslessImageRendering,true);

    int height = this->height();
    int width = this->width();
    int offset = 5;

    p.setPen(QColor(90, 90, 90));
    p.setBrush(m_backgroundColor);
    QRectF rect(3, 4, width-6, height-8);
    p.drawRoundedRect(rect, 10.0, 60.0);

    QPointF point;
    if (m_currentSwitch == 1) {
        // first
        point = QPointF(m_halfHeight, m_halfHeight);
    } else if (m_currentSwitch == m_kSwitchCount) {
        // last
        point = QPointF(width - m_halfHeight, m_halfHeight);
//        p.setPen(QColor(248, 227, 161));
//        for (int i = 0; i < 10; ++i) {
//            p.drawLine(poin2[i], QPoint(poin2[i].x() + 3, poin2[i].y() -3));
//            p.drawLine(poin2[i].x() + 3, poin2[i].y()-1, poin2[i].x(), poin2[i].y() -3);
//        }
    } else {
        // other
        point = QPointF(width / float(m_currentSwitch), m_halfHeight);
    }
    p.setPen(QColor(110, 64, 201));
    p.drawEllipse(point, m_halfHeight -1.0f, m_halfHeight -1.0f);

    QRectF target(offset, offset,
                  height - offset *2, height - offset *2);
    QRectF source(0.0, 0.0, m_cloudPix.size().width(), m_cloudPix.size().height());
    p.drawPixmap(target, m_cloudPix, source);

    QRectF target2(m_halfWidth - offset *2, offset,
                   height - offset *2, height - offset *2);
    QRectF source2(0.0, 0.0, m_sunPix.size().width(), m_sunPix.size().height());
    p.drawPixmap(target2, m_sunPix, source2);

    QRectF target3(width - height +offset, offset,
                   height - offset *2, height - offset *2);
    QRectF source3(0.0, 0.0, m_moonPix.size().width(), m_moonPix.size().height());
    p.drawPixmap(target3, m_moonPix, source3);
}

void SwitchButton::changeColor(int index)
{
    if (index == 1) {
        m_backgroundColor = QColor(120, 143, 230);
        m_cloudPix = changePixmapColor(m_cloudPix, m_kCloudLightMode);
        m_sunPix = changePixmapColor(m_sunPix, m_kSunLightMode);
        m_moonPix = changePixmapColor(m_moonPix, m_kMoonLightMode);
    } else if (index == 2) {
        m_backgroundColor = QColor(110, 147, 255);
        m_cloudPix = changePixmapColor(m_cloudPix, m_kCloudLightMode);
        m_sunPix = changePixmapColor(m_sunPix, m_kSunLightMode);
        m_moonPix = changePixmapColor(m_moonPix, m_kMoonLightMode);
    } else {
        m_backgroundColor = QColor(39, 51, 69);
        m_cloudPix = changePixmapColor(m_cloudPix, m_kCloudDarkMode);
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


void SwitchButton::mouseMoveEvent(QMouseEvent *event)
{
    //m_pointPtrList[i]->polygon.containsPoint(event->pos(), Qt::WindingFill)
    m_mouseOnSwitchNumber = (event->pos().x() / (width() / m_kSwitchCount)) + 1;
    if (m_mouseOnSwitchNumber > m_kSwitchCount) {
        m_mouseOnSwitchNumber = m_kSwitchCount;
    }
    //qDebug()<<m_mouseOnSwitchNumber;
}

void SwitchButton::mousePressEvent(QMouseEvent *event)
{
   if (event->buttons() & Qt::LeftButton) {
       m_mousePressed = true;
       m_pressPos = event->pos().x();
   }
}

void SwitchButton::mouseReleaseEvent(QMouseEvent *event)
{
//    if (event->buttons() & Qt::LeftButton) { // check
//        m_mousePressed = false;
//    }
    //m_pointPtrList[i]->area.contains(event->pos())
    //QRect()
    if (rect().contains(event->pos())) { // not excellent
        if (m_mousePressed) {
            int index = (m_pressPos / (width() / m_kSwitchCount)) + 1;
            if (index > m_kSwitchCount) {
                index = m_kSwitchCount;
            }
            m_currentSwitch = index;
            update();
        }
    }
    if (m_currentSwitch != m_oldSwitch) {
        changeColor(m_currentSwitch);
        m_oldSwitch = m_currentSwitch;
        emit currentIndexChanged(m_currentSwitch - 1);
    }

    qDebug()<<m_currentSwitch;
    m_mousePressed = false;
}

void SwitchButton::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    m_halfWidth = width() / 2.0f;
    m_halfHeight = height() / 2.0f;
//    for (int i = 0; i < 50; ++i) {
//        poin2[i].setX(rand() % (width() - 8));
//        poin2[i].setY(rand() % height());
//    }
}
