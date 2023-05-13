#include "axescurvesplot.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QTimer>
#include <QStyleOption>
#include <cmath>

#include <QDebug>

AxesCurvesPlot::AxesCurvesPlot(bool labelEnabled ,QWidget *parent) :
    QWidget(parent)
{
    Q_ASSERT(m_kPointsCount >= 2);
    setMouseTracking(true);

    m_height = 0;
    m_isDeviceConnect = false;
    m_curAxisPos.color = m_kPointCurrentPosColor;

    int range, interval;
    if ((m_kMinPointValue < 0 && m_kMaxPointValue < 0) || (m_kMinPointValue >= 0 && m_kMaxPointValue >= 0)) {
        range = m_kMaxPointValue - m_kMinPointValue;
    }
    else {
        range = abs(m_kMinPointValue) + abs(m_kMaxPointValue);
    }
    interval = range / (m_kPointsCount - 1);

    for (int i = 0; i < m_kPointsCount; ++i) {
        AxesCurve_point* point = new AxesCurve_point;
        point->color = m_kPointInactiveColor;
        point->posX = 0;
        point->posY = 0;
        point->is_drag = false;
        point->current_value = m_kMinPointValue + interval * i;
        m_pointPtrList.append(point);
    }
    if (labelEnabled) {
        for (int i = 0; i < m_kPointsCount; ++i) {
            QLabel* label = new QLabel(this);
            label->setFont(QFont("MS Shell Dlg 2", 8));
            label->setNum(m_pointPtrList[i]->current_value);
            label->setMinimumWidth(m_kLabelWidth);
            label->setAlignment(Qt::AlignHCenter);
            //label->setStyleSheet("background:transparent;");
            label->setVisible(true);
            m_labelPtrList.append(label);
        }
    }
}

void AxesCurvesPlot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    float tmp_x, tmp_y;
    int halfRadius = m_radius/2;

    // stylesheet
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // grid
    if (m_gridEnabled) {
        // dark style
        if (window()->palette().color(QWidget::backgroundRole()).value() < 80) {
            painter.setPen(QColor(100,100,100));
        } else {
            painter.setPen(Qt::lightGray);
        }

        // columns //330 67 - time of 1000 iteraction
        for (int i = 0; i < m_kColumnsCount + 1; ++i) {
            tmp_x = (i * m_columnWidth) + m_offset;
            painter.drawLine(tmp_x, m_offset, tmp_x, m_height - m_offset);
        }
        // rows
        for (int i = 0; i < m_kRowsCount + 1; ++i) {
            tmp_y = (i * m_rowHeight) + m_offset;
            painter.drawLine(m_offset, tmp_y, m_width - m_offset, tmp_y);
        }
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    // line
    QPen pen;
    pen.setWidth(m_lineWidth);
    pen.setColor(m_kPointInactiveColor);
    painter.setPen(pen);
    for (int i = 0; i < m_pointPtrList.size() -1; ++i) {
        QPointF p1(m_pointPtrList[i]->posX, m_pointPtrList[i]->posY);
        QPointF p2(m_pointPtrList[i + 1]->posX, m_pointPtrList[i + 1]->posY);
        painter.drawLine(p1, p2);
//        painter.drawLine(m_pointPtrList[i]->posX, m_pointPtrList[i]->posY,
//                         m_pointPtrList[i + 1]->posX, m_pointPtrList[i + 1]->posY);
    }

    // coordinates for ellipse mousOver //250 78    // can be optimized
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        m_pointPtrList[i]->area.setRect(m_pointPtrList[i]->posX - m_radius, m_pointPtrList[i]->posY - m_radius,
                                      m_radius*2, m_radius*2);
    }
    // handle ellipse
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        // ellipse border
        QPoint center(m_pointPtrList[i]->posX, m_pointPtrList[i]->posY);
        painter.setPen(m_pointPtrList[i]->color);
        painter.setBrush(m_pointPtrList[i]->color);
        painter.drawEllipse(center, m_radius, m_radius);
        // ellipse center
        painter.setPen(m_kPointMoveColor);
        painter.setBrush(m_kPointMoveColor);
        painter.drawEllipse(center, halfRadius, halfRadius);
    }

    // current axix position
    if (m_isDeviceConnect == true) {
        painter.setPen(m_curAxisPos.color);
        painter.setBrush(m_curAxisPos.color);
        painter.drawEllipse(QPoint(m_curAxisPos.posX, m_curAxisPos.posY),
                            m_currentPosRadius, m_currentPosRadius);
    }
}

void AxesCurvesPlot::setBorderOffset(int offset)
{
    m_offset = offset;
}

void AxesCurvesPlot::setPointRadius(int radius)
{
    m_radius = radius;
}

void AxesCurvesPlot::setLineWidth(int width)
{
    m_lineWidth = width;
}

void AxesCurvesPlot::setCurAxisPos(int posX, int posY)
{
    m_curAxisPos.posX = calcPointPosX(posX);   // увеличить Х scale
    m_curAxisPos.posY = calcPointPosY(posY);
    update();
}

void AxesCurvesPlot::setGridEnabled(bool enabled)
{
    m_gridEnabled = enabled;
}

int AxesCurvesPlot::lineWidth() const
{
    return m_lineWidth;
}

bool AxesCurvesPlot::gridEnabled() const
{
    return m_gridEnabled;
}

int AxesCurvesPlot::pointCount() const
{
    return m_kPointsCount;
}


int AxesCurvesPlot::pointValue(int pointIndex) const
{
    if (pointIndex > m_kPointsCount) {
        pointIndex = m_kPointsCount;
    } else if (pointIndex < 0) {
        pointIndex = 0;
    }
    return m_pointPtrList[pointIndex]->current_value;
}

