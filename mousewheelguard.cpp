#include "mousewheelguard.h"

#include <QEvent>
#include <QWidget>

//#include <QResizeEvent>
//bool FilterObject::eventFilter(QObject *watched, QEvent *event) {
//    if(watched!=target){//checks for correct target object.
//        return false;
//    }
//    if(event->type()!=QEvent::Resize){//and correct event
//        return false;
//    }

//    QResizeEvent *resEvent = static_cast<QResizeEvent*>(event);//then sets correct event type

//    goalHeight = 7*resEvent->size().width()/3;//calculates height, 7/3 of width in my case
//    if(target->height()!=goalHeight){
//        target->setFixedHeight(goalHeight);
//    }

//    return true;
//};

MouseWheelGuard::MouseWheelGuard(QObject *parent) : QObject(parent)
{
}

bool MouseWheelGuard::eventFilter(QObject *o, QEvent *e)
{
    const QWidget* widget = static_cast<QWidget*>(o);
    if (e->type() == QEvent::Wheel && widget && !widget->hasFocus())
    {
        e->ignore();
        return true;
    }

    return QObject::eventFilter(o, e);
}
