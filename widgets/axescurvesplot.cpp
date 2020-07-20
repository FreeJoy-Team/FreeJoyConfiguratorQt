#include "axescurvesplot.h"
#include "ui_axescurvesplot.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

#include <QDebug>












//////////////// ЭТО НЕ КОД ЭТО ПИЗДЕЦ !!!!!!!!!!
/// ПЕРЕДАЛАТЬ И УБРАТЬ МУСОР
/// ///////////////////////////////////////////////////// ПЕРЕД СНОМ НЕ СМОТРЕТЬ /////////////////////////////////////////////////














AxesCurvesPlot::AxesCurvesPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurvesPlot)
{
    ui->setupUi(this);
    setMouseTracking(true);

    points_count_ = 11;
    point_active_ = false;
    half_radius_ = radius_/2;

    int tmp_range;
    if ((min_point_value < 0 && max_point_value < 0) || (min_point_value >= 0 && max_point_value >= 0)) {
        tmp_range = max_point_value - min_point_value;
    }
    else {
        tmp_range = abs(min_point_value) + abs(max_point_value);
    }

    for (int i = 0; i < points_count_; ++i){
        AxesCurve_point* point = new AxesCurve_point;
        PointAdrList.append(point);
        PointAdrList[i]->color = point_inactive_color_;
        PointAdrList[i]->posX = 0;
        PointAdrList[i]->posY = 0;
        PointAdrList[i]->is_drag = false;
        PointAdrList[i]->current_value = ((tmp_range / (points_count_ - 1.0)) * i) + min_point_value;    // hz
    }

}

AxesCurvesPlot::~AxesCurvesPlot()
{
    delete ui;
}
        ////////////////// half_radius_ - убрать                ///                 /// half_radius_
        /// сделать SetPointValue в % ?

void AxesCurvesPlot::paintEvent(QPaintEvent *event)     // жирно, можно оптимизировать
{
    Q_UNUSED(event)
    QPainter painter;
    int tmp_x, tmp_y;

    painter.begin(this);
    painter.setPen(Qt::lightGray);

//    if (resize_activated_ == true){
//        qDebug()<<"resize";
        //paint columns
        for (int i = 0; i < columns_count_ + 1; ++i){
            tmp_x = (i * column_width_) + offset_;
            painter.drawLine(tmp_x, offset_, tmp_x, height_ - offset_);
        }
        // paint rows
        for (int i = 0; i < rows_count_ + 1; ++i){
            tmp_y = (i * row_height_) + offset_;
            painter.drawLine(offset_, tmp_y, width_ - offset_, tmp_y);
        }
//        resize_activated_ = false;
//    }


    // Antialiasing         // можно светху но всё станет мазнёй
    painter.setRenderHint(QPainter::Antialiasing, true);
    // paint line       // можно в цикл paint rect, но лучше отдельно
    QPen pen;
    pen.setWidth(2);
    pen.setColor(point_inactive_color_);
    for (int i = 0; i < PointAdrList.size(); ++i)   // проверка на драг=труе + соседние
    {
        painter.setPen(pen);
        if (i < PointAdrList.size() - 1){
            painter.drawLine(PointAdrList[i]->posX  + half_radius_, PointAdrList[i]->posY  + half_radius_,
                             PointAdrList[i + 1]->posX  + half_radius_, PointAdrList[i + 1]->posY  + half_radius_);
        }
    }

    // coordinates for rect
    for (int i = 0; i < PointAdrList.size(); ++i) {
        PointAdrList[i]->area.setRect(PointAdrList[i]->posX, PointAdrList[i]->posY,
                                      radius_, radius_);
    }
    // paint rect
    //painter.setPen(Qt::lightGray);
    for (int i = 0; i < PointAdrList.size(); ++i)   // проверка на драг=труе
    {
        painter.setPen(PointAdrList[i]->color);
        painter.setBrush(PointAdrList[i]->color);
        painter.drawEllipse(PointAdrList[i]->area);

        painter.setPen(point_move_color_);
        painter.setBrush(point_move_color_);
        painter.drawEllipse(PointAdrList[i]->area.x() + half_radius_/2, PointAdrList[i]->area.y() + half_radius_/2,
                            half_radius_, half_radius_);
    }

    painter.end();
}


int AxesCurvesPlot::GetPointValue(int point_number)
{
    return PointAdrList[point_number]->current_value = CalcPointValue(PointAdrList[point_number]->posY);
}

int AxesCurvesPlot::GetPointCount()
{
    return points_count_;
}

void AxesCurvesPlot::SetPointValue(int value, int point_number)
{
    PointAdrList[point_number]->posY = CalcPointPos(value);
    PointAdrList[point_number]->current_value = value;
    update();
}

void AxesCurvesPlot::SetLinear()
{
    int tmp_y;
    for (int i = 0; i < PointAdrList.size(); ++i){
        tmp_y = (i * row_height_) + offset_;
        PointAdrList[(PointAdrList.size() - 1) - i]->posY = tmp_y - half_radius_;
    }
    update();
}

void AxesCurvesPlot::SetLinearInvert()
{
    int tmp_y;
    for (int i = 0; i < PointAdrList.size(); ++i){
        tmp_y = (i * row_height_) + offset_;
        PointAdrList[i]->posY = tmp_y - half_radius_;
    }
    update();
}

void AxesCurvesPlot::SetExponent()
{
    int tmp_range;
    if ((min_point_value < 0 && max_point_value < 0) || (min_point_value >= 0 && max_point_value >= 0)) {
        tmp_range = max_point_value - min_point_value;
    }
    else {
        tmp_range = abs(min_point_value) + abs(max_point_value);
    }

    for (int i = 0; i < PointAdrList.size(); ++i) {
        SetPointValue(exp(i * log(tmp_range) / (points_count_ - 1)) + min_point_value, i);
    }
}

