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

private slots:
    void on_checkBox_WriteLog_clicked(bool checked);

private:
    Ui::DebugWindow *ui;

    int packets_count_;
    QElapsedTimer timer_;
    bool write_to_file_;
};

#endif // DEBUGWINDOW_H
