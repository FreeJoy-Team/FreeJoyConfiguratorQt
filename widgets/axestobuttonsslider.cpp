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
    axis_rect_color_ = kAxisRectColor_dis_;
    is_out_enabled_ = true;

}

AxesToButtonsSlider::~AxesToButtonsSlider()
{
    for (uint i = 0; i < points_count_; ++i) {
        A2B_point* point = PointPtrList_.takeLast();
        QLabel* label = LabelPtrList_.takeLast();
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
        axis_rect_color_ = kAxisRectColor_;
    } else {
        axis_rect_color_ = kAxisRectColor_dis_;
    }
    update();
}

void AxesToButtonsSlider::paintEvent(QPaintEvent *event)        // оптимизхировать width_ - offset_*2
{
    Q_UNUSED(event)
    QPainter painter;
    int rect_height = 5;
    int rect_y = 7 + kPaddingTop_;

    painter.begin(this);

    painter.setPen(Qt::lightGray);
    painter.drawRect(QRect(kOffset_, rect_y, this->width() - kOffset_*2, rect_height));

    // calc and paint raw data on a2b
    axis_output_width_ = (axis_output_value_ + AXIS_MAX_VALUE) / (float)AXIS_FULLSCALE * (this->width() - kOffset_*2);
    QRect rect(kOffset_, rect_y, axis_output_width_, rect_height);
    painter.drawRect(rect);
    painter.fillRect(rect, axis_rect_color_);

    // paint separators
    painter.setPen(Qt::lightGray);
    for (uint i = 0; i < 25; ++i){
        painter.drawLine((i * line_spacing_) + kOffset_, 15 + kPaddingTop_, (i * line_spacing_) + kOffset_, 18 + kPaddingTop_);
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
            painter.setBrush(PointPtrList_[i]->color);
            painter.setPen(PointPtrList_[i]->color);
//        }
        painter.drawPolygon(PointPtrList_[i]->polygon, Qt::WindingFill);
    }
    painter.end();
}

void AxesToButtonsSlider::DrawPoint(QPoint point_pos, uint point_number) {

    if (point_pos.x() < kOffset_ || point_pos.x() > this->width() - kOffset_) {
        return;
    }
    else if (PointPtrList_[point_number]->is_drag && point_pos.x() < this->width() + kOffset_) {

        if (points_count_ > 1){
            if (point_number > 0 && point_number < points_count_ - 1)
            {
                if (uint(point_pos.x()) < PointPtrList_[point_number - 1]->posX +kRangeBetween_ ||        // баг при загрузке конфига?
                    uint(point_pos.x()) > PointPtrList_[point_number + 1]->posX -kRangeBetween_)          // если точки впритык и происходит округление
                {
                    return;
                }
            } else if (point_number == 0){
                if (uint(point_pos.x()) > PointPtrList_[point_number + 1]->posX -kRangeBetween_){
                    return;
                }
            } else {
                if (uint(point_pos.x()) < PointPtrList_[point_number - 1]->posX +kRangeBetween_){
                    return;
                }
            }
        }
        MovePointer(point_pos.x(), point_number);
        PointPtrList_[point_number]->posX = point_pos.x();
        PointPtrList_[point_number]->current_value = CalcPointValue(PointPtrList_[point_number]->posX);
        //SetLableValue(point_pos.x(), point_number);
        LabelPtrList_[point_number]->setNum((int)PointPtrList_[point_number]->current_value);
        return;
    }
}

void AxesToButtonsSlider::SetLableValue(int point_pos ,uint point_number)
{
    LabelPtrList_[point_number]->setNum(int(CalcPointValue(point_pos)));
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
            PointPtrList_.append(point);
            LabelPtrList_.append(label);
            point->polygon << kPointer_[0] << kPointer_[1] << kPointer_[2] << kPointer_[3] << kPointer_[4];
            label->setMinimumWidth(kLabelWidth_);
            label->setAlignment(Qt::AlignHCenter);
            label->setStyleSheet("background:transparent;");
            label->setVisible(true);
        }
    }
    // delete points
    else if (count < points_count_)
    {
        for (uint i = 0; i < points_count_ - count; ++i) {
            A2B_point* point = PointPtrList_.takeLast();
            QLabel* label = LabelPtrList_.takeLast();
            delete point;
            delete label;
        }
    }
    points_count_ = count;
    PointsPositionReset();
}

uint AxesToButtonsSlider::GetPointsCount() const
{
    return points_count_;
}

void AxesToButtonsSlider::SetPointValue(uint value, uint point_number)
{
    if (point_number > points_count_){
        point_number = points_count_;
    }
    uint pos = uint((value * (this->width() - kOffset_*2.0f) / kMaxPointValue_));   // поколдовать
    // ?
    pos += kOffset_;
    if (pos > uint(this->width() - kOffset_)){
        pos = this->width() - kOffset_;
    } else if (pos < uint(kOffset_)){
        pos = kOffset_;
    }
    MovePointer(pos, point_number);
    //SetLableValue(pos, point_number);
    LabelPtrList_[point_number]->setNum(int(value));
    update();
}

uint AxesToButtonsSlider::GetPointValue(uint point_number) const
{
    return PointPtrList_[point_number]->current_value;
}


