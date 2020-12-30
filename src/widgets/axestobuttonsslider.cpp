#include "axestobuttonsslider.h"
#include "ui_axestobuttonsslider.h"
#include <cmath>
#include <QMouseEvent>
#include <QPainter>
#include <QPolygon>

//#include <QDebug>
AxesToButtonsSlider::AxesToButtonsSlider(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AxesToButtonsSlider)
{
    ui->setupUi(this);

    setMouseTracking(true);

    this->setMinimumHeight(m_kMinHeight);

    // call SetPointsCount?
    m_pointsCount = 0;
    m_axisOutputValue = 0;
    m_axisOutputWidth = 0;
    m_axisRectColor = m_kAxisRectColor_dis;
    m_isOutEnabled = true;
}

AxesToButtonsSlider::~AxesToButtonsSlider()
{
    for (uint i = 0; i < m_pointsCount; ++i) {
        A2B_point *point = m_pointPtrList.takeLast();
        QLabel *label = m_labelPtrList.takeLast();
        delete point;
        delete label;
    }
    delete ui;
}

void AxesToButtonsSlider::setAxisOutputValue(int outValue, bool isEnable)
{
    m_isOutEnabled = isEnable;
    m_axisOutputValue = outValue; // + AXIS_MAX_VALUE) / (float)AXIS_FULLSCALE;        //abs((value  - min)/ (float)(max - min));
    if (isEnable && this->isEnabled() == true) {
        m_axisRectColor = m_kAxisRectColor;
    } else {
        m_axisRectColor = m_kAxisRectColor_dis;
    }
    update();
}

void AxesToButtonsSlider::paintEvent(QPaintEvent *event) // оптимизхировать width_ - offset_*2
{
    Q_UNUSED(event)
    QPainter painter;
    int rect_height = 5;
    int rect_y = 7 + m_kPaddingTop;

    painter.begin(this);

    painter.setPen(Qt::lightGray);
    painter.drawRect(QRect(m_kOffset, rect_y, this->width() - m_kOffset * 2, rect_height));

    // calc and paint raw data on a2b
    m_axisOutputWidth = (m_axisOutputValue + AXIS_MAX_VALUE) / (float) AXIS_FULLSCALE * (this->width() - m_kOffset * 2);
    QRect rect(m_kOffset, rect_y, m_axisOutputWidth, rect_height);
    painter.drawRect(rect);
    painter.fillRect(rect, m_axisRectColor);

    // paint separators
    painter.setPen(Qt::lightGray);
    for (uint i = 0; i < 25; ++i) {
        painter.drawLine((i * m_lineSpacing) + m_kOffset,
                         15 + m_kPaddingTop,
                         (i * m_lineSpacing) + m_kOffset,
                         18 + m_kPaddingTop);
    }
    // Antialiasing     // спорно, мазня
    //painter.setRenderHint(QPainter::Antialiasing, true);

    // paint pointers
    for (uint i = 0; i < m_pointsCount; ++i) {
        //        if (this->isEnabled() == true && PointAdrList[i]->posX - half_pointer_width < (uint)axis_raw_width_){
        //            painter.setBrush(kPointRawActivColor);
        //            painter.setPen(kPointRawActivColor);
        //        } else {
        painter.setBrush(m_pointPtrList[i]->color);
        painter.setPen(m_pointPtrList[i]->color);
        //        }
        painter.drawPolygon(m_pointPtrList[i]->polygon, Qt::WindingFill);
    }
    painter.end();
}

