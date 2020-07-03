#ifndef AXESTOBUTTONSSLIDER_H
#define AXESTOBUTTONSSLIDER_H

#include <QWidget>
#include <QLabel>

#include "global.h"
#include "deviceconfig.h"

#define MAX_A2B_BUTTONS 12

namespace Ui {
class AxesToButtonsSlider;
}

class AxesToButtonsSlider : public QWidget
{
    Q_OBJECT

public:
    explicit AxesToButtonsSlider(QWidget *parent = nullptr);
    ~AxesToButtonsSlider();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    //void mousePressEvent(QMouseEvent *event) override;
    //void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::AxesToButtonsSlider *ui;
    void DrawPoint(QPoint point_pos, uint range_points);
    void MovePointer(uint pos_x, uint point_number);

    QPoint pointer[5]={
        QPoint(5,2),
        QPoint(5,9),
        QPoint(8,15),
        QPoint(11,9),
        QPoint(11,2),
    };

    QColor pointer_color_ = QColor(1,119,215);
    //const uint16_t MaxScaleValue=255;
    const uint offset_ = 10;
    uint buttons_count_;

    QList<QLabel*> LabelAdrList;

    struct A2B_parameters {
        QPolygon pointer_polygon;
        QColor pointer_color;
        QLabel* text_label;
        uint pointer_posX;
        bool is_pointer_drag;
    } range_point[MAX_A2B_BUTTONS];
};

#endif // AXESTOBUTTONSSLIDER_H
