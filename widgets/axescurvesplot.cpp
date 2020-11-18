#include "axescurvesplot.h"
#include "ui_axescurvesplot.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
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
    half_radius_ = kRadius_/2;

    // если вкладка не открыта, то размер не загружается и высота = 0. т.к. при изменении размера окна у меня не меняется высота,
    // то захардкодил 450, но это херовое решение
    height_ = 450;
    this->setMinimumHeight(height_);

    cur_axis_pos_.color = kPointCurrentPosColor_;

    int tmp_range;
    if ((kMinPointValue_ < 0 && kMaxPointValue_ < 0) || (kMinPointValue_ >= 0 && kMaxPointValue_ >= 0)) {
        tmp_range = kMaxPointValue_ - kMinPointValue_;
    }
    else {
        tmp_range = abs(kMinPointValue_) + abs(kMaxPointValue_);
    }

    for (int i = 0; i < points_count_; ++i){
        AxesCurve_point* point = new AxesCurve_point;

        QLabel* label = new QLabel(this);
        LabelPtrList_.append(label);

        PointPtrList_.append(point);
        PointPtrList_[i]->color = kPointInactiveColor_;
        PointPtrList_[i]->posX = 0;
        PointPtrList_[i]->posY = 0;
        PointPtrList_[i]->is_drag = false;
        PointPtrList_[i]->current_value = ((tmp_range / (points_count_ - 1.0)) * i) + kMinPointValue_;    // hz

        LabelPtrList_[i]->setFont(QFont("MS Shell Dlg 2", 8));
        LabelPtrList_[i]->setNum(PointPtrList_[i]->current_value);
        LabelPtrList_[i]->setMinimumWidth(kLabelWidth_);
        LabelPtrList_[i]->setAlignment(Qt::AlignHCenter);
        LabelPtrList_[i]->setStyleSheet("background:transparent;");
        LabelPtrList_[i]->setVisible(true);
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
        for (int i = 0; i < kColumnsCount_ + 1; ++i){
            tmp_x = (i * column_width_) + kOffset_;
            painter.drawLine(tmp_x, kOffset_, tmp_x, height_ - kOffset_);
        }
        // paint rows
        for (int i = 0; i < kRowsCount_ + 1; ++i){
            tmp_y = (i * row_height_) + kOffset_;
            painter.drawLine(kOffset_, tmp_y, width_ - kOffset_, tmp_y);
        }
//        resize_activated_ = false;
//    }


    // Antialiasing         // можно светху но всё станет мазнёй
    painter.setRenderHint(QPainter::Antialiasing, true);
    // paint line       // можно в цикл paint rect, но лучше отдельно
    QPen pen;
    pen.setWidth(2);
    pen.setColor(kPointInactiveColor_);
    for (int i = 0; i < PointPtrList_.size(); ++i)   // проверка на драг=труе + соседние
    {
        painter.setPen(pen);
        if (i < PointPtrList_.size() - 1){
            painter.drawLine(PointPtrList_[i]->posX  + half_radius_, PointPtrList_[i]->posY  + half_radius_,
                             PointPtrList_[i + 1]->posX  + half_radius_, PointPtrList_[i + 1]->posY  + half_radius_);
        }
    }

    // coordinates for rect
    for (int i = 0; i < PointPtrList_.size(); ++i) {
        PointPtrList_[i]->area.setRect(PointPtrList_[i]->posX, PointPtrList_[i]->posY,
                                      kRadius_, kRadius_);
    }
    // paint rect
    //painter.setPen(Qt::lightGray);
    for (int i = 0; i < PointPtrList_.size(); ++i)   // проверка на драг=труе
    {
        painter.setPen(PointPtrList_[i]->color);
        painter.setBrush(PointPtrList_[i]->color);
        painter.drawEllipse(PointPtrList_[i]->area);

        painter.setPen(kPointMoveColor_);
        painter.setBrush(kPointMoveColor_);
        painter.drawEllipse(PointPtrList_[i]->area.x() + half_radius_/2, PointPtrList_[i]->area.y() + half_radius_/2,
                            half_radius_, half_radius_);
    }

    // paint current axix position
    if (is_device_connect_ == true)
    {
        // coordinates for rect
        cur_axis_pos_.area.setRect(cur_axis_pos_.posX, cur_axis_pos_.posY,
                                  kRadius_, kRadius_);
        // paint rect
        painter.setPen(cur_axis_pos_.color);
        painter.setBrush(cur_axis_pos_.color);
        painter.drawEllipse(cur_axis_pos_.area);
    }

    painter.end();
}

void AxesCurvesPlot::UpdateAxis(int pos_x, int pos_y)
{
    cur_axis_pos_.posX = CalcPointPosX(pos_x);   // увеличить Х
    cur_axis_pos_.posY = CalcPointPos(pos_y);
    update();
}


int AxesCurvesPlot::GetPointValue(int point_number)
{
    return PointPtrList_[point_number]->current_value = CalcPointValue(PointPtrList_[point_number]->posY);
}

