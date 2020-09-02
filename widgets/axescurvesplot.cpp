#include "axescurvesplot.h"
#include "ui_axescurvesplot.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

#include <QDebug>






#include <QTimer>





//////////////// ЭТО НЕ КОД ЭТО ПИЗДЕЦ !!!!!!!!!!
/// ПЕРЕДАЛАТЬ И УБРАТЬ МУСОР
/// ///////////////////////////////////////////////////// ПЕРЕД СНОМ НЕ СМОТРЕТЬ УЖОСНАХ /////////////////////////////////////////////////













AxesCurvesPlot::AxesCurvesPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurvesPlot)
{
    ui->setupUi(this);
    setMouseTracking(true);

    points_count_ = CURVE_PLOT_POINTS_COUNT; // axescurvesconfig
    point_active_ = false;
    is_device_connect_ = false;
    half_radius_ = radius_/2;

    // если вкладка не открыта, то размер не загружается и высота = 0. т.к. при изменении размера окна у меня не меняется высота,
    // то захардкодил 450, но это херовое решение
    height_ = 450;
    this->setMinimumHeight(height_);

    cur_axis_pos.color = point_current_pos_color_;

    int tmp_range;
    if ((min_point_value < 0 && max_point_value < 0) || (min_point_value >= 0 && max_point_value >= 0)) {
        tmp_range = max_point_value - min_point_value;
    }
    else {
        tmp_range = abs(min_point_value) + abs(max_point_value);
    }

    for (int i = 0; i < points_count_; ++i){
        AxesCurve_point* point = new AxesCurve_point;

        QLabel* label = new QLabel(this);
        LabelAdrList.append(label);

        PointAdrList.append(point);
        PointAdrList[i]->color = point_inactive_color_;
        PointAdrList[i]->posX = 0;
        PointAdrList[i]->posY = 0;
        PointAdrList[i]->is_drag = false;
        PointAdrList[i]->current_value = ((tmp_range / (points_count_ - 1.0)) * i) + min_point_value;    // hz

        LabelAdrList[i]->setFont(QFont("MS Shell Dlg 2", 8));
        LabelAdrList[i]->setNum(PointAdrList[i]->current_value);
        LabelAdrList[i]->setMinimumWidth(label_width_);
        LabelAdrList[i]->setAlignment(Qt::AlignHCenter);
        LabelAdrList[i]->setVisible(true);
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

    // paint current axix position
    if (is_device_connect_ == true)
    {
        // coordinates for rect
        for (int i = 0; i < 1; ++i) {
            cur_axis_pos.area.setRect(cur_axis_pos.posX, cur_axis_pos.posY,
                                          radius_, radius_);
        }
        // paint rect
        //painter.setPen(Qt::lightGray);
        for (int i = 0; i < 1; ++i)
        {
            painter.setPen(cur_axis_pos.color);
            painter.setBrush(cur_axis_pos.color);
            painter.drawEllipse(cur_axis_pos.area);
        }
    }

    painter.end();
}

void AxesCurvesPlot::UpdateAxis(int pos_x, int pos_y)
{
//    static bool timer_enabled = false;
//    static int count = 0;
//    static int tmp;
//    count++;
//    time_t start = clock();

//    if (timer_enabled == false){
//        timer_enabled = true;

//      tmp = count;
//        QTimer::singleShot(200, [&]{
//            timer_enabled = false;
//            if (clock() - start > 20){
//                is_device_connect_ = false;
//                qDebug()<<"is_device_connect_ false";
//                update();

//            } else {
//                is_device_connect_ = true;
//            }
//        });
//    }
    //is_device_connect_ = true;
    cur_axis_pos.posX = CalcPointPosX(pos_x);
    cur_axis_pos.posY = CalcPointPos(pos_y);
    qDebug()<<"cur_axis_pos.posX ="<<cur_axis_pos.posX;
    update();
}


int AxesCurvesPlot::GetPointValue(int point_number)
{
    return PointAdrList[point_number]->current_value = CalcPointValue(PointAdrList[point_number]->posY);
}

int AxesCurvesPlot::GetPointCount()
{
    return points_count_;
}

void AxesCurvesPlot::SetPointValue(int point_number, int value)
{
    PointAdrList[point_number]->posY = CalcPointPos(value);
    PointAdrList[point_number]->current_value = value;
    LabelAdrList[point_number]->setNum(value);
    UpdateLabelPos();
    update();
    //qDebug()<<"posY"<<PointAdrList[point_number]->posY;
}

void AxesCurvesPlot::SetLinear()
{
    int tmp_y;
    for (int i = 0; i < PointAdrList.size(); ++i){
        tmp_y = (i * row_height_) + offset_;
        PointAdrList[(PointAdrList.size() - 1) - i]->posY = tmp_y - half_radius_;
        PointAdrList[(PointAdrList.size() - 1) - i]->current_value = CalcPointValue(PointAdrList[(PointAdrList.size() - 1) - i]->posY);
        LabelAdrList[(PointAdrList.size() - 1) - i]->setNum(PointAdrList[(PointAdrList.size() - 1) - i]->current_value);
    }
    UpdateLabelPos();
    update();
}

void AxesCurvesPlot::SetLinearInvert()
{
    int tmp_y;
    for (int i = 0; i < PointAdrList.size(); ++i){
        tmp_y = (i * row_height_) + offset_;
        PointAdrList[i]->posY = tmp_y - half_radius_;
        PointAdrList[i]->current_value = CalcPointValue(PointAdrList[i]->posY);
        LabelAdrList[i]->setNum(PointAdrList[i]->current_value);
    }
    UpdateLabelPos();
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

    int tmp_value;
    for (int i = 0; i < PointAdrList.size(); ++i)
    {
        tmp_value = round(exp(i * log(tmp_range) / (points_count_ - 1)) + min_point_value);
        if (tmp_value <= min_point_value +1){
            tmp_value = min_point_value;
        } else if (tmp_value >= max_point_value -1){
            tmp_value = max_point_value;
        }
        SetPointValue(i, tmp_value);
    }
    UpdateLabelPos();
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

    int tmp_value;
    for (int i = 0; i < PointAdrList.size(); ++i)
    {
        tmp_value = round(exp(i * log(tmp_range) / (points_count_ - 1)) + min_point_value);
        if (tmp_value <= min_point_value +1){
            tmp_value = min_point_value;
        } else if (tmp_value >= max_point_value -1){
            tmp_value = max_point_value;
        }
        SetPointValue((points_count_ - 1) - i, tmp_value);
    }
    UpdateLabelPos();
}

void AxesCurvesPlot::SetShape()     // руками так се, надо бы автоматом сделать
{
    if (PointAdrList.size() >= 11)
    {
        SetPointValue(0, -100);
        SetPointValue(1, -60);
        SetPointValue(2, -20);
        SetPointValue(3, -6);
        SetPointValue(4, -2);
        SetPointValue(5, 0);
        SetPointValue(6, 2);
        SetPointValue(7, 6);
        SetPointValue(8, 20);
        SetPointValue(9, 60);
        SetPointValue(10, 100);
    }
    UpdateLabelPos();
}

void AxesCurvesPlot::UpdateLabelPos()
{
    for (int i = 0; i < LabelAdrList.size(); ++i) {
        LabelAdrList[i]->move(PointAdrList[i]->posX, PointAdrList[i]->posY + radius_);
    }
}

int AxesCurvesPlot::CalcPointValue(int current_pos)     // ужоснах
{
    int value = 0;
    float half_height = (height_) / 2.0 - offset_;
    //current_pos -= - half_radius_;
    current_pos = current_pos - 13;     // костыль
    //qDebug()<<"height"<<height_;
    if (current_pos > half_height){
        float tmp_min = half_height / min_point_value;
        value = floor((current_pos - half_height) / float(tmp_min));
        //qDebug()<<"value ="<<value;
        return value;
    } else {
        float tmp_max = half_height / max_point_value;
        value = -((current_pos - half_height) / float(tmp_max));
        //qDebug()<<"value ="<<value;
        return value;
    }
    //qDebug()<<"value"<<value;
    return value;
}
     // ужоснах
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

    return pos + 12;        // костыль
}

