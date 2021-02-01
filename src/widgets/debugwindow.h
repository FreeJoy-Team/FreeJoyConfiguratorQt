#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QElapsedTimer>
#include <QWidget>

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = nullptr);
    ~DebugWindow();

    void retranslateUi();

    void devicePacketReceived();
    void resetPacketsCount();

    void logicalButtonState(int buttonNumber, bool state);

    Q_INVOKABLE // для мультипотока, хз правильно ли, но работает. CustomMessageHandler in main
        void printMsg(const QString &msg); // не уверен насчёт ссылки, мб надо копию получать с мультипотоком

private slots:
    void on_checkBox_WriteLog_clicked(bool checked);

private:
    Ui::DebugWindow *ui;
    void buttonLogReset();

    int m_packetsCount;
    QElapsedTimer m_timer;
    bool m_writeToFile;
};

#endif // DEBUGWINDOW_H