int AxesCurvesPlot::GetPointCount() const
{
    return points_count_;
}

void AxesCurvesPlot::SetPointValue(int point_number, int value)
{
    PointPtrList_[point_number]->posY = CalcPointPos(value);
    PointPtrList_[point_number]->current_value = value;
    LabelPtrList_[point_number]->setNum(value);
    UpdateLabelPos();
    update();
}

void AxesCurvesPlot::SetLinear()
{
    int tmp_y;
    for (int i = 0; i < PointPtrList_.size(); ++i){
        tmp_y = (i * row_height_) + kOffset_;
        PointPtrList_[(PointPtrList_.size() - 1) - i]->posY = tmp_y - half_radius_;
        PointPtrList_[(PointPtrList_.size() - 1) - i]->current_value = CalcPointValue(PointPtrList_[(PointPtrList_.size() - 1) - i]->posY);
        LabelPtrList_[(PointPtrList_.size() - 1) - i]->setNum(PointPtrList_[(PointPtrList_.size() - 1) - i]->current_value);
    }
    UpdateLabelPos();
    for (int i = 0; i < PointPtrList_.size(); ++i) {
        emit pointValueChanged(&i, &PointPtrList_[i]->current_value);
    }
    update();
}

void AxesCurvesPlot::SetLinearInvert()
{
    int tmp_y;
    for (int i = 0; i < PointPtrList_.size(); ++i){
        tmp_y = (i * row_height_) + kOffset_;
        PointPtrList_[i]->posY = tmp_y - half_radius_;
        PointPtrList_[i]->current_value = CalcPointValue(PointPtrList_[i]->posY);
        LabelPtrList_[i]->setNum(PointPtrList_[i]->current_value);
    }
    UpdateLabelPos();
    for (int i = 0; i < PointPtrList_.size(); ++i) {
        emit pointValueChanged(&i, &PointPtrList_[i]->current_value);
    }
    update();
}

void AxesCurvesPlot::SetExponent()
{
    int tmp_range;
    if ((kMinPointValue_ < 0 && kMaxPointValue_ < 0) || (kMinPointValue_ >= 0 && kMaxPointValue_ >= 0)) {
        tmp_range = kMaxPointValue_ - kMinPointValue_;
    }
    else {
        tmp_range = abs(kMinPointValue_) + abs(kMaxPointValue_);
    }

    int tmp_value;
    for (int i = 0; i < PointPtrList_.size(); ++i)
    {
        tmp_value = round(exp(i * log(tmp_range) / (points_count_ - 1)) + kMinPointValue_);
        if (tmp_value <= kMinPointValue_ +1){
            tmp_value = kMinPointValue_;
        } else if (tmp_value >= kMaxPointValue_ -1){
            tmp_value = kMaxPointValue_;
        }
        SetPointValue(i, tmp_value);
    }
    UpdateLabelPos();
    for (int i = 0; i < PointPtrList_.size(); ++i) {
        emit pointValueChanged(&i, &PointPtrList_[i]->current_value);
    }
}

void AxesCurvesPlot::SetExponentInvert()
{
    int tmp_range;
    if ((kMinPointValue_ < 0 && kMaxPointValue_ < 0) || (kMinPointValue_ >= 0 && kMaxPointValue_ >= 0)) {
        tmp_range = kMaxPointValue_ - kMinPointValue_;
    }
    else {
        tmp_range = abs(kMinPointValue_) + abs(kMaxPointValue_);
    }

    int tmp_value;
    for (int i = 0; i < PointPtrList_.size(); ++i)
    {
        tmp_value = round(exp(i * log(tmp_range) / (points_count_ - 1)) + kMinPointValue_);
        if (tmp_value <= kMinPointValue_ +1){
            tmp_value = kMinPointValue_;
        } else if (tmp_value >= kMaxPointValue_ -1){
            tmp_value = kMaxPointValue_;
        }
        SetPointValue((points_count_ - 1) - i, tmp_value);
    }
    UpdateLabelPos();
    for (int i = 0; i < PointPtrList_.size(); ++i) {
        emit pointValueChanged(&i, &PointPtrList_[i]->current_value);
    }
}

void AxesCurvesPlot::SetShape()
{
    if (PointPtrList_.size() >= 11)
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
    for (int i = 0; i < PointPtrList_.size(); ++i) {
        emit pointValueChanged(&i, &PointPtrList_[i]->current_value);
    }
}

void AxesCurvesPlot::UpdateLabelPos()
{
    for (int i = 0; i < LabelPtrList_.size(); ++i) {
        LabelPtrList_[i]->move(PointPtrList_[i]->posX, PointPtrList_[i]->posY + kRadius_);
    }
}

