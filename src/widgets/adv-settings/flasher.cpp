#include "flasher.h"
#include "ui_flasher.h"

#include <QFileDialog>
#include <QTimer>

#include "deviceconfig.h"
#include "global.h"

#include <QDebug>

Flasher::Flasher(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Flasher)
{
    ui->setupUi(this);
    m_enterToFlash_BtnText = ui->pushButton_FlasherMode->text();
    m_flashButtonText = ui->pushButton_FlashFirmware->text();
}

Flasher::~Flasher()
{
    delete ui;
}

void Flasher::retranslateUi()
{
    ui->retranslateUi(this);
}

void Flasher::deviceConnected(bool isConnect)
{
    if (isConnect == true) { // disable enter to flash button if flasher connected
        ui->pushButton_FlasherMode->setEnabled(true);
    } else {
        ui->pushButton_FlasherMode->setEnabled(false);
    }
}

void Flasher::flasherFound(bool isFound)
{
    ui->pushButton_FlashFirmware->setEnabled(isFound);
    // disable enter to flash button if flasher connected
    ui->pushButton_FlasherMode->setEnabled(!isFound);
    if (isFound == true) {
        qDebug()<< "Flasher found";
        ui->pushButton_FlasherMode->setStyleSheet(m_defaultButtonStyle + "color: white; background-color: rgb(0, 128, 0);");
        ui->pushButton_FlasherMode->setText(tr("Ready to flash"));
    } else {
        ui->pushButton_FlasherMode->setStyleSheet(m_defaultButtonStyle);
        ui->pushButton_FlasherMode->setText(m_enterToFlash_BtnText);
    }
}

void Flasher::on_pushButton_FlasherMode_clicked()
{
    emit flashModeClicked(false);
}

void Flasher::on_pushButton_FlashFirmware_clicked()
{
    QFile file(
        QFileDialog::getOpenFileName(this, tr("Open Config"), QDir::currentPath() + "/", tr("Binary files (.bin) (*.bin)")));

    if (file.open(QIODevice::ReadWrite)) {
        ui->pushButton_FlashFirmware->setEnabled(false);
        m_defaultButtonStyle = ui->pushButton_FlashFirmware->styleSheet();
        m_fileArray = file.readAll();
        qDebug() << "file array size =" << m_fileArray.size();
        emit startFlash(true);
    } else {
        qDebug() << "cant open file";
    }
}

const QByteArray *Flasher::fileArray() const
{
    return &m_fileArray;
}

void Flasher::flashStatus(int status, int percent)
{
    ui->progressBar_Flash->setValue(percent);

    if (percent == 1) {
        ui->pushButton_FlasherMode->setText(tr("Firmware flashing.."));
    }

    if (status == FINISHED) {
        ui->pushButton_FlashFirmware->setText(tr("Finished"));
        ui->pushButton_FlashFirmware->setStyleSheet(m_defaultButtonStyle
                                                    + "color: white; background-color: rgb(0, 128, 0);");
        flashDone();
    } else if (status == SIZE_ERROR) {
        ui->pushButton_FlashFirmware->setText(tr("SIZE ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet(m_defaultButtonStyle
                                                    + "color: white; background-color: rgb(200, 0, 0);");
        flashDone();
    } else if (status == CRC_ERROR) {
        ui->pushButton_FlashFirmware->setText(tr("CRC ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet(m_defaultButtonStyle
                                                    + "color: white; background-color: rgb(200, 0, 0);");
        flashDone();
    } else if (status == ERASE_ERROR) {
        ui->pushButton_FlashFirmware->setText(tr("ERASE ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet(m_defaultButtonStyle
                                                    + "color: white; background-color: rgb(200, 128, 0);");
        flashDone();
    } else if (status == 666) {
        ui->pushButton_FlashFirmware->setText(tr("ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet(m_defaultButtonStyle
                                                    + "color: white; background-color: rgb(200, 0, 0);");
        flashDone();
    }
}

void Flasher::flashDone()
{
    QTimer::singleShot(1000, [&] {
        ui->pushButton_FlashFirmware->setStyleSheet(m_defaultButtonStyle);
        ui->pushButton_FlashFirmware->setEnabled(false);
        ui->pushButton_FlashFirmware->setText(m_flashButtonText);
        ui->pushButton_FlasherMode->setEnabled(true);
        ui->pushButton_FlasherMode->setStyleSheet(m_defaultButtonStyle);
        ui->pushButton_FlasherMode->setText(m_enterToFlash_BtnText);
        m_fileArray.clear();
        m_fileArray.shrink_to_fit();
    });
}
