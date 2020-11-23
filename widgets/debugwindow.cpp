#include "debugwindow.h"
#include "ui_debugwindow.h"

#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QTextStream>
#include <QTime>

#include "global.h"
#include <QDebug>

DebugWindow::DebugWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DebugWindow)
{
    ui->setupUi(this);

    m_packetsCount = 0;
    m_writeToFile = false;
}

DebugWindow::~DebugWindow()
{
    delete ui;
}

void DebugWindow::retranslateUi()
{
    ui->retranslateUi(this);
}

void DebugWindow::devicePacketReceived()
{
    //    if (isVisible()){
    //    }
    static int count = 0;
    static QElapsedTimer packet_timer;

    m_packetsCount++;
    if (packet_timer.hasExpired(100)) {
        ui->label_PacketsCount->setNum(m_packetsCount);
        packet_timer.start();
    }

    if (m_timer.hasExpired(5000) && m_timer.isValid()) {
        ui->label_PacketsSpeed->setText(QString::number(((double) m_timer.restart() / (double) count), 'f', 3)
                                        + tr(" ms"));
        count = 0;
    } else if (m_timer.isValid() == false) { // валид-инвалид для правильного отображения при подключении-отключении девайса
        m_timer.start();
    }

    count++;
}

void DebugWindow::resetPacketsCount()
{
    m_packetsCount = 0;
    ui->label_PacketsCount->setNum(m_packetsCount);

    m_timer.invalidate();
    ui->label_PacketsSpeed->setText(tr("0 ms"));
}

void DebugWindow::printMsg(const QString &msg)
{
    QString log(QTime::currentTime().toString() + ": " + msg + '\n');
    ui->textBrowser_DebugMsg->insertPlainText(log);         // append?
    ui->textBrowser_DebugMsg->moveCursor(QTextCursor::End); // с plainTextEdit криво пашет

    if (m_writeToFile) {
        QFile file(QDir::currentPath() + '/' + "FreeJoyConfigurator_Log.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            qDebug() << "cant open file";
            return;
        }
        QTextStream out(&file);
        out << log;
    }
}

void DebugWindow::logicalButtonState(int buttonNumber, bool state)
{
    if (gEnv.pDebugWindow) {
        if (state) {
            ui->textBrowser_ButtonsPressLog->insertPlainText(QTime::currentTime().toString() + ": " + tr("Logical button ")
                                                             + QString::number(buttonNumber) + tr(" pressed") + '\n');
            ui->textBrowser_DebugMsg->moveCursor(QTextCursor::End);
        } else {
            ui->textBrowser_ButtonsUnpressLog->insertPlainText(QTime::currentTime().toString() + ": "
                                                               + tr("Logical button ") + QString::number(buttonNumber)
                                                               + tr(" unpressed") + '\n');
            ui->textBrowser_DebugMsg->moveCursor(QTextCursor::End);
        }
    }
}

void DebugWindow::on_checkBox_WriteLog_clicked(bool checked)
{
    QString text = ui->textBrowser_DebugMsg->toPlainText();
    QFile file(QDir::currentPath() + '/' + "FreeJoyConfigurator_Log.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qDebug() << "cant open file";
        return;
    } else {
        file.resize(0);
    }
    QTextStream out(&file);
    out << "########## START WRITE LOG ##########\n" << text;

    m_writeToFile = checked;
}