void AxesToButtonsSlider::drawPoint(const QPoint &pointPos, uint pointNumber)
{
    if (pointPos.x() < m_kOffset || pointPos.x() > this->width() - m_kOffset) {
        return;
    } else if (m_pointPtrList[pointNumber]->is_drag && pointPos.x() < this->width() + m_kOffset) {
        if (m_pointsCount > 1) {
            if (pointNumber > 0 && pointNumber < m_pointsCount - 1) {
                if (uint(pointPos.x()) < m_pointPtrList[pointNumber - 1]->posX + m_kRangeBetween // баг при загрузке конфига?
                    || uint(pointPos.x()) > m_pointPtrList[pointNumber + 1]->posX - m_kRangeBetween) // если точки впритык и происходит округление
                {
                    return;
                }
            } else if (pointNumber == 0) {
                if (uint(pointPos.x()) > m_pointPtrList[pointNumber + 1]->posX - m_kRangeBetween) {
                    return;
                }
            } else {
                if (uint(pointPos.x()) < m_pointPtrList[pointNumber - 1]->posX + m_kRangeBetween) {
                    return;
                }
            }
        }
        movePointer(pointPos.x(), pointNumber);
        m_pointPtrList[pointNumber]->posX = pointPos.x();
        m_pointPtrList[pointNumber]->current_value = calcPointValue(m_pointPtrList[pointNumber]->posX);
        //SetLableValue(point_pos.x(), point_number);
        m_labelPtrList[pointNumber]->setNum((int) m_pointPtrList[pointNumber]->current_value);
        return;
    }
}

void AxesToButtonsSlider::setLableValue(int pointPos, uint pointNumber)
{
    m_labelPtrList[pointNumber]->setNum(int(calcPointValue(pointPos)));
}

void AxesToButtonsSlider::setPointsCount(uint count)
{
    if (count < 2)
        count = 2;
    // create points
    if (count > m_pointsCount) {
        for (uint i = 0; i < count - m_pointsCount; ++i) {
            A2B_point *point = new A2B_point;
            QLabel *label = new QLabel(this);
            label->setFont(QFont("MS Shell Dlg 2", 8));
            m_pointPtrList.append(point);
            m_labelPtrList.append(label);
            point->polygon << m_kPointer[0] << m_kPointer[1] << m_kPointer[2] << m_kPointer[3] << m_kPointer[4];
            label->setMinimumWidth(m_kLabelWidth);
            label->setAlignment(Qt::AlignHCenter);
            label->setStyleSheet("background:transparent;");
            label->setVisible(true);
        }
    }
    // delete points
    else if (count < m_pointsCount) {
        for (uint i = 0; i < m_pointsCount - count; ++i) {
            A2B_point *point = m_pointPtrList.takeLast();
            QLabel *label = m_labelPtrList.takeLast();
            delete point;
            delete label;
        }
    }
    m_pointsCount = count;
    pointsPositionReset();
}

uint AxesToButtonsSlider::pointsCount() const
{
    return m_pointsCount;
}

void AxesToButtonsSlider::setPointValue(uint value, uint pointNumber)
{
    if (pointNumber > m_pointsCount) {
        pointNumber = m_pointsCount;
    }
    uint pos = uint((value * (this->width() - m_kOffset * 2.0f) / m_kMaxPointValue)); // поколдовать
    // ?
    pos += m_kOffset;
    if (pos > uint(this->width() - m_kOffset)) {
        pos = this->width() - m_kOffset;
    } else if (pos < uint(m_kOffset)) {
        pos = m_kOffset;
    }
    movePointer(pos, pointNumber);
    //SetLableValue(pos, point_number);
    m_labelPtrList[pointNumber]->setNum(int(value));
    update();
}

uint AxesToButtonsSlider::pointValue(uint pointNumber) const
{
    return m_pointPtrList[pointNumber]->current_value;
}

uint AxesToButtonsSlider::calcPointValue(int currentPos) const
{
    float tmp_value = (this->width() - m_kOffset * 2.0f) / m_kMaxPointValue;
    return uint(round((currentPos - m_kOffset) / tmp_value));
}

void AxesToButtonsSlider::pointsPositionReset()
{
    float tmp_distance = (this->width() - m_kOffset * 2.0f) / (m_pointsCount - 1); // поколдовать
    // apply color, position
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        if (this->isEnabled() == true) {
            m_pointPtrList[i]->color = m_kPointerColor;
            //axis_rect_color_ = kAxisRectColor;
        } else {
            m_pointPtrList[i]->color = Qt::lightGray;
            //axis_rect_color_ = kAxisRectColor_dis;
        }
        //PointAdrList[i]->color = pointer_color_;
        m_pointPtrList[i]->is_drag = false;
        m_pointPtrList[i]->posX = round(i * tmp_distance + m_kOffset);
    }
    // last to last X position
    m_pointPtrList[m_pointsCount - 1]->posX = this->width() - m_kOffset;
    // move points
    for (uint i = 0; i < m_pointsCount; ++i) {
        movePointer(m_pointPtrList[i]->posX, i);
        //SetLableValue(PointAdrList[i]->posX, i);
        m_pointPtrList[i]->current_value = calcPointValue(m_pointPtrList[i]->posX);
        m_labelPtrList[i]->setNum(int(m_pointPtrList[i]->current_value));
    }
    update();
}