int AxesCurvesPlot::CalcPointPosX(int value_x)     // хз, центр посередине, а дальше скейлится, в зависимости от значений min и max
{                                               // наверно лучше потом сделать центр в зависимости от значений min и max
    int pos = 0;
    float half_width = (width_) / 2.0 - offset_;    // отдельной переменной для оптимизации?


//    if (value_x < 50){
//        value_x += value_x;
//    } else {
//        value_x += value_x;
//        //value_x = value_x * -1;
//    }
        float tmp_max = half_width / 100;
        pos = round(value_x * tmp_max);




//    if (value_x < max_point_value - min_point_value){
//        float tmp_min = half_width / min_point_value;      // эта функция запускается до инииализации height_, неопределённое поведение
//        pos = round(value_x * tmp_min + half_width);
//    } else {
//        float tmp_max = half_width / max_point_value;
//        pos = round(value_x * tmp_max + half_width);
//    }

    return pos + 12;        // костыль
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
        PointAdrList[i]->posY = CalcPointPos(PointAdrList[i]->current_value);
        UpdateLabelPos();
    }
    for (int i = 0; i < PointAdrList.size(); ++i){
        //////tmp_y = (i * row_height_) + offset_;
        //PointAdrList[i]->posY = CalcPointPos(PointAdrList[i]->current_value);//tmp_y - radius_/2;      // temp
    }
//    for (int i = 0; i < rows_count_ + 1; ++i){
//        tmp_y = (i * row_height_) + offset_;
//        PointAdrList[i]->posY = tmp_y - radius_/2;      // temp
//    }

    // size for a2b
    //int x = this->geometry().x();
    int width = width_-offset_;
    emit sizeChanged(width);
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

            if (event->pos().y() < offset_ || event->pos().y() > this->height() - offset_) {        // event->pos().y() сделать переменную для оптимизации?
                return;
            }

            if (event->buttons() & Qt::LeftButton){
                PointAdrList[i]->posY = event->pos().y() - half_radius_;   // half_radius_ - убрать ненужные во всём коде
                PointAdrList[i]->color = point_move_color_;

                PointAdrList[i]->current_value = CalcPointValue(PointAdrList[i]->posY);
                LabelAdrList[i]->setNum(PointAdrList[i]->current_value);
                UpdateLabelPos();
                update();
                emit pointValueChanged(&i, &PointAdrList[i]->current_value);
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

void AxesCurvesPlot::DeviceStatus(bool is_connect)
{
    is_device_connect_ = is_connect;
    update();
}

