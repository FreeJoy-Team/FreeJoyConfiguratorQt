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
    setMouseTracking(true);

    m_height = 0;
    m_isDeviceConnect = false;
    m_curAxisPos.color = m_kPointCurrentPosColor;

    int tmp_range;
    if ((m_kMinPointValue < 0 && m_kMaxPointValue < 0) || (m_kMinPointValue >= 0 && m_kMaxPointValue >= 0)) {
        tmp_range = m_kMaxPointValue - m_kMinPointValue;
    }
    else {
        tmp_range = abs(m_kMinPointValue) + abs(m_kMaxPointValue);
    }

    for (int i = 0; i < m_kPointsCount; ++i) {
        AxesCurve_point* point = new AxesCurve_point;
        m_pointPtrList.append(point);
        m_pointPtrList[i]->color = m_kPointInactiveColor;
        m_pointPtrList[i]->posX = 0;
        m_pointPtrList[i]->posY = 0;
        m_pointPtrList[i]->is_drag = false;
        m_pointPtrList[i]->current_value = ((tmp_range / (m_kPointsCount - 1.0)) * i) + m_kMinPointValue;
    }
    if (labelEnabled) {
        for (int i = 0; i < m_kPointsCount; ++i) {
            QLabel* label = new QLabel(this);
            m_labelPtrList.append(label);
            m_labelPtrList[i]->setFont(QFont("MS Shell Dlg 2", 8));
            m_labelPtrList[i]->setNum(m_pointPtrList[i]->current_value);
            m_labelPtrList[i]->setMinimumWidth(m_kLabelWidth);
            m_labelPtrList[i]->setAlignment(Qt::AlignHCenter);
            m_labelPtrList[i]->setStyleSheet("background:transparent;");
            m_labelPtrList[i]->setVisible(true);
        }
    }
}
#include <QStylePainter>
void AxesCurvesPlot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter;
    painter.begin(this);
    int tmp_x, tmp_y;
    int halfRadius = m_radius/2;

    // stylesheet
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // for dark style
    if (QWidget::palette().color(QWidget::backgroundRole()).value() < 80) {
        painter.setPen(QColor(110,110,110));
    } else {
        painter.setPen(Qt::lightGray);
    }

    //paint columns //330 67 - time of 1000 iteraction
    for (int i = 0; i < m_kColumnsCount + 1; ++i) {
        tmp_x = (i * m_columnWidth) + m_offset;
        painter.drawLine(tmp_x, m_offset, tmp_x, m_height - m_offset);
    }
    // paint rows
    for (int i = 0; i < m_kRowsCount + 1; ++i) {
        tmp_y = (i * m_rowHeight) + m_offset;
        painter.drawLine(m_offset, tmp_y, m_width - m_offset, tmp_y);
    }

    // Antialiasing //105 21
    painter.setRenderHint(QPainter::Antialiasing, true);
    // paint line
    QPen pen;
    pen.setWidth(m_lineWidth);
    pen.setColor(m_kPointInactiveColor);
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        painter.setPen(pen);
        if (i < m_pointPtrList.size() - 1) {
            painter.drawLine(m_pointPtrList[i]->posX, m_pointPtrList[i]->posY,
                             m_pointPtrList[i + 1]->posX, m_pointPtrList[i + 1]->posY);
        }
    }

    // coordinates for rect //250 78    // can be optimized
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        m_pointPtrList[i]->area.setRect(m_pointPtrList[i]->posX - m_radius, m_pointPtrList[i]->posY - m_radius,
                                      m_radius*2, m_radius*2);
    }
    // paint rect
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        // paint ellipse border
        QPoint center(m_pointPtrList[i]->posX, m_pointPtrList[i]->posY);
        painter.setPen(m_pointPtrList[i]->color);
        painter.setBrush(m_pointPtrList[i]->color);
        painter.drawEllipse(center, m_radius, m_radius);
        // paint ellipse center
        painter.setPen(m_kPointMoveColor);
        painter.setBrush(m_kPointMoveColor);
        painter.drawEllipse(center, halfRadius, halfRadius);
    }

    // paint current axix position
    if (m_isDeviceConnect == true) {
        // paint rect
        painter.setPen(m_curAxisPos.color);
        painter.setBrush(m_curAxisPos.color);
        painter.drawEllipse(QPoint(m_curAxisPos.posX, m_curAxisPos.posY),
                            m_currentPosRadius, m_currentPosRadius);
    }
    painter.end();
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

int AxesCurvesPlot::pointValue(int point_number)
{
    return m_pointPtrList[point_number]->current_value;
}

int AxesCurvesPlot::pointCount() const
{
    return m_kPointsCount;
}

void AxesCurvesPlot::setPointValue(int pointNumber, int value)
{
    m_pointPtrList[pointNumber]->posY = calcPointPosY(value);
    m_pointPtrList[pointNumber]->current_value = value;
    m_labelPtrList[pointNumber]->setNum(value);
    updateLabelPos();
    update();
}

