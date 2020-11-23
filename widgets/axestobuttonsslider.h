#ifndef AXESTOBUTTONSSLIDER_H
#define AXESTOBUTTONSSLIDER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE
//#include <QLabel>

#include "deviceconfig.h"
#include "global.h"

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

    void setPointsCount(uint count);
    uint pointsCount() const;

    void setPointValue(uint value, uint pointNumber);
    uint pointValue(uint pointNumber) const;

    void setAxisOutputValue(int outValue, bool isEnable);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *) override;
    bool event(QEvent *event) override;

private:
    Ui::AxesToButtonsSlider *ui;
    void drawPoint(const QPoint &pointPos, uint pointNumber); // QPoint &point_pos
    void movePointer(uint posX, uint pointNumber);
    uint calcPointValue(int currentPos) const;
    void pointsPositionReset();
    void setLableValue(int pointPos, uint pointNumber);

    float m_lineSpacing;

    float m_axisOutputValue;
    int m_axisOutputWidth;
    bool m_isOutEnabled;

    const QColor m_kAxisRectColor = QColor(160, 0, 0);
    const QColor m_kAxisRectColor_dis = QColor(160, 0, 0, 80);
    const QColor m_kPointRawActivColor = QColor(0, 170, 0);
    QColor m_axisRectColor;

    const int m_kHalfPointerWidth = 4;

    const QColor m_kPointerColor = QColor(1, 119, 215);
    const uint m_kMaxPointValue = 255;
    const int m_kOffset = 10;
    const int m_kRangeBetween = 13; // минимальное расстояние между ближайшими указателями
    const int m_kLabelWidth = 20;
    const int m_kMinHeight = 40; //45

    const int m_kPaddingTop = 7;
    const QPoint m_kPointer[5] = {
        QPoint(-m_kHalfPointerWidth, 2), // + padding_top_
        QPoint(-m_kHalfPointerWidth, 12),
        QPoint(0, 20),
        QPoint(m_kHalfPointerWidth, 12),
        QPoint(m_kHalfPointerWidth, 2),
    };

    uint m_pointsCount;

    QList<QLabel *> m_labelPtrList;

    struct A2B_point // uint гемороя наделал
    {
        QPolygon polygon;
        QColor color;
        QLabel *text_label;
        uint posX;
        uint current_value;
        bool is_drag;
    };
    QList<A2B_point *> m_pointPtrList;
};

#endif // AXESTOBUTTONSSLIDER_H
