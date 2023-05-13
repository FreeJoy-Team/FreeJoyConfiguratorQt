#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <QWidget>
#include <QColor>
#include <QPainterPath>

class ColorWheel : public QWidget
{
    Q_OBJECT
public:
    explicit ColorWheel(QWidget *parent = nullptr);
    ~ColorWheel();

    void setCurrentColor(const QColor &color);
    QColor currentColor() const;

signals:
    //void colorSelected(const QColor &color);
    void currentColorChanged(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPixmap m_colorWheel;
    QPixmap drawColorWheel();
    QPainterPath m_colorWheelFrame;

    QColor posToColor(const QPoint &pos);
    QPoint colorToPos(const QColor &color);

    const int m_margin = 3;
    bool m_mousePressed;

    float m_radius;
    QPoint m_center;

    const int m_pickerSize = 9;
    QRect m_picker;
    const QColor m_pickerColor = QColor(0, 0, 0);
    QColor m_currentColor;
};

#endif // COLORWHEEL_H
