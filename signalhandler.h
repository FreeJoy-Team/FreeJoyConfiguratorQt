#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include <QObject>
#include "common_types.h"

#include "global.h"
#include "deviceconfig.h"

class SignalHandler : public QObject
{
    Q_OBJECT
public:
    explicit SignalHandler(QObject *parent = nullptr);
    ~SignalHandler();

private:
    QThread* threadSignalHandler;
    bool is_finish_ = false;

signals:
    void a2bCountChanged(int a2b_identifier, int a2b_count);

private slots:
    void ValueChangeEvent();

};

#endif // SIGNALHANDLER_H
