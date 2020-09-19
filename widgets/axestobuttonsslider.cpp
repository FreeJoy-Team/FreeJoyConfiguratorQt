#include "axestobuttonsslider.h"
#include "ui_axestobuttonsslider.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPolygon>
#include <cmath>

//#include <QDebug>
AxesToButtonsSlider::AxesToButtonsSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesToButtonsSlider)
{
    ui->setupUi(this);

    setMouseTracking(true);

    this->setMinimumHeight(kMinHeight_);

    // call SetPointsCount?
    points_count_ = 0;
    axis_output_value_ = 0;
    axis_output_width_ = 0;
    axis_rect_color_ = kAxisRectColor_dis;
    is_out_enabled_ = true;

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

void AxesToButtonsSlider::SetAxisOutputValue(int out_value, bool is_enable)
{
    is_out_enabled_ = is_enable;
    axis_output_value_ = out_value;// + AXIS_MAX_VALUE) / (float)AXIS_FULLSCALE;        //abs((value  - min)/ (float)(max - min));
    if (is_enable && this->isEnabled() == true){
        axis_rect_color_ = kAxisRectColor;
    } else {
        axis_rect_color_ = kAxisRectColor_dis;
    }
    update();
}

void AxesToButtonsSlider::paintEvent(QPaintEvent *event)        // оптимизхировать width_ - offset_*2
{
    Q_UNUSED(event)
    QPainter painter;
    int rect_height = 5;
    int rect_y = 7 + padding_top_;

    painter.begin(this);

    painter.setPen(Qt::lightGray);
    painter.drawRect(QRect(offset_, rect_y, this->width() - offset_*2, rect_height));

    // calc and paint raw data on a2b
    axis_output_width_ = (axis_output_value_ + AXIS_MAX_VALUE) / (float)AXIS_FULLSCALE * (this->width() - offset_*2);
    QRect rect(offset_, rect_y, axis_output_width_, rect_height);
    painter.drawRect(rect);
    painter.fillRect(rect, axis_rect_color_);

    // paint separators
    painter.setPen(Qt::lightGray);
    for (uint i = 0; i < 25; ++i){
        painter.drawLine((i * line_spacing_) + offset_, 15 + padding_top_, (i * line_spacing_) + offset_, 18 + padding_top_);
    }
    // Antialiasing     // спорно, мазня
    //painter.setRenderHint(QPainter::Antialiasing, true);

    // paint pointers
    for (uint i = 0; i < points_count_; ++i)
    {
//        if (this->isEnabled() == true && PointAdrList[i]->posX - half_pointer_width < (uint)axis_raw_width_){
//            painter.setBrush(kPointRawActivColor);
//            painter.setPen(kPointRawActivColor);
//        } else {
            painter.setBrush(PointAdrList[i]->color);
            painter.setPen(PointAdrList[i]->color);
//        }
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
                if (uint(point_pos.x()) < PointAdrList[point_number - 1]->posX +range_between_ ||        // баг при загрузке конфига?
                    uint(point_pos.x()) > PointAdrList[point_number + 1]->posX -range_between_)          // если точки впритык и происходит округление
                {
                    return;
                }
            } else if (point_number == 0){
                if (uint(point_pos.x()) > PointAdrList[point_number + 1]->posX -range_between_){
                    return;
                }
            } else {
                if (uint(point_pos.x()) < PointAdrList[point_number - 1]->posX +range_between_){
                    return;
                }
            }
        }
        MovePointer(point_pos.x(), point_number);
        PointAdrList[point_number]->posX = point_pos.x();
        PointAdrList[point_number]->current_value = CalcPointValue(PointAdrList[point_number]->posX);
        //SetLableValue(point_pos.x(), point_number);
        LabelAdrList[point_number]->setNum((int)PointAdrList[point_number]->current_value);
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
            label->setFont(QFont("MS Shell Dlg 2", 8));
            PointAdrList.append(point);
            LabelAdrList.append(label);
            point->polygon << pointer[0] << pointer[1] << pointer[2] << pointer[3] << pointer[4];
            label->setMinimumWidth(label_width_);
            label->setAlignment(Qt::AlignHCenter);
            label->setStyleSheet("background:transparent;");
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
    uint pos = uint((value * (this->width() - offset_*2.0f) / max_point_value_));   // поколдовать
    // ?
    pos += offset_;
    if (pos > uint(this->width() - offset_)){
        pos = this->width() - offset_;
    } else if (pos < uint(offset_)){
        pos = offset_;
    }
    MovePointer(pos, point_number);
    //SetLableValue(pos, point_number);
    LabelAdrList[point_number]->setNum(int(value));
    update();
}

