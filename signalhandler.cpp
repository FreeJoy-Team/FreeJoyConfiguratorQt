#include "signalhandler.h"
#include <QThread>
#include "widgets/pincombobox.h"
#include "widgets/pinconfig.h"

SignalHandler::SignalHandler(QObject *parent) : QObject(parent)         // снести нахер это ненужное говно
{
    threadSignalHandler = new QThread();
    moveToThread(threadSignalHandler);
        connect(threadSignalHandler, SIGNAL(started()), this, SLOT(ValueChangeEvent()));
        threadSignalHandler->start();
}

SignalHandler::~SignalHandler()
{
    is_finish_ = true;
    threadSignalHandler->quit();
    threadSignalHandler->deleteLater();
    threadSignalHandler->wait();
    delete threadSignalHandler;
}

#include <QDebug>
void SignalHandler::ValueChangeEvent()       // задержка в начале КОСТЫЛЬ. Она нужна на время создания приложения,
{                                            // чтобы сигнал не отправился в пустоту
    QThread::msleep(1000);
    int a2b_count = 0;
    while (is_finish_ == false)
    {
        QThread::msleep(500);
        // axes to buttons count signal
        int tmp_a2b = 0;
        for (int i = 0; i < MAX_AXIS_NUM; ++i){
            if (gEnv.pDeviceConfig->config.axes_to_buttons[i].is_enabled){
                tmp_a2b += gEnv.pDeviceConfig->config.axes_to_buttons[i].buttons_cnt;
            }
        }
        if (a2b_count != tmp_a2b){
            a2b_count = tmp_a2b;
            emit a2bCountChanged(678, a2b_count);       // 678 в PinConfig
            qDebug()<<a2b_count;
        }

        // button count

    }
}
