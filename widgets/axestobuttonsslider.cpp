#include "axestobuttonsslider.h"
#include "ui_axestobuttonsslider.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPolygon>

AxesToButtonsSlider::AxesToButtonsSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesToButtonsSlider)
{
    ui->setupUi(this);

    buttons_count_ = 2;

    for (int i = 0; i < 5; ++i) {
        pointer[i].setX(pointer[i].x() + 50);
    }

    for (int i = 0; i < MAX_A2B_BUTTONS; ++i) {
        QLabel * label = new QLabel(this);
        LabelAdrList.append(label);     // delete?
        range_point[i].text_label = LabelAdrList[i];

        range_point[i].pointer_polygon << pointer[0] << pointer[1] << pointer[2] << pointer[3] << pointer[4];
        range_point[i].pointer_color = pointer_color_;
        range_point[i].is_pointer_drag = false;
        range_point[i].pointer_posX = offset_;

    }
}

AxesToButtonsSlider::~AxesToButtonsSlider()
{
    delete ui;
}
#include <QDebug>
void AxesToButtonsSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter;
    int width,tmp;

    width = this->width();
    tmp = width / 20;

    painter.begin(this);
    painter.setPen(Qt::lightGray);
    painter.drawRect(QRect(offset_, 7, width, 2));

    for (uint i = 0; i < 25; ++i){
        painter.drawLine((i * tmp) + offset_, 15, (i * tmp) + offset_, 18);
    }

    for (uint i = 0; i < buttons_count_; ++i){
        painter.setBrush(range_point[i].pointer_color);
        painter.setPen(range_point[i].pointer_color);
        painter.drawPolygon(range_point[i].pointer_polygon, Qt::WindingFill);
    }

    painter.end();
}

void AxesToButtonsSlider::MovePointer(uint pos_x, uint point_number)
{
    pointer[0].setX(pos_x-7);
    pointer[1].setX(pos_x-7);
    pointer[2].setX(pos_x-3);
    pointer[3].setX(pos_x);
    pointer[4].setX(pos_x);
    range_point[point_number].pointer_polygon.setPoint(0, pointer[0]);
    range_point[point_number].pointer_polygon.setPoint(1, pointer[1]);
    range_point[point_number].pointer_polygon.setPoint(2, pointer[2]);
    range_point[point_number].pointer_polygon.setPoint(3, pointer[3]);
    range_point[point_number].pointer_polygon.setPoint(4, pointer[4]);
}

void AxesToButtonsSlider::mouseMoveEvent(QMouseEvent *event)
{
    //MovePointer(event->pos().x(), 0);
    //update();

    for (uint i = 0; i < buttons_count_; ++i){
        if (range_point[i].pointer_polygon.containsPoint(event->pos(),Qt::WindingFill)) {
                range_point[i].pointer_color = Qt::black;
        } else range_point[i].pointer_color = QColor(1,119,215);

        if (event->buttons() & Qt::LeftButton)
            DrawPoint(event->pos(), i);
                    //for (int i = 0; i < 5; ++i) {
                        //range_points[i].pointer_polygon        //setX(pointer[i].x() + 50);
                    //}

        if (range_point[i].is_pointer_drag) {
            range_point[i].pointer_color=Qt::lightGray;
        }
    }
    update();
}

void AxesToButtonsSlider::DrawPoint(QPoint point_pos, uint range_point_number) {
    uint point_value = 0;

//    if ((point_pos.x() < offset_) || point_pos.x() > this->width() + offset_) {
//        return;
//    }

    //if ((range_point[range_point_number].is_pointer_drag) && point_pos.x() < (this->width() + offset_)) {
        //point_value = checkPinValue(point_pos.x(), range_point_number);
        //MovePointer(point_value, range_point_number);
        MovePointer(point_pos.x(), range_point_number);
        range_point[range_point_number].pointer_posX = point_pos.x();//point_value;
        //setLableValue(range_point_number);
        return;
    //}
}
