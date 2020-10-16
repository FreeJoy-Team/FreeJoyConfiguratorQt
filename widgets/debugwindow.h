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

    void PrintMsg(const QString &msg);

    void LogicalButtonState(int button_number, bool state);

private:
    Ui::DebugWindow *ui;

    int packets_count;
    QElapsedTimer timer;
};

#endif // DEBUGWINDOW_H
