#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include "widgets/flasher.h"
#include <QWidget>

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

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

    Flasher *flasher() const; // const?

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

    void on_spinBox_FontSize_valueChanged(int fontSize);
    void on_pushButton_About_clicked();
    void on_pushButton_Wiki_clicked();

private:
    Ui::AdvancedSettings *ui;

    void setStyle(QPushButton *pressedButton, QString filePath, QString styleName, bool isDark); //hz

    Flasher *m_flasher;

    QString m_default_text;
    QString m_default_style;
};

#endif // ADVANCEDSETTINGS_H
