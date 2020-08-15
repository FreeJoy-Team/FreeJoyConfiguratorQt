#ifndef MOUSEWHEELGUARD_H
#define MOUSEWHEELGUARD_H

#include <QObject>

class MouseWheelGuard : public QObject
{
public:
    explicit MouseWheelGuard(QObject *parent);

protected:
    bool eventFilter(QObject* o, QEvent* e) override;
};

#endif // MOUSEWHEELGUARD_H