QVector<int> AxesCurvesPlot::pointValues() const
{
    QVector<int> vec(m_kPointsCount);
    for (int i = 0; i < m_kPointsCount; ++i) {
        vec[i] = m_pointPtrList[i]->current_value;
    }
    return vec;
}

void AxesCurvesPlot::setPointValues(const QVector<int> &values)
{
    for (int i = 0; i < values.size(); ++i) {
        setPointValue(i, values[i]);
    }
}

void AxesCurvesPlot::setPointValue(int pointIndex, int value)
{
    if (pointIndex > m_kPointsCount) {
        pointIndex = m_kPointsCount;
    } else if (pointIndex < 0) {
        pointIndex = 0;
    } else if (m_pointPtrList[pointIndex]->current_value == value) {
        return;
    } else if (value > m_kMaxPointValue) {
        value = m_kMaxPointValue;
    } else if (value < m_kMinPointValue) {
        value = m_kMinPointValue;
    }

    m_pointPtrList[pointIndex]->posY = calcPointPosY(value);
    m_pointPtrList[pointIndex]->current_value = value;
    if (m_labelPtrList.isEmpty() == false) {
        m_labelPtrList[pointIndex]->setNum(value);
        updateLabelPos();
    }
    update();
    emit pointValueChanged(pointIndex, value);
}

int AxesCurvesPlot::maximum() const
{
    return m_kMaxPointValue;
}

int AxesCurvesPlot::minimum() const
{
    return m_kMinPointValue;
}


void AxesCurvesPlot::updateLabelPos()
{
    if (m_labelPtrList.isEmpty() == false) {
        for (int i = 0; i < m_labelPtrList.size(); ++i) {
            m_labelPtrList[i]->move(m_pointPtrList[i]->posX - m_radius, m_pointPtrList[i]->posY + m_radius);
        }
    }
}

int AxesCurvesPlot::calcPointValue(int currentPos) const
{
    int value = 0;
    float half_height = m_height / 2.0f - m_offset;
    currentPos -= m_offset;

    if (currentPos > half_height){
        float tmp_min = (half_height)/ m_kMinPointValue;
        value = roundf((currentPos - half_height) / float(tmp_min));//floorf
        return value;
    } else {
        float tmp_max = (half_height)/ m_kMaxPointValue;
        value = -roundf((currentPos - half_height) / float(tmp_max));
        return value;
    }
    //return value;
}

float AxesCurvesPlot::calcPointPosY(int value) const
{
    float pos = 0.0f;
    float half_height = m_height / 2.0f - m_offset;

    if (value < m_kMaxPointValue - m_kMinPointValue){
        float tmp_min = half_height / m_kMinPointValue;
        pos = roundf(value * tmp_min + half_height);
    } else {
        float tmp_max = half_height / m_kMaxPointValue;
        pos = roundf(value * tmp_max + half_height);
    }
    return pos + m_offset;
}

float AxesCurvesPlot::calcPointPosX(int value) const
{
    int pos = 0;
    float half_width = m_width / 2.0f - m_offset;

    float tmp_max = half_width / 100.0f;
    pos = roundf(value * tmp_max);

    return pos + m_offset;
}


void AxesCurvesPlot::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)

    m_width = width() -1;   // -1 ?????????????????????????????????
    m_height = height() -1; // -1 - bottom/right offset for painting

    m_columnWidth = (m_width - m_offset*2) / float(m_kColumnsCount);
    m_rowHeight = (m_height - m_offset*2) / float(m_kRowsCount);

    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        m_pointPtrList[i]->posX = i * m_columnWidth + m_offset;
        m_pointPtrList[i]->posY = calcPointPosY(m_pointPtrList[i]->current_value);
    }
    updateLabelPos();
}

void AxesCurvesPlot::mouseMoveEvent(QMouseEvent *event)
{
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        if (m_pointPtrList[i]->is_drag == false) {
            if (m_pointPtrList[i]->area.contains(event->pos())) {
                m_pointPtrList[i]->color = m_kPointActiveColor;
                break;
            } else {
                m_pointPtrList[i]->color = m_kPointInactiveColor;
            }
        } else if (m_pointPtrList[i]->is_drag == true) {
            if (event->pos().y() < m_offset || event->pos().y() >= height() - m_offset) {
                return;
            }
            if (event->buttons() & Qt::LeftButton) {
                m_pointPtrList[i]->posY = event->pos().y();
                m_pointPtrList[i]->color = m_kPointMoveColor;
                m_pointPtrList[i]->current_value = calcPointValue(m_pointPtrList[i]->posY);

                if (m_labelPtrList.isEmpty() == false) {
                    m_labelPtrList[i]->setNum(m_pointPtrList[i]->current_value);
                }
                updateLabelPos();
                emit pointValueChanged(i, m_pointPtrList[i]->current_value);
            }
            break;
        }
    }
    update();
}

void AxesCurvesPlot::mousePressEvent(QMouseEvent *event)
{
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        if (m_pointPtrList[i]->area.contains(event->pos())) {
            m_pointPtrList[i]->is_drag = true;
            break;
        }
    }
}

void AxesCurvesPlot::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    for (int i = 0; i < m_pointPtrList.size(); ++i){
        if(m_pointPtrList[i]->is_drag == true){
            m_pointPtrList[i]->is_drag = false;
            m_pointPtrList[i]->color = m_kPointInactiveColor;
            break;
        }
    }
    update();
}

void AxesCurvesPlot::deviceStatus(bool isConnect)
{
    m_isDeviceConnect = isConnect;
    update();
}

