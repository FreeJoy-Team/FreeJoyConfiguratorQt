
#ifndef COLORVALUESLIDER_H
#define COLORVALUESLIDER_H

#include <QSlider>
#include <QGradient>


class ColorValueSlider : public QSlider
{
    Q_OBJECT
public:
    explicit ColorValueSlider(QWidget *parent = nullptr);
    explicit ColorValueSlider(Qt::Orientation orientation, QWidget *parent = nullptr);

    void setCurrentColor(const QColor &color);
    QColor currentColor() const;

    void setFirstColor(const QColor &color); // rename
    QColor firstColor() const;

signals:
    void firstColorChanged(const QColor &color);
    void currentColorChanged(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QLinearGradient m_gradient;
    QColor m_currentColor;
    QColor m_firstColor;
};

#endif // COLORVALUESLIDER_H
