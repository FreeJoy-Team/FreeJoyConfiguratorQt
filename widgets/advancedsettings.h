#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include <QWidget>
#include "widgets/flasher.h"

QT_BEGIN_NAMESPACE
class QFile;
class QPushButton;
QT_END_NAMESPACE
//#include <QFile>
//#include <QPushButton>

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
    
    Flasher* GetFlasher();

signals:
    void languageChanged(QString language);
    void interfaceStyleChanged(bool is_dark);

    void fontChanged();

private slots:
    void on_pushButton_LangEnglish_clicked();
    void on_pushButton_LangRussian_clicked();

    void on_pushButton_StyleDefault_clicked();

    void on_pushButton_StyleWhite_clicked();

    void on_pushButton_StyleDark_clicked();

    void on_spinBox_FontSize_valueChanged(int font_size);

    void on_pushButton_About_clicked();

    void on_pushButton_Wiki_clicked();

private:
    Ui::AdvancedSettings *ui;

    void SetStyle(QPushButton* pressed_button, QString file_name, QString style_name, bool is_dark); //hz
    
    Flasher* flasher_;

    QString tmp_text_;
    QString tmp_style_;
};

#endif // ADVANCEDSETTINGS_H