int AxesCurvesPlot::CalcPointValue(int current_pos) const     // ужоснах
{
    int value = 0;
    float half_height = (height_) / 2.0 - kOffset_;
    //current_pos -= - half_radius_;
    current_pos = current_pos - 13;     // костыль
    //qDebug()<<"height"<<height_;
    if (current_pos > half_height){
        float tmp_min = half_height / kMinPointValue_;
        value = floor((current_pos - half_height) / float(tmp_min));
        //qDebug()<<"value ="<<value;
        return value;
    } else {
        float tmp_max = half_height / kMaxPointValue_;
        value = -((current_pos - half_height) / float(tmp_max));
        //qDebug()<<"value ="<<value;
        return value;
    }
    //qDebug()<<"value"<<value;
    return value;
}
     // ужоснах
int AxesCurvesPlot::CalcPointPos(int value) const     // хз, центр посередине, а дальше скейлится, в зависимости от значений min и max
{                                               // наверно лучше потом сделать центр в зависимости от значений min и max
    int pos = 0;
    float half_height = (height_) / 2.0 - kOffset_;    // отдельной переменной для оптимизации?

    if (value < kMaxPointValue_ - kMinPointValue_){
        float tmp_min = half_height / kMinPointValue_;      // эта функция запускается до инииализации height_, неопределённое поведение
        pos = round(value * tmp_min + half_height);
    } else {
        float tmp_max = half_height / kMaxPointValue_;
        pos = round(value * tmp_max + half_height);
    }

    return pos + 12;        // костыль
}

int AxesCurvesPlot::CalcPointPosX(int value_x) const     // хз, центр посередине, а дальше скейлится, в зависимости от значений min и max
{                                               // наверно лучше потом сделать центр в зависимости от значений min и max
    int pos = 0;
    float half_width = (width_) / 2.0 - kOffset_;    // отдельной переменной для оптимизации?


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

    column_width_ = (width_ - kOffset_*2) / float(kColumnsCount_);
    row_height_ = (height_ - kOffset_*2) / float(kRowsCount_);


    for (int i = 0; i < PointPtrList_.size(); ++i){
        tmp_x = (i * column_width_) + kOffset_;
        PointPtrList_[i]->posX = tmp_x - half_radius_;      // temp
        PointPtrList_[i]->posY = CalcPointPos(PointPtrList_[i]->current_value);
        UpdateLabelPos();
    }
    for (int i = 0; i < PointPtrList_.size(); ++i){
        //////tmp_y = (i * row_height_) + offset_;
        //PointAdrList[i]->posY = CalcPointPos(PointAdrList[i]->current_value);//tmp_y - radius_/2;      // temp
    }
//    for (int i = 0; i < rows_count_ + 1; ++i){
//        tmp_y = (i * row_height_) + offset_;
//        PointAdrList[i]->posY = tmp_y - radius_/2;      // temp
//    }

    // size for a2b
    //int x = this->geometry().x();
    int width = width_-kOffset_;
    emit sizeChanged(width);
}

void AxesCurvesPlot::mouseMoveEvent(QMouseEvent *event)
{
    // оптимизировать, добавить проверку только по columns line + radius/2 ?
    for (int i = 0; i < PointPtrList_.size(); ++i)
    {
        if (PointPtrList_[i]->is_drag == false){
            if (PointPtrList_[i]->area.contains(event->pos()))   // наверх
            {
                PointPtrList_[i]->color = kPointActiveColor_;
                point_active_ = true;       // ????
                update();
                break;  //?
            } else if (point_active_ == true){
                PointPtrList_[i]->color = kPointInactiveColor_;
                update();
                //break;  //?
            }
        }
        else if (PointPtrList_[i]->is_drag == true){     // if nah

            if (event->pos().y() < kOffset_ || event->pos().y() > this->height() - kOffset_) {        // event->pos().y() сделать переменную для оптимизации?
                return;
            }

            if (event->buttons() & Qt::LeftButton){
                PointPtrList_[i]->posY = event->pos().y() - half_radius_;   // half_radius_ - убрать ненужные во всём коде
                PointPtrList_[i]->color = kPointMoveColor_;

                PointPtrList_[i]->current_value = CalcPointValue(PointPtrList_[i]->posY);
                LabelPtrList_[i]->setNum(PointPtrList_[i]->current_value);
                UpdateLabelPos();
                update();
                emit pointValueChanged(&i, &PointPtrList_[i]->current_value);
//                qDebug()<<"event->pos().y()"<<event->pos().y();
//                qDebug()<<"value"<<PointAdrList[i]->current_value;
                //CalcPointValue(event->pos().y());
            }
            break;
        }
    }
    //update();
}

void AxesCurvesPlot::mousePressEvent(QMouseEvent *event)
{
    for (int i = 0; i < PointPtrList_.size(); ++i){
        if (PointPtrList_[i]->area.contains(event->pos())) {
            PointPtrList_[i]->is_drag = true;
            break;
        }
    }
}
void AxesCurvesPlot::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    for (int i = 0; i < PointPtrList_.size(); ++i){
        if(PointPtrList_[i]->is_drag == true){
            PointPtrList_[i]->is_drag = false;
            PointPtrList_[i]->color = kPointInactiveColor_;
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

