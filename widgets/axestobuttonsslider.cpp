#include "axestobuttonsslider.h"
#include "ui_axestobuttonsslider.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPolygon>
#include <cmath>

#include <QDebug>
AxesToButtonsSlider::AxesToButtonsSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesToButtonsSlider)
{
    ui->setupUi(this);

    setMouseTracking(true);
    this->setMinimumHeight(40);

    points_count_ = 0;

}

AxesToButtonsSlider::~AxesToButtonsSlider()
{
    for (uint i = 0; i < points_count_; ++i) {
        A2B_point* point = PointAdrList.takeLast();
        QLabel* label = LabelAdrList.takeLast();
        delete point;
        delete label;
    }
    delete ui;
}

void AxesToButtonsSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter;
    int width;
    float tmp;

    width = this->width();
    tmp = (width - offset_*2) / 24.0;

//    QPen pen;
//    pen.setWidth(1);
//    pen.setColor(Qt::lightGray);
    painter.begin(this);
    painter.setPen(Qt::lightGray);
    painter.drawRect(QRect(offset_, 7, width - offset_*2, 5));

    for (uint i = 0; i < 25; ++i){
        painter.drawLine((i * tmp) + offset_, 15, (i * tmp) + offset_, 18);
    }

    for (uint i = 0; i < points_count_; ++i)
    {
        painter.setBrush(PointAdrList[i]->color);
        painter.setPen(PointAdrList[i]->color);
        painter.drawPolygon(PointAdrList[i]->polygon, Qt::WindingFill);
    }
    painter.end();
}

void AxesToButtonsSlider::DrawPoint(QPoint point_pos, uint point_number) {

    if (point_pos.x() < offset_ || point_pos.x() > this->width() - offset_) {
        return;
    }
    else if (PointAdrList[point_number]->is_drag && point_pos.x() < this->width() + offset_) {

        if (points_count_ > 1){
            if (point_number > 0 && point_number < points_count_ - 1)
            {
                if (uint(point_pos.x()) < PointAdrList[point_number - 1]->posX +6 ||        // 6 - мин расстояние до соседних точек
                    uint(point_pos.x()) > PointAdrList[point_number + 1]->posX -6)          // убрать хардкод
                {
                    return;
                }
            } else if (point_number == 0){
                if (uint(point_pos.x()) > PointAdrList[point_number + 1]->posX -6){
                    return;
                }
            } else {
                if (uint(point_pos.x()) < PointAdrList[point_number - 1]->posX +6){
                    return;
                }
            }
        }
        MovePointer(point_pos.x(), point_number);
        PointAdrList[point_number]->posX = point_pos.x();
        SetLableValue(point_pos.x(), point_number);
        PointAdrList[point_number]->current_value = CalcPointValue(PointAdrList[point_number]->posX);
        return;
    }
}

void AxesToButtonsSlider::SetLableValue(int point_pos ,uint point_number)
{
    LabelAdrList[point_number]->setNum(int(CalcPointValue(point_pos)));
}

void AxesToButtonsSlider::SetPointsCount(uint count)        // count = 0 -crash
{
    if (count < 2) count = 2;
    // create points
    if (count > points_count_)
    {
        for (uint i = 0; i < count - points_count_; ++i) {
            A2B_point* point = new A2B_point;
            QLabel* label = new QLabel(this);
            PointAdrList.append(point);
            LabelAdrList.append(label);
            point->polygon << pointer[0] << pointer[1] << pointer[2] << pointer[3] << pointer[4];
            label->setMinimumWidth(label_width_);
            label->setAlignment(Qt::AlignHCenter);
            label->setVisible(true);
        }
    }
    // delete points
    else if (count < points_count_)
    {
        for (uint i = 0; i < points_count_ - count; ++i) {
            A2B_point* point = PointAdrList.takeLast();
            QLabel* label = LabelAdrList.takeLast();
            delete point;
            delete label;
        }
    }
    points_count_ = count;

    PointsPositionReset();
}

uint AxesToButtonsSlider::GetPointsCount()
{
    return points_count_;
}


void AxesToButtonsSlider::SetPointValue(uint value, uint point_number)
{
    if (point_number > points_count_){
        point_number = points_count_;
    }
    uint pos = uint(round(value * float(this->width() - offset_*2) / max_point_value_));
    // ?
    pos += offset_;
    if (pos > uint(this->width() - offset_)){
        pos = this->width() - offset_;
    } else if (pos < uint(offset_)){
        pos = offset_;
    }
    MovePointer(pos, point_number);
    update();
}

