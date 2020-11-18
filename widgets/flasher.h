#ifndef FLASHER_H
#define FLASHER_H

#include <QWidget>

namespace Ui {
class Flasher;
}

class Flasher : public QWidget
{
    Q_OBJECT

public:
    explicit Flasher(QWidget *parent = nullptr);
    ~Flasher();
    
    void RetranslateUi();

    void DeviceConnected(bool is_connect);

    const QByteArray* GetFileArray() const;
    
signals:
    void flashModeClicked(bool is_start_flash);
    void startFlash(bool is_start_flash);
    
private slots:
    void flasherFound(bool is_found);
    void flashStatus(int status, int percent);

    void on_pushButton_FlasherMode_clicked();
    void on_pushButton_FlashFirmware_clicked();

private:
    Ui::Flasher *ui;
    
    QByteArray file_array_;
    QString flash_button_text_;
    QString enter_to_flash_btn_text_;
    QString default_button_style_;
    void FlashDone();
};

#endif // FLASHER_H
