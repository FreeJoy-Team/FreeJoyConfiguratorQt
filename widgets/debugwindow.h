#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QWidget>
#include <QElapsedTimer>

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = nullptr);
    ~DebugWindow();

    void RetranslateUi();

    void DevicePacketReceived();
    void ResetPacketsCount();

    void LogicalButtonState(int button_number, bool state);

    Q_INVOKABLE // для мультипотока, хз правильно ли, но работает. CustomMessageHandler in main
    void PrintMsg(const QString &msg); // не уверен насчёт ссылки, мб надо копию получать с мультипотоком

private:
    Ui::DebugWindow *ui;

    int packets_count_;
    QElapsedTimer timer_;
};

#endif // DEBUGWINDOW_H
