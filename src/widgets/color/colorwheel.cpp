#include "colorwheel.h"
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionFrame>
#include <QtMath>

ColorWheel::ColorWheel(QWidget *parent)
    : QWidget{parent}
    , m_mousePressed(false)
    , m_picker(0, 0, m_pickerSize, m_pickerSize)
    , m_currentColor(QColor(Qt::white))
{
    resize(200, 200);
    setMouseTracking(true);
    colorToPos(m_currentColor);
}

ColorWheel::~ColorWheel()
{}

void ColorWheel::setCurrentColor(const QColor &color)
{
    int h, s, v;
    color.getHsv(&h, &s, &v);

    QColor col(color);
    col = QColor::fromHsv(h, s, 255).toRgb();

    if (col == m_currentColor) return;

    m_currentColor = col;
    m_picker.moveCenter(colorToPos(m_currentColor));
    emit currentColorChanged(m_currentColor);
}

QColor ColorWheel::currentColor() const
{
    return m_currentColor;
}

// draw pixmap only on resize, performance reson
QPixmap ColorWheel::drawColorWheel()
{
    QPixmap pix (rect().size());
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect(m_center.x() - m_radius, m_center.y() - m_radius,
               m_radius * 2, m_radius * 2);

    int pieOffset = 90*16; // 16 = 1 degrees, offset 90 degrees(start from 12 clock) // must be removed
    for (int angle = 0; angle < 360; ++angle) {
        QColor color(QColor::fromHsv(angle, 255, 255));

        QRadialGradient gradient(m_center, m_radius);
        gradient.setColorAt(0, Qt::white);
        gradient.setColorAt(1, color);

        QBrush brush(gradient);
        QPen pen(brush, 1.0);
        painter.setPen(pen);
        painter.setBrush(brush);
        painter.drawPie(rect, angle * 16 + pieOffset, 16);
    }
    m_colorWheelFrame.clear();
    m_colorWheelFrame.addEllipse(m_center, m_radius +1, m_radius +1);
    m_colorWheelFrame.setFillRule(Qt::WindingFill);
    return pix;
}

void ColorWheel::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (isEnabled()) {
        painter.drawPixmap(rect(), m_colorWheel);
    } else {
        // convert to greyscale with transparency // need to save and calc in resize !!!!!!!
        QImage image = m_colorWheel.toImage().convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; y < image.height(); ++y) {
            QRgb *scanLine = (QRgb*)image.scanLine(y);
            for (int x = 0; x < image.width(); ++x) {
                QRgb pixel = *scanLine;
                uint ci = uint(qGray(pixel));
                *scanLine = qRgba(ci, ci, ci, qAlpha(pixel) / 3);
                ++scanLine;
            }
        }
        painter.drawImage(rect(), image);
    }

    QPen pen;
    // draw frame
    pen.setWidth(2);//1
    pen.setBrush(QColor(Qt::gray));
    painter.setPen(pen);
    painter.drawPath(m_colorWheelFrame);
    // draw picker
    pen.setWidth(2);
    pen.setBrush(m_pickerColor);
    painter.setPen(pen);
    painter.setBrush(QBrush());
    painter.drawEllipse(m_picker);
}


QColor ColorWheel::posToColor(const QPoint &pos)
{
    int center_x = pos.x() - m_center.x();
    int center_y = pos.y() - m_center.y();
    float atan = qAtan2(center_x, center_y);
    float angle = qRadiansToDegrees(atan) + 180.0f;
    QColor color(QColor::fromHsvF(angle / 360.0f, 1.0f, 1.0f));

    QLineF line(m_center, pos);
    float ratio = line.length() / m_radius;
    if (ratio > 1.0) ratio = 1.0; // temp
    if (ratio < 0.0) ratio = 0.0; // temp

    float r = (ratio*color.redF() + (1.0f-ratio)*1.0f);
    float g = (ratio*color.greenF() + (1.0f-ratio)*1.0f);
    float b = (ratio*color.blueF() + (1.0f-ratio)*1.0f);

    return QColor::fromRgbF(r, g, b);
}