uint AxesToButtonsSlider::GetPointValue(uint point_number)
{
    return PointAdrList[point_number]->current_value;
}


uint AxesToButtonsSlider::CalcPointValue(int current_pos)
{
    float tmp_value = (this->width() - offset_*2.0f) / max_point_value_;    // для красоты
    return uint(round(  (current_pos - offset_) / tmp_value  ));
}

void AxesToButtonsSlider::PointsPositionReset()
{
    float tmp_distance = (this->width() - offset_*2.0f) / (points_count_ -1);   // поколдовать
    // apply color, position
    for (int i = 0; i < PointAdrList.size(); ++i) {
        if (this->isEnabled() == true){
            PointAdrList[i]->color = pointer_color_;
            //axis_rect_color_ = kAxisRectColor;
        } else {
            PointAdrList[i]->color = Qt::lightGray;
            //axis_rect_color_ = kAxisRectColor_dis;
        }
        //PointAdrList[i]->color = pointer_color_;
        PointAdrList[i]->is_drag = false;
        PointAdrList[i]->posX = round(i * tmp_distance + offset_);
    }
    // last to last X position
    PointAdrList[points_count_ - 1]->posX = this->width() - offset_;
    // move points
    for (uint i = 0; i < points_count_; ++i) {
        MovePointer(PointAdrList[i]->posX, i);
        //SetLableValue(PointAdrList[i]->posX, i);
        PointAdrList[i]->current_value = CalcPointValue(PointAdrList[i]->posX);
        LabelAdrList[i]->setNum((int)PointAdrList[i]->current_value);
    }
    update();
}

void AxesToButtonsSlider::MovePointer(uint pos_x, uint point_number)
{
    //QPolygon* polygon = &PointAdrList[point_number]->polygon;       // не думаю что это даст выигрыш в оптимизации вместо нескольких PointAdrList[point_number]->polygon
    LabelAdrList[point_number]->move(pos_x - label_width_/2, pointer[2].y() + padding_top_);       // label move
    PointAdrList[point_number]->posX = pos_x;
    PointAdrList[point_number]->polygon.setPoint(0, pos_x - half_pointer_width, pointer[0].y() + padding_top_);
    PointAdrList[point_number]->polygon.setPoint(1, pos_x - half_pointer_width, pointer[1].y() + padding_top_);
    PointAdrList[point_number]->polygon.setPoint(2, pos_x, pointer[2].y() + padding_top_);
    PointAdrList[point_number]->polygon.setPoint(3, pos_x + half_pointer_width, pointer[3].y() + padding_top_);
    PointAdrList[point_number]->polygon.setPoint(4, pos_x + half_pointer_width, pointer[4].y() + padding_top_);
}


void AxesToButtonsSlider::mouseMoveEvent(QMouseEvent *event)
{
    for (uint i = 0; i < points_count_; ++i)
    {
        if (PointAdrList[i]->is_drag == false){
            if (PointAdrList[i]->polygon.containsPoint(event->pos(), Qt::WindingFill)) {
                    PointAdrList[i]->color = Qt::black;
            } else {
                PointAdrList[i]->color = pointer_color_;
            }
        }
        else if (PointAdrList[i]->is_drag == true){        // много лишнего. потом чекнуть
            if (event->buttons() & Qt::LeftButton){
                DrawPoint(event->pos(), i);
            }
            if (PointAdrList[i]->is_drag) {
                PointAdrList[i]->color = Qt::lightGray;
            }
            break;      // not tested
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
            PointAdrList[i]->color = pointer_color_;        // ????
            break;
        }
    }
    update();
}

void AxesToButtonsSlider::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    float tmp_value = (this->width() - offset_*2.0f) / (float)max_point_value_;
    for (int i = 0; i < PointAdrList.size(); ++i) {
        PointAdrList[i]->posX = (PointAdrList[i]->current_value * tmp_value) + offset_;
        MovePointer(PointAdrList[i]->posX, i);
    }
    line_spacing_ = (this->width() - offset_*2.0f) / 24.0f;
}

bool AxesToButtonsSlider::event(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        if (this->isEnabled() == true){
            for (int i = 0; i < PointAdrList.size(); ++i) {
                PointAdrList[i]->color = pointer_color_;
            }
            if (is_out_enabled_ == true){
               axis_rect_color_ = kAxisRectColor;
            }
        } else {
            for (int i = 0; i < PointAdrList.size(); ++i) {
                PointAdrList[i]->color = Qt::lightGray;
            }
            axis_rect_color_ = kAxisRectColor_dis;
        }
        update();
        return true;
    }
    return QWidget::event(event);
}
