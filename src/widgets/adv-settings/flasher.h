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

    void retranslateUi();

    void deviceConnected(bool isConnect);

    const QByteArray *fileArray() const;

signals:
    void flashModeClicked(bool is_start_flash);
    void startFlash(bool is_start_flash);

public slots:
    void flasherFound(bool isFound);
    void flashStatus(int status, int percent);
private slots:
    void on_pushButton_FlasherMode_clicked();
    void on_pushButton_FlashFirmware_clicked();

private:
    Ui::Flasher *ui;

    QByteArray m_fileArray;
    QString m_flashButtonText;
    QString m_enterToFlash_BtnText;
    QString m_defaultButtonStyle;
    void flashDone();
};

#endif // FLASHER_H