uint AxesToButtonsSlider::CalcPointValue(int current_pos) const
{
    float tmp_value = (this->width() - kOffset_*2.0f) / kMaxPointValue_;    // для красоты
    return uint(round(  (current_pos - kOffset_) / tmp_value  ));
}

void AxesToButtonsSlider::PointsPositionReset()
{
    float tmp_distance = (this->width() - kOffset_*2.0f) / (points_count_ -1);   // поколдовать
    // apply color, position
    for (int i = 0; i < PointPtrList_.size(); ++i) {
        if (this->isEnabled() == true){
            PointPtrList_[i]->color = kPointerColor_;
            //axis_rect_color_ = kAxisRectColor;
        } else {
            PointPtrList_[i]->color = Qt::lightGray;
            //axis_rect_color_ = kAxisRectColor_dis;
        }
        //PointAdrList[i]->color = pointer_color_;
        PointPtrList_[i]->is_drag = false;
        PointPtrList_[i]->posX = round(i * tmp_distance + kOffset_);
    }
    // last to last X position
    PointPtrList_[points_count_ - 1]->posX = this->width() - kOffset_;
    // move points
    for (uint i = 0; i < points_count_; ++i) {
        MovePointer(PointPtrList_[i]->posX, i);
        //SetLableValue(PointAdrList[i]->posX, i);
        PointPtrList_[i]->current_value = CalcPointValue(PointPtrList_[i]->posX);
        LabelPtrList_[i]->setNum((int)PointPtrList_[i]->current_value);
    }
    update();
}

void AxesToButtonsSlider::MovePointer(uint pos_x, uint point_number)
{
    //QPolygon* polygon = &PointAdrList[point_number]->polygon;       // не думаю что это даст выигрыш в оптимизации вместо нескольких PointAdrList[point_number]->polygon
    LabelPtrList_[point_number]->move(pos_x - kLabelWidth_/2, kPointer_[2].y() + kPaddingTop_);       // label move
    PointPtrList_[point_number]->posX = pos_x;
    PointPtrList_[point_number]->polygon.setPoint(0, pos_x - kHalfPointerWidth_, kPointer_[0].y() + kPaddingTop_);
    PointPtrList_[point_number]->polygon.setPoint(1, pos_x - kHalfPointerWidth_, kPointer_[1].y() + kPaddingTop_);
    PointPtrList_[point_number]->polygon.setPoint(2, pos_x, kPointer_[2].y() + kPaddingTop_);
    PointPtrList_[point_number]->polygon.setPoint(3, pos_x + kHalfPointerWidth_, kPointer_[3].y() + kPaddingTop_);
    PointPtrList_[point_number]->polygon.setPoint(4, pos_x + kHalfPointerWidth_, kPointer_[4].y() + kPaddingTop_);
}


void AxesToButtonsSlider::mouseMoveEvent(QMouseEvent *event)
{
    for (uint i = 0; i < points_count_; ++i)
    {
        if (PointPtrList_[i]->is_drag == false){
            if (PointPtrList_[i]->polygon.containsPoint(event->pos(), Qt::WindingFill)) {
                    PointPtrList_[i]->color = Qt::black;
            } else {
                PointPtrList_[i]->color = kPointerColor_;
            }
        }
        else if (PointPtrList_[i]->is_drag == true){        // много лишнего. потом чекнуть
            if (event->buttons() & Qt::LeftButton){
                DrawPoint(event->pos(), i);
            }
            if (PointPtrList_[i]->is_drag) {
                PointPtrList_[i]->color = Qt::lightGray;
            }
            break;      // not tested
        }
    }
    update();
}

void AxesToButtonsSlider::mousePressEvent(QMouseEvent *event)
{
    for (uint i = 0; i < points_count_; ++i){
        if (PointPtrList_[i]->polygon.containsPoint(event->pos(), Qt::WindingFill)) {
            PointPtrList_[i]->is_drag = true;
            break;
        }
    }
}

void AxesToButtonsSlider::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    for (uint i = 0; i < points_count_; ++i)
    {
        if(PointPtrList_[i]->is_drag == true)
        {
            PointPtrList_[i]->is_drag = false;
            PointPtrList_[i]->color = kPointerColor_;        // ????
            break;
        }
    }
    update();
}

void AxesToButtonsSlider::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    float tmp_value = (this->width() - kOffset_*2.0f) / (float)kMaxPointValue_;
    for (int i = 0; i < PointPtrList_.size(); ++i) {
        PointPtrList_[i]->posX = (PointPtrList_[i]->current_value * tmp_value) + kOffset_;
        MovePointer(PointPtrList_[i]->posX, i);
    }
    line_spacing_ = (this->width() - kOffset_*2.0f) / 24.0f;
}

bool AxesToButtonsSlider::event(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        if (this->isEnabled() == true){
            for (int i = 0; i < PointPtrList_.size(); ++i) {
                PointPtrList_[i]->color = kPointerColor_;
            }
            if (is_out_enabled_ == true){
               axis_rect_color_ = kAxisRectColor_;
            }
        } else {
            for (int i = 0; i < PointPtrList_.size(); ++i) {
                PointPtrList_[i]->color = Qt::lightGray;
            }
            axis_rect_color_ = kAxisRectColor_dis_;
        }
        update();
        return true;
    }
    return QWidget::event(event);
}
