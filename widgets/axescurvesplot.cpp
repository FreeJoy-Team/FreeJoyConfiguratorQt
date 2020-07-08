#include "axescurvesplot.h"
#include "ui_axescurvesplot.h"
#include <QPainter>
#include <QMouseEvent>

AxesCurvesPlot::AxesCurvesPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurvesPlot)
{
    ui->setupUi(this);
    setMouseTracking(true);

    points_count_ = 11;

    for (int i = 0; i < points_count_; ++i){
        AxesCurve_point* point = new AxesCurve_point;
        PointAdrList.append(point);
        PointAdrList[i]->color = point_inactive_color_;
        PointAdrList[i]->posX = 0;
        PointAdrList[i]->posY = 0;
        PointAdrList[i]->is_drag = false;
    }

}
#include <QDebug>
AxesCurvesPlot::~AxesCurvesPlot()
{
    delete ui;
}

void AxesCurvesPlot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter;
    int width, height;
    int tmp_x, tmp_y;
    float tmp_column_width, tmp_row_height;

    width = this->width();
    height = this->height();

    tmp_column_width = (width - offset_*2) / float(columns_count_);
    tmp_row_height = (height - offset_*2) / float(rows_count_);

    painter.begin(this);
    painter.setPen(Qt::lightGray);

    //paint columns
    for (int i = 0; i < columns_count_ + 1; ++i){
        tmp_x = (i * tmp_column_width) + offset_;
        painter.drawLine(tmp_x, offset_, tmp_x, height - offset_);
    }
    // paint rows
    for (int i = 0; i < rows_count_ + 1; ++i){
        tmp_y = (i * tmp_row_height) + offset_;
        painter.drawLine(offset_, tmp_y, width - offset_, tmp_y);
    }
    // coordinates for rect
    for (int i = 0; i < PointAdrList.size(); ++i) {
        PointAdrList[i]->area.setRect(PointAdrList[i]->posX, PointAdrList[i]->posY,
                                      radius_, radius_);
    }
    // paint rect
    for (int i = 0; i < PointAdrList.size(); ++i)
    {
        painter.setBrush(PointAdrList[i]->color);
        painter.drawEllipse(PointAdrList[i]->area);
        painter.setBrush(point_move_color_);
        painter.drawEllipse(PointAdrList[i]->area.x() + radius_/4, PointAdrList[i]->area.y() + radius_/4,
                            radius_/2, radius_/2);
    }
    // Antialiasing         // можно светху но всё станет мазнёй
    painter.setRenderHint(QPainter::Antialiasing, true);
    // paint line       // можно в цикл paint rect, но лучше отдельно
    QPen pen;
    pen.setWidth(2);
    pen.setColor(point_inactive_color_);
    for (int i = 0; i < PointAdrList.size(); ++i)
    {
        painter.setPen(pen);
        if (i < PointAdrList.size() - 1){
            painter.drawLine(PointAdrList[i]->posX  + radius_/2, PointAdrList[i]->posY  + radius_/2,    // radius/2 мб переменную для оптимизации?
                             PointAdrList[i + 1]->posX  + radius_/2, PointAdrList[i + 1]->posY  + radius_/2);
        }
    }
    painter.end();
}

void AxesCurvesPlot::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    int width, height;      // для красоты
    int tmp_x, tmp_y;
    float tmp_column_width, tmp_row_height;
    width = this->width();
    height = this->height();

    tmp_column_width = (width - offset_*2) / float(columns_count_);
    tmp_row_height = (height - offset_*2) / float(rows_count_);

    for (int i = 0; i < columns_count_ + 1; ++i){
        tmp_x = (i * tmp_column_width) + offset_;
        PointAdrList[i]->posX = tmp_x - radius_/2;      // temp
    }
    for (int i = 0; i < rows_count_ + 1; ++i){
        tmp_y = (i * tmp_row_height) + offset_;
        PointAdrList[i]->posY = tmp_y - radius_/2;      // temp
    }
}

void AxesCurvesPlot::mouseMoveEvent(QMouseEvent *event)
{
    for (int i = 0; i < PointAdrList.size(); ++i)
    {
        if (PointAdrList[i]->is_drag == false){
            if (PointAdrList[i]->area.contains(event->pos())) {
                PointAdrList[i]->color = point_active_color_;
            } else {
                PointAdrList[i]->color = point_inactive_color_;
            }
        }


        // так се
        else if (PointAdrList[i]->is_drag == true){     // if nah

            if (event->pos().y() < offset_ || event->pos().y() > this->height() - offset_) {
                return;
            }

            if (event->buttons() & Qt::LeftButton){
                PointAdrList[i]->posY = event->pos().y() - radius_/2;
                PointAdrList[i]->color = point_move_color_;
            }
            break;
        }
    }
    update();
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

void AxesCurvesPlot::MovePoint(int pos_y, int point_number)
{
//    int tmp_y;

//    for (int i = 0; i < rows_count_ + 1; ++i){
//        tmp_y = (i * tmp_row_height) + offset_;
//        PointAdrList[i]->posY = tmp_y - radius_/2;      // temp
//    }
}

void AxesCurvesPlot::CalcPointValue(int current_pos)
{

}
