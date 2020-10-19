#include "debugwindow.h"
#include "ui_debugwindow.h"

#include <QElapsedTimer>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

DebugWindow::DebugWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);

    packets_count_ = 0;
    write_to_file_ = false;
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

    packets_count_++;
    //ui->label_PacketsCount->setNum(packets_count);
    if (packet_timer.hasExpired(100)){
        ui->label_PacketsCount->setNum(packets_count_);
        packet_timer.start();
    }

    if (timer_.hasExpired(5000) && timer_.isValid()){
        ui->label_PacketsSpeed->setText(QString::number(((double)timer_.restart() / (double)count), 'f', 3) + tr(" ms"));
        count = 0;
    }
    else if (timer_.isValid() == false){     // валид-инвалид для правильного отображения при подключении-отключении девайса
        timer_.start();
    }

    count++;
}

void DebugWindow::ResetPacketsCount()
{
    packets_count_ = 0;
    ui->label_PacketsCount->setNum(packets_count_);

    timer_.invalidate();
    ui->label_PacketsSpeed->setText(tr("0 ms"));
}


void DebugWindow::PrintMsg(const QString &msg)
{
//    ui->plainTextEdit_DebugMsg->insertPlainText(QTime::currentTime().toString() + ':' + ' ' + msg + '\n');
//    ui->plainTextEdit_DebugMsg->verticalScrollBar()->setValue(ui->plainTextEdit_DebugMsg->verticalScrollBar()->maximum());
    //ui->plainTextEdit_DebugMsg->appendPlainText(QTime::currentTime().toString() + ':' + ' ' + msg);
    //ui->plainTextEdit_DebugMsg->append(QTime::currentTime().toString() + ':' + ' ' + msg);

    //ui->plainTextEdit_DebugMsg->verticalScrollBar()->setValue(ui->plainTextEdit_DebugMsg->verticalScrollBar()->maximum());

    //scrollToBottom();

    QString log(QTime::currentTime().toString() + ": " + msg + '\n');
    ui->textBrowser_DebugMsg->insertPlainText(log); // append?
    ui->textBrowser_DebugMsg->moveCursor(QTextCursor::End);     // бля, с plainTextEdit криво пашет

    if (write_to_file_){
        QFile file(QDir::currentPath() + '/' + "FreeJoyConfigurator_Log.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)){
            qDebug()<<"cant open file";
            return;
        }
        QTextStream out(&file);
        out << log;
    }
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

void DebugWindow::on_checkBox_WriteLog_clicked(bool checked)
{
    QString text = ui->textBrowser_DebugMsg->toPlainText();
    QFile file(QDir::currentPath() + '/' + "FreeJoyConfigurator_Log.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)){
        qDebug()<<"cant open file";
        return;
    } else {
        file.resize(0);
    }
    QTextStream out(&file);
    out << "########## START WRITE LOG ##########\n" <<text;

    write_to_file_ = checked;
}
