#include "mousewheelguard.h"

#include <QEvent>
#include <QWidget>

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
