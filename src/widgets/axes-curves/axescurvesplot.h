#ifndef AXESCURVESPLOT_H
#define AXESCURVESPLOT_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE
//#include <QLabel>

#define CURVE_PLOT_POINTS_COUNT 11 //?

#define CURVES_MIN_VALUE -100 //?
#define CURVES_MAX_VALUE 100

class AxesCurvesPlot : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurvesPlot(bool labelEnabled = true, QWidget *parent = nullptr);

    void setBorderOffset(int offset);
    void setPointRadius(int radius);
    void setLineWidth(int width);

    int pointValue(int point_number);
    int pointCount() const;

    void setPointValue(int pointNumber, int value);

    void setCurAxisPos(int posX, int posY);
    void deviceStatus(bool isConnect);

    void setLinear();
    void setExponent();
    void setExponentInvert();
    void setShape();

signals:
    void pointValueChanged(const int *point, const int *value);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    int calcPointValue(int currentPos) const;
    int calcPointPosY(int value) const;
    int calcPointPosX(int value) const;
    void movePoint(int pos_y, int point_number);
    void updateLabelPos();

    int m_radius = 8;
    int m_lineWidth = 2;
    int m_offset = 20;
    int m_currentPosRadius = 6;
    const int m_kPointsCount = CURVE_PLOT_POINTS_COUNT;
    const int m_kColumnsCount = CURVE_PLOT_POINTS_COUNT - 1;
    const int m_kRowsCount = CURVE_PLOT_POINTS_COUNT - 1;
    const int m_kLabelWidth = 20;
    bool m_isDeviceConnect;

    float m_columnWidth;
    float m_rowHeight;
    int m_width;
    int m_height;

    const int m_kMaxPointValue = CURVES_MAX_VALUE;
    const int m_kMinPointValue = CURVES_MIN_VALUE;

    const QColor m_kPointCurrentPosColor = QColor(190, 0, 0, 220);
    const QColor m_kPointInactiveColor = QColor(1, 119, 215);
    const QColor m_kPointActiveColor = Qt::black;
    const QColor m_kPointMoveColor = Qt::lightGray;
    const QColor m_kCurveColor = QColor(1, 119, 215);

    struct AxesCurve_currentPos
    {
        QColor color;
        int posX;
        int posY;
    };
    AxesCurve_currentPos m_curAxisPos;

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
    QList<AxesCurve_point *> m_pointPtrList;
    QList<QLabel *> m_labelPtrList;
};

#endif // AXESCURVESPLOT_H
