#ifndef AXESCURVESPLOT_H
#define AXESCURVESPLOT_H

#include <QWidget>
#include <QLabel>

#define CURVE_PLOT_POINTS_COUNT 11

#define CURVES_MIN_VALUE -100
#define CURVES_MAX_VALUE 100

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

    void SetPointValue(int point_number, int value);

    void UpdateAxis(int pos_x, int pos_y);
    void DeviceStatus(bool is_connect);

    void SetLinear();
    void SetLinearInvert();
    void SetExponent();
    void SetExponentInvert();
    void SetShape();

signals:
    //! current x and width for a2b
    void sizeChanged(int width);    // not used
    void pointValueChanged(const int *point, const int *value);

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

    const int offset_ = 20;//20
    const int columns_count_ = 10;
    const int rows_count_ = 10;
    const int radius_ = 16;
    const int label_width_ = 20;
    bool is_device_connect_;

    float column_width_;
    float row_height_;
    int width_, height_;
    int half_radius_;

    const int max_point_value = CURVES_MAX_VALUE;
    const int min_point_value = CURVES_MIN_VALUE;

    int points_count_;
    //int current_curve_;

    QColor const point_current_pos_color_ = QColor(190,0,0,200);
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
