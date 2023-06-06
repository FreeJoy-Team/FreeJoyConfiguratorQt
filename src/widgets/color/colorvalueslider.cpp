#include "colorvalueslider.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QStyleOptionFrame>
#include <QPainter>

ColorValueSlider::ColorValueSlider(QWidget *parent)
    : ColorValueSlider{Qt::Horizontal, parent}
{
}

ColorValueSlider::ColorValueSlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider{orientation, parent}
    , m_currentColor(QColor(Qt::white))
    , m_firstColor(QColor(Qt::white))
    , m_mouseInside(false)
{
    setTickPosition(NoTicks);
    setMaximum(255);
    setValue(255);
    setMinimum(0);
    m_gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
    m_gradient.setSpread(QGradient::RepeatSpread);
    m_gradient.setColorAt(0, QColor(Qt::black));
    m_gradient.setColorAt(1, m_firstColor);
}


void ColorValueSlider::setCurrentColor(const QColor &color)
{
    if (color == m_currentColor) return;

    m_currentColor = color;
    setValue(m_currentColor.value());
    emit currentColorChanged(m_currentColor);
    setFirstColor(color);
}

QColor ColorValueSlider::currentColor() const
{
    return m_currentColor;
}

void ColorValueSlider::setFirstColor(const QColor &color) // value always 255
{
    QColor firstCol(QColor::fromHsv(color.hue(), color.saturation(), 255));
    if (firstCol == m_firstColor) return;

    m_firstColor = firstCol;
    m_gradient.setColorAt(0, QColor(Qt::black));
    m_gradient.setColorAt(1, m_firstColor);
    update();
    emit firstColorChanged(m_firstColor);

//    setCurrentColor(color);
    int val = value();
    if (val < 0) val = 0;
    else if (val > 255) val = 255;
    m_currentColor = QColor::fromHsv(color.hue(), color.saturation(), val);
    emit currentColorChanged(m_currentColor);
}

QColor ColorValueSlider::firstColor() const
{
    return m_firstColor;
}


void ColorValueSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    QStyleOptionFrame panel;
    panel.initFrom(this);
    panel.lineWidth = 1;
    panel.midLineWidth = 0;
    panel.state |= QStyle::State_Sunken;
    style()->drawPrimitive(QStyle::PE_Frame, &panel, &painter, this);
    QRect r = style()->subElementRect(QStyle::SE_FrameContents, &panel, this);
    painter.setClipRect(r);

    int gradientDirection = invertedAppearance() ? -1 : 1;

    QLinearGradient grad(m_gradient);
    // widget disabled
    if (isEnabled() == false) {
        grad.setColorAt(1, QColor(Qt::gray));
    }

    if(orientation() == Qt::Horizontal) {
        grad.setFinalStop(gradientDirection, 0);
    } else {
        grad.setFinalStop(0, -gradientDirection);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawRect(1,1,geometry().width()-2,geometry().height()-2);
    // draw handle // excess functionality
    qreal pos = (maximum() != 0) ?
                    static_cast<qreal>(value() - minimum()) / maximum() : 0;
    QColor color;
    auto stops = grad.stops();
    int i;
    for (i = 0; i < stops.size(); i++) {
        if (stops[i].first > pos)
            break;
    }
    if (i == 0) {
        color = stops.front().second;
    } if (i == stops.size()) {
        color = stops.back().second;
    } else {
        auto &a = stops[i - 1];
        auto &b = stops[i];
        auto c = (b.first - a.first);
        qreal q = (c != 0) ?
                      (pos - a.first) / c : 0;
        color = QColor::fromRgbF(b.second.redF() * q + a.second.redF() * (1.0 - q),
                                 b.second.greenF() * q + a.second.greenF() * (1.0 - q),
                                 b.second.blueF() * q + a.second.blueF() * (1.0 - q),
                                 b.second.alphaF() * q + a.second.alphaF() * (1.0 - q));
    }
    if (color.valueF() > 0.5f || color.alphaF() < 0.5f) {
        painter.setPen(QPen(Qt::black, 3));
    } else {
        painter.setPen(QPen(Qt::white, 3));
    }
    // calc handle position
    QPointF p1;
    QPointF p2;
    if (orientation() == Qt::Horizontal) {
        pos = pos * (geometry().width() - 5);
        p1 = QPointF(pos  + 2.5f, 0);
        p2 = p1 + QPointF(0, geometry().height() - 5);
    } else {
        pos = (1.0 - pos) * (geometry().height() - 5);
        p1 = QPointF(0, pos + 2.5f);
        p2 = p1 + QPointF(geometry().width(), 0);
    }

    QRectF handle = QRectF(p1, p2);
    if (!m_mouseInside) {
        handle.adjust(0, 1, 0, -1);
    }
    painter.drawRect(handle);
}

void ColorValueSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
        setSliderDown(true);

        double pos;  // дублируется !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if (orientation() == Qt::Horizontal) {
            if (geometry().width() > 5) {
                pos = static_cast<double>(event->pos().x() - 2.5) / (geometry().width() - 5);
            } else {
                pos = 0;
            }
            pos = qMax(qMin(pos, 1.0), 0.0);
            setSliderPosition(qRound(minimum() + pos * (maximum() - minimum())));
        } else {
            if (geometry().height() > 5) {
                pos = static_cast<double>(event->pos().y() - 2.5) / (geometry().height() - 5);
            } else {
                pos = 0;
            }
            pos = qMax(qMin(pos, 1.0), 0.0);
            setSliderPosition(maximum() - pos * (maximum() - minimum()));
        }

        update();
    } else {
        QSlider::mousePressEvent(event);
    }
}

void ColorValueSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
        setSliderDown(false);
        update();
    } else {
        QSlider::mousePressEvent(event);
    }
}

void ColorValueSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        event->accept();

        double pos;  // дублируется !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if (orientation() == Qt::Horizontal) {
            if (geometry().width() > 5) {
                pos = static_cast<double>(event->pos().x() - 2.5) / (geometry().width() - 5);
            } else {
                pos = 0;
            }
            pos = qMax(qMin(pos, 1.0), 0.0);
            setSliderPosition(qRound(minimum() + pos * (maximum() - minimum())));
        } else {
            if (geometry().height() > 5) {
                pos = static_cast<double>(event->pos().y() - 2.5) / (geometry().height() - 5);
            } else {
                pos = 0;
            }
            pos = qMax(qMin(pos, 1.0), 0.0);
            setSliderPosition(maximum() - pos * (maximum() - minimum()));
        }

        update();
    } else {
        QSlider::mouseMoveEvent(event);
    }
}

void ColorValueSlider::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    //QSlider::leaveEvent(event);
    m_mouseInside = false;
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void ColorValueSlider::enterEvent(QEvent *event)
#else
void ColorValueSlider::enterEvent(QEnterEvent *event)
#endif
{
    Q_UNUSED(event)
    //QSlider::enterEvent(event);
    m_mouseInside = true;
}

void ColorValueSlider::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        //scrollWithPixels(numPixels);  // for mac, not finished
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        setValue(value() + numSteps.y());
    }
    event->accept();
}