QPoint ColorWheel::colorToPos(const QColor &color)
{
    float ratio = 0;
    float ratio1 = 1;
    float ratio2 = 1;
    float ratio3 = 1;

    // скорее всего код полное говно
    if (color != QColor(Qt::white) && color != QColor(Qt::black)) {
        QColor col(QColor::fromHsvF(color.toHsv().hueF(), 1.0f, 1.0f)); // color without gradient

        float rr = abs(color.redF() - col.redF());
        float gg = abs(color.greenF() - col.greenF());
        float bb = abs(color.blueF() - col.blueF());
        if ((rr > gg && rr >= bb) || (rr >= gg && rr > bb)) {
                ratio1 = (color.redF() - 1.0f) / (col.redF() - 1.0f);
        }
        if ((gg > rr && gg >= bb) || (gg >= rr && gg > bb)) {
                ratio2 = (color.greenF() - 1.0f) / (col.greenF() - 1.0f);
        }
        if ((bb > rr && bb >= gg) || (bb >= rr && bb > gg)) {
                ratio3 = (color.blueF() - 1.0f) / (col.blueF() - 1.0f);
        }

        if (ratio1 < 0.5f || ratio2 < 0.5f || ratio3 < 0.5f){
            ratio = std::min({ratio1, ratio2, ratio3}, [](float f1, float f2){
                if (f1 == 0) return false;
                else if (f2 == 0) return true;
                else return f1 < f2;
            });
        } else {
            ratio = std::max({ratio1, ratio2, ratio3}, [](float f1, float f2){
                if (f1 == 0) return false;
                else if (f2 == 0) return true;
                else return f1 > f2;
            });
        }
        if (ratio > 1) ratio = 1;
        if (ratio < 0) ratio = 0;
    }

    float lengthFromCenter = m_radius * ratio;
    float angle = color.toHsv().hueF() + 0.25f; // 0.25f offset 90 degrees to 0 degrees =  3clock(for x, y coord) // must be removed
    if (angle > 1.0f) angle -= 1.0f;

    float x = m_center.x() + lengthFromCenter * cos(qDegreesToRadians(-angle * 360.0f));
    float y = m_center.y() + lengthFromCenter * sin(qDegreesToRadians(-angle * 360.0f));

    QPoint ofset(x, y);
    return ofset;
}


void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_colorWheelFrame.contains(event->pos())) {
        m_mousePressed = true;
        m_picker.moveCenter(event->pos());
        update();

        QColor col(posToColor(event->pos()));
        if (col != m_currentColor) {
            m_currentColor = col;
            emit currentColorChanged(m_currentColor);
        }
    }
}

void ColorWheel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mousePressed = false;
    }
}

void ColorWheel::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mousePressed) {
        if (m_colorWheelFrame.contains(event->pos())) {
            m_picker.moveCenter(event->pos());
        } else {
            // limiting picket to elipse radius
            int x_c = m_center.x();
            int y_c = m_center.y();

            int x_pos = event->pos().x();
            int y_pos = event->pos().y();

            float line = sqrt(pow((x_pos - x_c), 2) + pow((y_pos - y_c), 2));

            int X = x_c + (m_radius * (x_pos - x_c)) / line;
            int Y = y_c + (m_radius * (y_pos - y_c)) / line;

            m_picker.moveCenter(QPoint(X, Y));
        }
        update();

        QColor col(posToColor(event->pos()));
        if (col != m_currentColor) {
            m_currentColor = col;
            emit currentColorChanged(m_currentColor);
        }
    }
}

void ColorWheel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    m_center = QPoint(width() / 2.0f, height() / 2.0f); ////////////////// !!!!!!!!!!!!!!!!!!!!!!!!!!!
    m_radius = std::min(width(), height()) / 2.0f - 2*m_margin;
    m_colorWheel = drawColorWheel();
    m_picker.moveCenter(colorToPos(m_currentColor));
    update();
}
