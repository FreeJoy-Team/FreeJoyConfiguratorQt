#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QWidget>

class QPropertyAnimation;

class SwitchButton : public QWidget
{
    Q_OBJECT
public:
    explicit SwitchButton(QWidget *parent = nullptr);

signals:
    void currentIndexChanged(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:

    QPixmap changePixmapColor(const QPixmap &pixmap, const QColor &color);
    void changeColor(int index);

    QPixmap m_cloudPix = QPixmap(":/Images/cloud.png");
    const QColor m_kCloudLightMode = QColor(195, 195, 195);
    const QColor m_kCloudDarkMode = QColor(100, 100, 100);

    QPixmap m_sunPix = QPixmap(":/Images/sun.png");
    const QColor m_kSunLightMode = QColor(248, 227, 161);
    const QColor m_kSunDarkMode = QColor(124, 113, 60);

    QPixmap m_moonPix = QPixmap(":/Images/moon.png");
    const QColor m_kMoonLightMode = QColor(74, 79, 89);//QColor(49, 54, 59);
    const QColor m_kMoonDarkMode = QColor(248, 227, 161);

    QColor m_backgroundColor;

    const int m_kSwitchCount = 3;
    //const int m_kOffset = 5;

    int m_currentSwitch;
    int m_oldSwitch;
    int m_mouseOnSwitchNumber;
    bool m_mousePressed;
    int m_pressPos;

    float m_halfWidth;
    float m_halfHeight;
};

#endif // SWITCHBUTTON_H
