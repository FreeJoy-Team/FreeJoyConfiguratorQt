#include "flasher.h"
#include "ui_flasher.h"

#include <QFileDialog>
#include <QTimer>

#include "global.h"
#include "deviceconfig.h"

#include <QDebug>

Flasher::Flasher(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Flasher)
{
    ui->setupUi(this);
    enter_to_flash_btn_text_ = ui->pushButton_FlasherMode->text();
    flash_button_text_ = ui->pushButton_FlashFirmware->text();
}

Flasher::~Flasher()
{
    delete ui;
}

void Flasher::RetranslateUi()
{
    ui->retranslateUi(this);
}


void Flasher::DeviceConnected(bool is_connect)
{
    if (is_connect == true){        // disable enter to flash button if flasher connected
        ui->pushButton_FlasherMode->setEnabled(true);
    } else {
        ui->pushButton_FlasherMode->setEnabled(false);
    }
}

void Flasher::flasherFound(bool is_found)
{
    qDebug()<<"Flasher found";
    ui->pushButton_FlashFirmware->setEnabled(is_found);
    // disable enter to flash button if flasher connected
    if (is_found == true){
        ui->pushButton_FlasherMode->setEnabled(false);
        ui->pushButton_FlasherMode->setStyleSheet(default_button_style_ + "color: white; background-color: rgb(0, 128, 0);");
        ui->pushButton_FlasherMode->setText(tr("Ready to flash"));
    } else {
        ui->pushButton_FlasherMode->setEnabled(true);
        ui->pushButton_FlasherMode->setStyleSheet(default_button_style_);
        ui->pushButton_FlasherMode->setText(enter_to_flash_btn_text_);
    }
}

void Flasher::on_pushButton_FlasherMode_clicked()
{
    emit flashModeClicked(false);
}

void Flasher::on_pushButton_FlashFirmware_clicked()
{
    QFile file(QFileDialog::getOpenFileName(this, tr("Open Config"), QDir::currentPath() + "/", tr("Binary files (.bin) (*.bin)")));

    if (file.open(QIODevice::ReadWrite)){
        ui->pushButton_FlashFirmware->setEnabled(false);
        default_button_style_ = ui->pushButton_FlashFirmware->styleSheet();
        file_array_ = file.readAll();
        qDebug()<<"file array size ="<<file_array_.size();
        emit startFlash(true);
    } else {
        qDebug()<<"cant open file";
    }
}


const QByteArray* Flasher::GetFileArray()
{
    return &file_array_;
}


void Flasher::flashStatus(int status, int percent)
{
    ui->progressBar_Flash->setValue(percent);

    if (percent == 1){
        ui->pushButton_FlasherMode->setText(tr("Firmware flashing.."));
    }

    if (status == FINISHED){
        ui->pushButton_FlashFirmware->setText(tr("Finished"));
        ui->pushButton_FlashFirmware->setStyleSheet(default_button_style_ + "color: white; background-color: rgb(0, 128, 0);");
        FlashDone();
    }
    else if (status == SIZE_ERROR){
        ui->pushButton_FlashFirmware->setText(tr("SIZE ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet(default_button_style_ + "color: white; background-color: rgb(200, 0, 0);");
        FlashDone();
    }
    else if (status == CRC_ERROR){
        ui->pushButton_FlashFirmware->setText(tr("CRC ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet(default_button_style_ + "color: white; background-color: rgb(200, 0, 0);");
        FlashDone();
    }
    else if (status == ERASE_ERROR){
        ui->pushButton_FlashFirmware->setText(tr("ERASE ERROR"));
        ui->pushButton_FlashFirmware->setStyleSheet(default_button_style_ + "color: white; background-color: rgb(200, 128, 0);");
        FlashDone();
    }
}

void Flasher::FlashDone()
{
    QTimer::singleShot(1000, [&]{
        ui->pushButton_FlashFirmware->setStyleSheet(default_button_style_);
        ui->pushButton_FlashFirmware->setEnabled(false);
        ui->pushButton_FlashFirmware->setText(flash_button_text_);
        ui->pushButton_FlasherMode->setEnabled(true);
        ui->pushButton_FlasherMode->setStyleSheet(default_button_style_);
        ui->pushButton_FlasherMode->setText(enter_to_flash_btn_text_);
        file_array_.clear();
        file_array_.shrink_to_fit();
    });
}
