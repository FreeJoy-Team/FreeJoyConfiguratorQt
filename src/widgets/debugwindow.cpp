#include "debugwindow.h"
#include "ui_debugwindow.h"

#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QSettings>

#include "global.h"
#include <QDebug>
#include <QStandardPaths>
DebugWindow::DebugWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DebugWindow)
{
    ui->setupUi(this);

    m_packetsCount = 0;
    m_writeToFile = false;

    QString docLoc = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (docLoc.isEmpty() == false) {
        docLoc+= "/FreeJoy/";
    }
    QSettings s(docLoc + "FreeJoySettings.conf", QSettings::IniFormat);

    s.beginGroup("OtherSettings");
    ui->checkBox_WriteLog->setChecked(s.value("LogEnabled", "false").toBool());
    s.endGroup();
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
        ui->label_PacketsSpeed->setText(QString::number((double(m_timer.restart()) / double(count)), 'f', 3)
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

    buttonLogReset();
}

void DebugWindow::printMsg(const QString &msg)
{
    QString log(QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + ": " + msg + '\n');
    ui->textBrowser_DebugMsg->insertPlainText(log);         // append?
    ui->textBrowser_DebugMsg->moveCursor(QTextCursor::End); // с plainTextEdit криво пашет

    if (m_writeToFile) {
        QString date(QDateTime::currentDateTime().toString("YYYY-MM-DDTHH:MM"));
        QFile file(gEnv.pAppSettings->fileName().remove("FreeJoySettings.conf") + "log/" + "FJLog" + date + ".txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            qWarning() << "cant open file";
            return;
        }
        QTextStream out(&file);
        out << log;
    }
}

void DebugWindow::logicalButtonState(int buttonNumber, bool state)
{
    if (state) {
        ui->textBrowser_ButtonsPressLog->insertPlainText(
                    QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + ": " + tr("Logical button ")
                    + QString::number(buttonNumber) + tr(" pressed") + '\n');

        ui->textBrowser_ButtonsPressLog->moveCursor(QTextCursor::End);
    } else {
        ui->textBrowser_ButtonsUnpressLog->insertPlainText(
                    QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + ": " + tr("Logical button ")
                    + QString::number(buttonNumber) + tr(" unpressed") + '\n');

        ui->textBrowser_ButtonsUnpressLog->moveCursor(QTextCursor::End);
    }
}

void DebugWindow::buttonLogReset()
{
    ui->textBrowser_ButtonsPressLog->clear();
    ui->textBrowser_ButtonsUnpressLog->clear(); // need to improve
}

void DebugWindow::on_checkBox_WriteLog_clicked(bool checked)
{
    gEnv.pAppSettings->beginGroup("OtherSettings");
    gEnv.pAppSettings->setValue("LogEnabled", checked);
    gEnv.pAppSettings->endGroup();

    m_writeToFile = checked;
}
