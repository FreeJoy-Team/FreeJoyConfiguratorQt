#ifndef AXESTOBUTTONSSLIDER_H
#define AXESTOBUTTONSSLIDER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE
//#include <QLabel>

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

    void SetPointsCount(uint count);
    uint GetPointsCount() const;

    void SetPointValue(uint value, uint point_number);
    uint GetPointValue(uint point_number) const;

    void SetAxisOutputValue(int out_value, bool is_enable);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent*) override;
    bool event(QEvent *event) override;

private:
    Ui::AxesToButtonsSlider *ui;
    void DrawPoint(QPoint point_pos, uint range_points);
    void MovePointer(uint pos_x, uint point_number);
    uint CalcPointValue(int current_pos) const;
    void PointsPositionReset();
    void SetLableValue(int point_pos ,uint point_number);

    float line_spacing_;

    float axis_output_value_;
    int axis_output_width_;
    bool is_out_enabled_;

    const QColor kAxisRectColor_ = QColor(160, 0, 0);
    const QColor kAxisRectColor_dis_ = QColor(160, 0, 0, 80);
    const QColor kPointRawActivColor_ = QColor(0, 170, 0);
    QColor axis_rect_color_;

    const int kHalfPointerWidth_ = 4;

    const QColor kPointerColor_ = QColor(1,119,215);
    const uint kMaxPointValue_ = 255;
    const int kOffset_ = 10;
    const int kRangeBetween_ = 13;  // минимальное расстояние между ближайшими указателями
    const int kLabelWidth_ = 20;
    const int kMinHeight_ = 40;//45

    const int kPaddingTop_ = 7;
    const QPoint kPointer_[5]={
        QPoint(-kHalfPointerWidth_,2),// + padding_top_
        QPoint(-kHalfPointerWidth_,12),
        QPoint(0,20),
        QPoint(kHalfPointerWidth_,12),
        QPoint(kHalfPointerWidth_,2),
    };

    uint points_count_;

    QList<QLabel*> LabelPtrList_;


    struct A2B_point    // uint гемороя наделал
    {
        QPolygon polygon;
        QColor color;
        QLabel* text_label;
        uint posX;
        uint current_value;
        bool is_drag;
    };
     QList<A2B_point*> PointPtrList_;

};

#endif // AXESTOBUTTONSSLIDER_H
