#include "debugwindow.h"
#include "ui_debugwindow.h"

DebugWindow::DebugWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);
    main_window_x_ = 0;
    main_window_y_ = 0;
    is_drag_ = false;
    is_magneted_ = true;
}

DebugWindow::~DebugWindow()
{
    delete ui;
}


void DebugWindow::MainWindowPos(int x, int y)
{
    main_window_x_ = x;
    main_window_y_ = y;
}

//bool temp = false;
void DebugWindow::moveEvent(QMoveEvent *event)
{
    Q_UNUSED(event)
//    if (is_magneted_ == false && this->x() - main_window_x_ <= 50 && this->x() - main_window_x_ >= -50)
//    {
//        this->move(main_window_x_, this->y());  // WARNING!!!!!!!!!!!!!!!!!!!!
//        is_magneted_ = true;
//    }
//    else if (is_drag_ == true)
//    {
//        is_magneted_ = false;
//    }

//    if (true)
//    {

//    }
}

void DebugWindow::mouseMoveEvent(QMouseEvent *event)
{
//    if (is_drag_ == false && event->buttons() & Qt::LeftButton){
//        drag_click_pos_ = event->pos();
//        is_drag_ = true;
//    }
//    if (is_magneted_ == true && is_drag_ == true && event->pos().x() - drag_click_pos_.x() <= -25 && event->pos().x() - drag_click_pos_.x() >= 25){

//    }
}

void DebugWindow::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    //is_drag_ = true;
}

void DebugWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    is_drag_ = false;
}