uint AxesToButtonsSlider::GetPointValue(uint point_number)
{
    return PointAdrList[point_number]->current_value;
}


uint AxesToButtonsSlider::CalcPointValue(int current_pos)
{
    float tmp_value = (this->width() - offset_*2) / float(max_point_value_);    // для красоты
    return uint(round(  (current_pos - offset_) / tmp_value  ));
}

void AxesToButtonsSlider::PointsPositionReset()
{
    int tmp_distance = round((float(this->width()) - offset_*2) / (points_count_ -1));
    // apply color, position
    for (int i = 0; i < PointAdrList.size(); ++i) {
        if (this->isEnabled() == true){
            PointAdrList[i]->color = pointer_color_;
        } else {
            PointAdrList[i]->color = Qt::lightGray;
        }
        //PointAdrList[i]->color = pointer_color_;
        PointAdrList[i]->is_drag = false;
        PointAdrList[i]->posX = i * tmp_distance + offset_;
    }
    // last to last X position
    PointAdrList[points_count_ - 1]->posX = this->width() - offset_;
    // move points
    for (uint i = 0; i < points_count_; ++i) {
        MovePointer(PointAdrList[i]->posX, i);
        SetLableValue(PointAdrList[i]->posX, i);
        PointAdrList[i]->current_value = CalcPointValue(PointAdrList[i]->posX);
    }
    update();
}

void AxesToButtonsSlider::MovePointer(uint pos_x, uint point_number)
{
    LabelAdrList[point_number]->move(pos_x - label_width_/2, pointer[2].y());       // label move

    PointAdrList[point_number]->posX = pos_x;
    PointAdrList[point_number]->polygon.setPoint(0, pos_x - half_pointer_width, pointer[0].y());
    PointAdrList[point_number]->polygon.setPoint(1, pos_x - half_pointer_width, pointer[1].y());
    PointAdrList[point_number]->polygon.setPoint(2, pos_x, pointer[2].y());
    PointAdrList[point_number]->polygon.setPoint(3, pos_x + half_pointer_width, pointer[3].y());
    PointAdrList[point_number]->polygon.setPoint(4, pos_x + half_pointer_width, pointer[4].y());
}


void AxesToButtonsSlider::mouseMoveEvent(QMouseEvent *event)
{
    for (uint i = 0; i < points_count_; ++i)
    {
        if (PointAdrList[i]->polygon.containsPoint(event->pos(), Qt::WindingFill)) {
                PointAdrList[i]->color = Qt::black;
        } else {
            PointAdrList[i]->color = pointer_color_;
        }

        if (event->buttons() & Qt::LeftButton){
            DrawPoint(event->pos(), i);
        }

        if (PointAdrList[i]->is_drag) {
            PointAdrList[i]->color = Qt::lightGray;
        }
    }
    update();
}

void AxesToButtonsSlider::mousePressEvent(QMouseEvent *event)
{
    for (uint i = 0; i < points_count_; ++i){
        if (PointAdrList[i]->polygon.containsPoint(event->pos(), Qt::WindingFill)) {
            PointAdrList[i]->is_drag = true;
            break;
        }
    }
}

void AxesToButtonsSlider::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    for (uint i = 0; i < points_count_; ++i)
    {
        if(PointAdrList[i]->is_drag == true)
        {
            PointAdrList[i]->is_drag = false;
            PointAdrList[i]->color = pointer_color_;
            break;
        }
    }
    update();
}

void AxesToButtonsSlider::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    float tmp_value = (event->size().width() - offset_*2) / float(max_point_value_);
    for (int i = 0; i < PointAdrList.size(); ++i) {
        PointAdrList[i]->posX = (PointAdrList[i]->current_value * tmp_value) + offset_;
        MovePointer(PointAdrList[i]->posX, i);
    }
}

bool AxesToButtonsSlider::event(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        if (this->isEnabled() == true){
            for (int i = 0; i < PointAdrList.size(); ++i) {
                PointAdrList[i]->color = pointer_color_;
            }
        } else {
            for (int i = 0; i < PointAdrList.size(); ++i) {
                PointAdrList[i]->color = Qt::lightGray;
            }
        }
        update();
        return true;
    }
    return QWidget::event(event);
}
