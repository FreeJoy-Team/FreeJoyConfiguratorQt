#ifndef AXESCURVESPLOT_H
#define AXESCURVESPLOT_H

#include <QWidget>

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
    void MovePoint(int pos_y, int point_number);

    int offset_ = 15;
    int columns_count_ = 10;
    int rows_count_ = 10;
    int radius_ = 16;

    int column_width_;
    int row_height_;
    int width_, height_;
    int half_radius_;

    int const max_point_value = 100;
    int const min_point_value = -100;

    int points_count_;

    QColor const point_inactive_color_ = QColor(1,119,215);
    QColor const point_active_color_ = Qt::black;
    QColor const point_move_color_ = Qt::lightGray;
    QColor const curve_color_ = QColor(1,119,215);

    bool point_active_;

    struct AxesCurve_point
    {
        QColor color;
        QRect area;
        int posX;
        int posY;
        int current_value;
        bool is_drag;
    };
     QList<AxesCurve_point*> PointAdrList;
};

#endif // AXESCURVESPLOT_H
