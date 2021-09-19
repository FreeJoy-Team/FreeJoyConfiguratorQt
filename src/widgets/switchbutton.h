#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QWidget>

class QPropertyAnimation;

class SwitchButton : public QWidget
{
    Q_OBJECT
public:
    explicit SwitchButton(QWidget *parent = nullptr);

    bool isChecked() const;

public slots:
    void setChecked(bool checked);

signals:
    void stateChanged(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:

    QPixmap changePixmapColor(const QPixmap &pixmap, const QColor &color);
    void changeColor(bool state);

    QPixmap m_sunPix = QPixmap(":/Images/sun.png");
    const QColor m_kSunLightMode = QColor(248, 227, 161);
    const QColor m_kSunDarkMode = QColor(124, 113, 60);
    const QColor m_kSunBackground = QColor(100, 123, 210);;

    QPixmap m_moonPix = QPixmap(":/Images/moon.png");
    const QColor m_kMoonLightMode = QColor(74, 79, 89);//QColor(49, 54, 59);
    const QColor m_kMoonDarkMode = QColor(248, 227, 161);
    const QColor m_kMoonBackground = QColor(39, 51, 69);

    //QColor m_backgroundColor;

    bool m_currentState;
    bool m_mousePressed;

    float m_halfWidth;
    float m_halfHeight;
};

#endif // SWITCHBUTTON_H
