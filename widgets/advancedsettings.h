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

signals:
    void languageChanged(QString language);
    void interfaceStyleChanged(bool is_dark);

    //void styleChanged();

private slots:
    //void styleButtonWaiting();

    void on_pushButton_LangEnglish_clicked();
    void on_pushButton_LangRussian_clicked();

    void on_pushButton_StyleDefault_clicked();

    void on_pushButton_StyleWhite_clicked();

    void on_pushButton_StyleDark_clicked();

    void on_spinBox_FontSize_valueChanged(int arg1);

private:
    Ui::AdvancedSettings *ui;

    QString tmp_text;
    QString tmp_style;
};

#endif // ADVANCEDSETTINGS_H
