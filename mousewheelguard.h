#ifndef MOUSEWHEELGUARD_H
#define MOUSEWHEELGUARD_H

#include <QObject>

//class FilterObject:public QObject{
//public:
//    QWidget *target = nullptr;//it holds a pointer to target object
//    int goalHeight=0;
//    FilterObject(QObject *parent=nullptr):QObject(parent){}//uses QObject constructor
//    bool eventFilter(QObject *watched, QEvent *event) override;//and overrides eventFilter function
//};

class MouseWheelGuard : public QObject
{
public:
    explicit MouseWheelGuard(QObject *parent);

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
};

#endif // MOUSEWHEELGUARD_H
