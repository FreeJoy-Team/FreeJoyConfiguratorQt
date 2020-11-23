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

namespace Ui {
class AxesCurvesPlot;
}

class AxesCurvesPlot : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurvesPlot(QWidget *parent = nullptr);
    ~AxesCurvesPlot();

    int pointValue(int point_number);
    int pointCount() const;

    void setPointValue(int pointNumber, int value);

    void updateAxis(int posX, int posY);
    void deviceStatus(bool isConnect);

    void setLinear();
    void setLinearInvert();
    void setExponent();
    void setExponentInvert();
    void setShape();

signals:
    //! current x and width for a2b
    void sizeChanged(int width); // not used
    void pointValueChanged(const int *point, const int *value);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    //bool event(QEvent *event) override;

private:
    Ui::AxesCurvesPlot *ui;
    int calcPointValue(int currentPos) const;
    int calcPointPos(int value) const;
    int calcPointPosX(int valueX) const;
    void movePoint(int pos_y, int point_number);
    void updateLabelPos();

    const int m_kOffset = 20; //20
    const int m_kColumnsCount = 10;
    const int m_kRowsCount = 10;
    const int m_kRadius = 16;
    const int m_kLabelWidth = 20;
    bool m_isDeviceConnect;

    float m_columnWidth;
    float m_rowHeight;
    int m_width;
    int m_height;
    int m_halfRadius;

    const int m_kMaxPointValue = CURVES_MAX_VALUE;
    const int m_kMinPointValue = CURVES_MIN_VALUE;

    int m_pointsCount;

    const QColor m_kPointCurrentPosColor = QColor(190, 0, 0, 200);
    const QColor m_kPointInactiveColor = QColor(1, 119, 215);
    const QColor m_kPointActiveColor = Qt::black;
    const QColor m_kPointMoveColor = Qt::lightGray;
    const QColor m_kCurveColor = QColor(1, 119, 215);

    bool m_pointActive;

    struct AxesCurve_currentPos
    {
        QColor color;
        QRect area;
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
