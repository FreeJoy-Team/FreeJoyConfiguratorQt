#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPoint;
QT_END_NAMESPACE

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = nullptr);
    ~DebugWindow();

    void MainWindowPos(int x, int y);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void moveEvent(QMoveEvent *event) override;

private:
    Ui::DebugWindow *ui;

    int main_window_x_;
    int main_window_y_;

    QPoint drag_click_pos_;

    bool is_drag_;
    bool is_magneted_;

//    void moveEvent(QMoveEvent *event);
};

#endif // DEBUGWINDOW_H