void AxesCurvesPlot::SetExponentInvert()
{
    int tmp_range;
    if ((min_point_value < 0 && max_point_value < 0) || (min_point_value >= 0 && max_point_value >= 0)) {
        tmp_range = max_point_value - min_point_value;
    }
    else {
        tmp_range = abs(min_point_value) + abs(max_point_value);
    }

    for (int i = 0; i < PointAdrList.size(); ++i) {
        SetPointValue(exp(i * log(tmp_range) / (points_count_ - 1)) + min_point_value, (points_count_ - 1) - i);
    }
}

void AxesCurvesPlot::SetShape()     // руками так се, надо бы автоматом сделать
{
    if (PointAdrList.size() >= 11)
    {
        SetPointValue(-100, 0);
        SetPointValue(-60, 1);
        SetPointValue(-20, 2);
        SetPointValue(-6, 3);
        SetPointValue(-2, 4);
        SetPointValue(0, 5);
        SetPointValue(2, 6);
        SetPointValue(6, 7);
        SetPointValue(20, 8);
        SetPointValue(60, 9);
        SetPointValue(100, 10);
    }
}



int AxesCurvesPlot::CalcPointValue(int current_pos)
{
    int value = 0;
    float half_height = (height_) / 2.0 - offset_;    // отдельной переменной для оптимизации?
    current_pos -= half_radius_;

    if (current_pos > half_height){
        float tmp_min = half_height / min_point_value;
        value = round((current_pos - half_height) / float(tmp_min));
        //qDebug()<<"value ="<<value;
        return value;
    } else {
        float tmp_max = half_height / max_point_value;
        value = -round((current_pos - half_height) / float(tmp_max));
        //qDebug()<<"value ="<<value;
        return value;
    }
    return value;
}

int AxesCurvesPlot::CalcPointPos(int value)     // хз, центр посередине, а дальше скейлится, в зависимости от значений min и max
{                                               // наверно лучше потом сделать центр в зависимости от значений min и max
    int pos = 0;
    float half_height = (height_) / 2.0 - offset_;    // отдельной переменной для оптимизации?

    if (value < max_point_value - min_point_value){
        float tmp_min = half_height / min_point_value;      // эта функция запускается до инииализации height_, неопределённое поведение
        pos = round(value * tmp_min + half_height);
    } else {
        float tmp_max = half_height / max_point_value;
        pos = round(value * tmp_max + half_height);
    }

    return pos + half_radius_;
}



void AxesCurvesPlot::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)

    int tmp_x;//, tmp_y;
    width_ = this->width();
    height_ = this->height();

    column_width_ = (width_ - offset_*2) / float(columns_count_);
    row_height_ = (height_ - offset_*2) / float(rows_count_);


    for (int i = 0; i < PointAdrList.size(); ++i){
        tmp_x = (i * column_width_) + offset_;
        PointAdrList[i]->posX = tmp_x - half_radius_;      // temp
    }
    for (int i = 0; i < PointAdrList.size(); ++i){
        //tmp_y = (i * row_height_) + offset_;
        PointAdrList[i]->posY = CalcPointPos(PointAdrList[i]->current_value);//tmp_y - radius_/2;      // temp
        //qDebug()<<"value"<<i<<PointAdrList[i]->current_value;
    }
//    for (int i = 0; i < rows_count_ + 1; ++i){
//        tmp_y = (i * row_height_) + offset_;
//        PointAdrList[i]->posY = tmp_y - radius_/2;      // temp
//    }

}

void AxesCurvesPlot::mouseMoveEvent(QMouseEvent *event)
{
    // оптимизировать, добавить проверку только по columns line + radius/2 ?
    for (int i = 0; i < PointAdrList.size(); ++i)
    {
        if (PointAdrList[i]->is_drag == false){
            if (PointAdrList[i]->area.contains(event->pos()))   // наверх
            {
                PointAdrList[i]->color = point_active_color_;
                point_active_ = true;       // ????
                update();
                break;  //?
            } else if (point_active_ == true){
                PointAdrList[i]->color = point_inactive_color_;
                update();
                //break;  //?
            }
        }
        else if (PointAdrList[i]->is_drag == true){     // if nah

            if (event->pos().y() < offset_ || event->pos().y() > this->height() - offset_) {
                return;
            }

            if (event->buttons() & Qt::LeftButton){
                PointAdrList[i]->posY = event->pos().y() - half_radius_;   // half_radius_ - убрать ненужные во всём коде
                PointAdrList[i]->color = point_move_color_;

                PointAdrList[i]->current_value = CalcPointValue(PointAdrList[i]->posY);
                update();
                qDebug()<<"event->pos().y()"<<event->pos().y();
                qDebug()<<"value"<<PointAdrList[i]->current_value;
                //CalcPointValue(event->pos().y());
            }
            break;
        }
    }
    //update();
}

void AxesCurvesPlot::mousePressEvent(QMouseEvent *event)
{
    for (int i = 0; i < PointAdrList.size(); ++i){
        if (PointAdrList[i]->area.contains(event->pos())) {
            PointAdrList[i]->is_drag = true;
            break;
        }
    }
}
void AxesCurvesPlot::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    for (int i = 0; i < PointAdrList.size(); ++i){
        if(PointAdrList[i]->is_drag == true){
            PointAdrList[i]->is_drag = false;
            PointAdrList[i]->color = point_inactive_color_;
            break;
        }
    }
    update();
}