void AxesToButtonsSlider::movePointer(uint posX, uint pointNumber)
{
    QPolygon *polygon = &m_pointPtrList[pointNumber]->polygon;
    m_labelPtrList[pointNumber]->move(posX - m_kLabelWidth / 2, m_kPointer[2].y() + m_kPaddingTop); // label move
    m_pointPtrList[pointNumber]->posX = posX;
    polygon->setPoint(0, posX - m_kHalfPointerWidth, m_kPointer[0].y() + m_kPaddingTop);
    polygon->setPoint(1, posX - m_kHalfPointerWidth, m_kPointer[1].y() + m_kPaddingTop);
    polygon->setPoint(2, posX, m_kPointer[2].y() + m_kPaddingTop);
    polygon->setPoint(3, posX + m_kHalfPointerWidth, m_kPointer[3].y() + m_kPaddingTop);
    polygon->setPoint(4, posX + m_kHalfPointerWidth, m_kPointer[4].y() + m_kPaddingTop);
}

void AxesToButtonsSlider::mouseMoveEvent(QMouseEvent *event)
{
    for (uint i = 0; i < m_pointsCount; ++i) {
        if (m_pointPtrList[i]->is_drag == false) {
            if (m_pointPtrList[i]->polygon.containsPoint(event->pos(), Qt::WindingFill)) {
                m_pointPtrList[i]->color = Qt::black;
            } else {
                m_pointPtrList[i]->color = m_kPointerColor;
            }
        } else if (m_pointPtrList[i]->is_drag == true) { // много лишнего. потом чекнуть
            if (event->buttons() & Qt::LeftButton) {
                drawPoint(event->pos(), i);
            }
            if (m_pointPtrList[i]->is_drag) {
                m_pointPtrList[i]->color = Qt::lightGray;
            }
            break; // not tested
        }
    }
    update();
}

void AxesToButtonsSlider::mousePressEvent(QMouseEvent *event)
{
    for (uint i = 0; i < m_pointsCount; ++i) {
        if (m_pointPtrList[i]->polygon.containsPoint(event->pos(), Qt::WindingFill)) {
            m_pointPtrList[i]->is_drag = true;
            break;
        }
    }
}

void AxesToButtonsSlider::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    for (uint i = 0; i < m_pointsCount; ++i) {
        if (m_pointPtrList[i]->is_drag == true) {
            m_pointPtrList[i]->is_drag = false;
            m_pointPtrList[i]->color = m_kPointerColor; // ????
            break;
        }
    }
    update();
}

void AxesToButtonsSlider::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    float tmp_value = (this->width() - m_kOffset * 2.0f) / (float) m_kMaxPointValue;
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        m_pointPtrList[i]->posX = (m_pointPtrList[i]->current_value * tmp_value) + m_kOffset;
        movePointer(m_pointPtrList[i]->posX, i);
    }
    m_lineSpacing = (this->width() - m_kOffset * 2.0f) / 24.0f;
}

bool AxesToButtonsSlider::event(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        if (this->isEnabled() == true) {
            for (int i = 0; i < m_pointPtrList.size(); ++i) {
                m_pointPtrList[i]->color = m_kPointerColor;
            }
            if (m_isOutEnabled == true) {
                m_axisRectColor = m_kAxisRectColor;
            }
        } else {
            for (int i = 0; i < m_pointPtrList.size(); ++i) {
                m_pointPtrList[i]->color = Qt::lightGray;
            }
            m_axisRectColor = m_kAxisRectColor_dis;
        }
        update();
        return true;
    }
    return QWidget::event(event);
}
