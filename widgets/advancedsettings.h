#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include <QWidget>

namespace Ui {
class AdvancedSettings;
}

class AdvancedSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedSettings(QWidget *parent = nullptr);
    ~AdvancedSettings();

    void ReadFromConfig();
    void WriteToConfig();

    void RetranslateUi();

    void DeviceConnected(bool is_connect);

    const QByteArray* GetFileArray();

signals:
    void languageChanged(QString language);
    void interfaceStyleChanged(bool is_dark);

    void flashModeClicked(bool is_start_flash);
    //void startFlash(QByteArray* file_array);
    void startFlash(bool is_start_flash);

    //void styleChanged();

private slots:
    //void styleButtonWaiting();
    void flasherFound(bool is_found);
    void flashStatus(int status, int percent);

    void on_pushButton_LangEnglish_clicked();
    void on_pushButton_LangRussian_clicked();

    void on_pushButton_StyleDefault_clicked();

    void on_pushButton_StyleWhite_clicked();

    void on_pushButton_StyleDark_clicked();

    void on_spinBox_FontSize_valueChanged(int font_size);

    void on_pushButton_FlasherMode_clicked();

    void on_pushButton_FlashFirmware_clicked();

private:
    Ui::AdvancedSettings *ui;

    QByteArray file_array_;
    QString flash_button_text_;
    QString enter_to_flash_btn_text_;
    QString default_style_;
    void FlashDone();

    QString tmp_text;
    QString tmp_style;
};

#endif // ADVANCEDSETTINGS_H
