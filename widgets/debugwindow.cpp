#include "debugwindow.h"
#include "ui_debugwindow.h"

#include <QElapsedTimer>
#include <QTime>

DebugWindow::DebugWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);

    packets_count = 0;
}

DebugWindow::~DebugWindow()
{
    delete ui;
}

void DebugWindow::RetranslateUi()
{
    ui->retranslateUi(this);
}


void DebugWindow::DevicePacketReceived()
{
//    if (isVisible()){
//    }
    static int count = 0;
    static QElapsedTimer packet_timer;

    packets_count++;
    //ui->label_PacketsCount->setNum(packets_count);
    if (packet_timer.hasExpired(100)){
        ui->label_PacketsCount->setNum(packets_count);
        packet_timer.start();
    }

    if (timer.hasExpired(5000) && timer.isValid()){
        ui->label_PacketsSpeed->setText(QString::number(((double)timer.restart() / (double)count), 'f', 3) + tr(" ms"));
        count = 0;
    }
    else if (timer.isValid() == false){     // валид-инвалид для правильного отображения при подключении-отключении девайса
        timer.start();
    }

    count++;
}

void DebugWindow::ResetPacketsCount()
{
    packets_count = 0;
    ui->label_PacketsCount->setNum(packets_count);

    timer.invalidate();
    ui->label_PacketsSpeed->setText(tr("0 ms"));
}


void DebugWindow::PrintMsg(const QString &msg)
{
//    ui->plainTextEdit_DebugMsg->insertPlainText(QTime::currentTime().toString() + ':' + ' ' + msg + '\n');
//    ui->plainTextEdit_DebugMsg->verticalScrollBar()->setValue(ui->plainTextEdit_DebugMsg->verticalScrollBar()->maximum());
    //ui->plainTextEdit_DebugMsg->appendPlainText(QTime::currentTime().toString() + ':' + ' ' + msg);
    //ui->plainTextEdit_DebugMsg->append(QTime::currentTime().toString() + ':' + ' ' + msg);

    //ui->plainTextEdit_DebugMsg->verticalScrollBar()->setValue(ui->plainTextEdit_DebugMsg->verticalScrollBar()->maximum());

    ui->textBrowser_DebugMsg->insertPlainText(QTime::currentTime().toString() + ": " + msg + '\n'); // append?
    ui->textBrowser_DebugMsg->moveCursor(QTextCursor::End);     // бля, с plainTextEdit криво пашет
}


void DebugWindow::LogicalButtonState(int button_number, bool state)
{
    if (state){
        ui->textBrowser_ButtonsPressLog->insertPlainText(QTime::currentTime().toString() + ": " + tr("Logical button ") + QString::number(button_number) + tr(" pressed") + '\n');
        ui->textBrowser_DebugMsg->moveCursor(QTextCursor::End);
    } else {
        ui->textBrowser_ButtonsUnpressLog->insertPlainText(QTime::currentTime().toString() + ": " + tr("Logical button ") + QString::number(button_number) + tr(" unpressed") + '\n');
        ui->textBrowser_DebugMsg->moveCursor(QTextCursor::End);
    }
}
