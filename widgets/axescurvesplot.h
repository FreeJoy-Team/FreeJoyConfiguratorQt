#ifndef AXESCURVESPLOT_H
#define AXESCURVESPLOT_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class AxesCurvesPlot;
}

class AxesCurvesPlot : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurvesPlot(QWidget *parent = nullptr);
    ~AxesCurvesPlot();

    int GetPointValue(int point_number);
    int GetPointCount();

    void SetPointValue(int value, int point_number);

    void UpdateAxis(int pos_x, int pos_y);
    void DeviceStatus(bool is_connect);

    void SetLinear();
    void SetLinearInvert();
    void SetExponent();
    void SetExponentInvert();
    void SetShape();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    //bool event(QEvent *event) override;

private:
    Ui::AxesCurvesPlot *ui;
    int CalcPointValue(int current_pos);
    int CalcPointPos(int value);
    int CalcPointPosX(int value_x);
    void MovePoint(int pos_y, int point_number);
    void UpdateLabelPos();

    const int offset_ = 20;
    const int columns_count_ = 10;
    const int rows_count_ = 10;
    const int radius_ = 16;
    const int label_width_ = 20;
    bool is_device_connect_ = false;

    float column_width_;
    float row_height_;
    int width_, height_;
    int half_radius_;

    const int max_point_value = 100;
    const int min_point_value = -100;

    int points_count_;

    QColor const point_inactive_color_ = QColor(1,119,215);
    QColor const point_active_color_ = Qt::black;
    QColor const point_move_color_ = Qt::lightGray;
    QColor const curve_color_ = QColor(1,119,215);

    bool point_active_;

    struct AxesCurve_currentPos
    {
        QColor color;
        QRect area;
        int posX;
        int posY;
    };
    AxesCurve_currentPos cur_axis_pos;

    struct AxesCurve_point
    {
        QColor color;
        QRect area;
        int posX;
        int posY;
        int current_value;
        bool is_drag;
        //QLabel text_label;
    };
     QList<AxesCurve_point*> PointAdrList;
     QList<QLabel*> LabelAdrList;
};

#endif // AXESCURVESPLOT_H