void AxesCurvesPlot::setLinear()
{
    int tmpY, tmpVal;
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        tmpY = (i * m_rowHeight) + m_offset;
        m_pointPtrList[(m_pointPtrList.size() -1) -i]->posY = tmpY;

        tmpVal = calcPointValue(m_pointPtrList[(m_pointPtrList.size() -1) -i]->posY);
        m_pointPtrList[(m_pointPtrList.size() -1) -i]->current_value = tmpVal;
        m_labelPtrList[(m_pointPtrList.size() -1) -i]->setNum(tmpVal);
    }
    updateLabelPos();
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
    }
    update();
}

void AxesCurvesPlot::setExponent()
{
    int tmp_range;
    if ((m_kMinPointValue < 0 && m_kMaxPointValue < 0) || (m_kMinPointValue >= 0 && m_kMaxPointValue >= 0)) {
        tmp_range = m_kMaxPointValue - m_kMinPointValue;
    }
    else {
        tmp_range = abs(m_kMinPointValue) + abs(m_kMaxPointValue);
    }

    int tmp_value;
    for (int i = 0; i < m_pointPtrList.size(); ++i)
    {
        tmp_value = round(exp(i * log(tmp_range) / (m_kPointsCount - 1)) + m_kMinPointValue);
        if (tmp_value <= m_kMinPointValue +1){
            tmp_value = m_kMinPointValue;
        } else if (tmp_value >= m_kMaxPointValue -1){
            tmp_value = m_kMaxPointValue;
        }
        setPointValue(i, tmp_value);
    }
    updateLabelPos();
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
    }
}

void AxesCurvesPlot::setExponentInvert()
{
    int tmp_range;
    if ((m_kMinPointValue < 0 && m_kMaxPointValue < 0) || (m_kMinPointValue >= 0 && m_kMaxPointValue >= 0)) {
        tmp_range = m_kMaxPointValue - m_kMinPointValue;
    }
    else {
        tmp_range = abs(m_kMinPointValue) + abs(m_kMaxPointValue);
    }

    int tmp_value;
    for (int i = 0; i < m_pointPtrList.size(); ++i)
    {
        tmp_value = round(exp(i * log(tmp_range) / (m_kPointsCount - 1)) + m_kMinPointValue);
        if (tmp_value <= m_kMinPointValue +1){
            tmp_value = m_kMinPointValue;
        } else if (tmp_value >= m_kMaxPointValue -1){
            tmp_value = m_kMaxPointValue;
        }
        setPointValue((m_kPointsCount - 1) - i, tmp_value);
    }
    updateLabelPos();
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
    }
}

void AxesCurvesPlot::setShape()
{
    if (m_pointPtrList.size() >= 11)
    {
        setPointValue(0, -100);
        setPointValue(1, -60);
        setPointValue(2, -20);
        setPointValue(3, -6);
        setPointValue(4, -2);
        setPointValue(5, 0);
        setPointValue(6, 2);
        setPointValue(7, 6);
        setPointValue(8, 20);
        setPointValue(9, 60);
        setPointValue(10, 100);
    }
    updateLabelPos();
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
    }
}


void AxesCurvesPlot::updateLabelPos()
{
    for (int i = 0; i < m_labelPtrList.size(); ++i) {
        m_labelPtrList[i]->move(m_pointPtrList[i]->posX - m_radius, m_pointPtrList[i]->posY + m_radius);
    }
}

int AxesCurvesPlot::calcPointValue(int currentPos) const
{
    int value = 0;
    float half_height = m_height / 2.0f - m_offset;
    currentPos -= m_offset;

    if (currentPos > half_height){
        float tmp_min = (half_height)/ m_kMinPointValue;
        value = floorf((currentPos - half_height) / float(tmp_min));
        return value;
    } else {
        float tmp_max = (half_height)/ m_kMaxPointValue;
        value = -roundf((currentPos - half_height) / float(tmp_max));
        return value;
    }
    return value;
}

int AxesCurvesPlot::calcPointPosY(int value) const
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

int AxesCurvesPlot::calcPointPosX(int value) const
{
    int pos = 0;
    float half_width = m_width / 2.0f - m_offset;

    float tmp_max = half_width / 100.0f;
    pos = round(value * tmp_max);

    return pos + m_offset;
}


void AxesCurvesPlot::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)

    m_width = width();
    m_height = height();

    m_columnWidth = (m_width - m_offset*2) / float(m_kColumnsCount);
    m_rowHeight = (m_height - m_offset*2) / float(m_kRowsCount);

    for (int i = 0; i < m_pointPtrList.size(); ++i){
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

                m_labelPtrList[i]->setNum(m_pointPtrList[i]->current_value);
                updateLabelPos();
                emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
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

