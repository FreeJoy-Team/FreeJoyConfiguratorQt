#include "mousewheelguard.h"

#include <QEvent>
#include <QWidget>

MouseWheelGuard::MouseWheelGuard(QObject *parent)
    : QObject(parent)
{}

// protection against the mouse wheel if the widget is not setFocusPolicy(Qt::WheelFocus)
// без протекта можно при прокручивании страницы случайно навести на комбобокс и изменить его колесом мыши
// при установке setFocusPolicy(Qt::StrongFocus) и протекта на комбобокс придётся нажать, для прокручивания колесом
bool MouseWheelGuard::eventFilter(QObject *o, QEvent *e)
{
    const QWidget *widget = static_cast<QWidget *>(o);
    if (e->type() == QEvent::Wheel && widget && !widget->hasFocus()) {
        e->ignore();
        return true;
    }

    return QObject::eventFilter(o, e);
}
