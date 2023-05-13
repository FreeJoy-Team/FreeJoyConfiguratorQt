#include <QCheckBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common_types.h"
#include "global.h"
#include "deviceconfig.h"

#include <QDebug>
#include <QToolTip>

#if defined Q_OS_WIN && _MSC_VER
    #include <dwmapi.h>
    #pragma comment (lib,"Dwmapi.lib") // fixes error LNK2019: unresolved external symbol __imp__DwmExtendFrameIntoClientArea

    enum : WORD {
        DwmwaUseImmersiveDarkMode = 20,
        DwmwaUseImmersiveDarkModeBefore20h1 = 19
    };

    bool setDarkBorderToWindow(HWND hwnd, bool dark)
    {
        const BOOL darkBorder = dark ? TRUE : FALSE;
        const bool ok =
                SUCCEEDED(DwmSetWindowAttribute(hwnd, DwmwaUseImmersiveDarkMode, &darkBorder, sizeof(darkBorder)))
                || SUCCEEDED(DwmSetWindowAttribute(hwnd, DwmwaUseImmersiveDarkModeBefore20h1, &darkBorder, sizeof(darkBorder)));
        if (!ok) {
            qDebug()<<QString("%1: Unable to set %2 window border.").arg(__FUNCTION__, dark ? "dark" : "light");
        }
        return ok;
    }
#endif

////////////////////////////////////////////////// style //////////////////////////////////////////////////
// i cannot use qApp->setStyleSheet() because it takes a long time. groupBox_LogicalButtons contains a lot of elements
// this trick skips groupBox_LogicalButtons. If anyone has any ideas on how to do this better, please tell me
void MainWindow::themeChanged(bool dark)
{
    // ЕБАНЫЙ БЛЯДЬ groupBox_LogicalButtons СУКА НАХУЙ!! В НЁМ ДОХУИЩЕ ЕБУЧИХ ЭЛЕМЕНТОВ И СТАЙЛЁБАНЫЕШИТЫ ПРИМЕНЯЮТСЯ ХУЙЛИАРД ЛЕТ
    // Я УЖЕ НЕ ЗНАЮ КАК ИЗЪЕБНУТЬСЯ ЧТОБЫ УСКОРИТЬ ЭТО ДЕРЬМО
    static QList<QGroupBox *> groupBoxes;

    QString styleName;
    ui->tabWidget->setDocumentMode(true);

    if (groupBoxes.isEmpty()) {
        for (auto &child : window()->findChildren<QGroupBox *>()) {
            groupBoxes.append(child);
        }
    }

    if (dark == false)
    {
        //qApp->setPalette(QPalette());
        QPalette pal(QColor(240, 240, 240));
        pal.setColor(QPalette::Disabled, QPalette::Button, QColor(210, 210, 210));
        pal.setColor(QPalette::Dark, QColor(216, 216, 216));     // qframe
        QToolTip::setPalette(pal);
        qApp->setPalette(pal);

        for (int i = 0; i < groupBoxes.size(); ++i) {
            if (groupBoxes[i]->objectName() == QStringLiteral("groupBox_LogicalButtons") ||
                    groupBoxes[i]->objectName() == QStringLiteral("groupBox_PhysicalButtons")) {
                continue;
            }

            if (groupBoxes[i]->objectName() == QStringLiteral("groupBox_AxixName")) {
                groupBoxes[i]->setStyleSheet(QStringLiteral(R"(
                        QGroupBox {
                             background: none;
                             border: 1px solid #d8d8d8;
                             border-radius: 3px;
                             margin-top: 2ex;
                             padding: 6px 0px 0px 0px ;
                         }

                        QGroupBox::title
                        {
                            color:#000000;
                            subcontrol-position: top left;
                            subcontrol-origin: margin;
                            left: 6px;
                            padding: -2px 0px 0px 2px;
                            background: none;
                         }
                    )"));
                continue;
            }
            groupBoxes[i]->setStyleSheet(QStringLiteral(R"(
                    QGroupBox {
                        background: none;
                        border: 1px solid #d8d8d8;
                        border-radius: 3px;
                        margin-top: 2ex;
                        padding: 6px 0px 0px 0px ;
                     }

                    QGroupBox::title
                    {
                        color:#000000;
                        subcontrol-position: top center;
                        subcontrol-origin: margin;
                        left:-3px;
                        padding: -2px 0px 0px 2px;
                        background: none;
                     }
                )"));
        }

        ui->pushButton_Wiki->setStyleSheet(QStringLiteral(R"(
                QPushButton#pushButton_Wiki {
                    border: 1px solid;
                    border-color: rgb(173, 173, 173);
                    border-radius:15px;
                    padding:0px;
                    margin: 0px;
                    min-width: 30;
                    max-width: 30;
                    max-height: 30;
                    min-height: 30;
                    width: 30;
                    height: 30;
                }

                QPushButton#pushButton_Wiki:hover {
                    border: 1px solid;
                    border-color: rgb(0, 120, 215);
                    background-color: rgb(229, 241, 251);
                    border-radius:15px;
                }
            )"));

        // stylesheet icon: url(...); does not work in linux?
        ui->pushButton_Wiki->setIcon(QIcon(":/Images/ST_wiki.png"));
        styleName = "white";
#if defined Q_OS_WIN && _MSC_VER
        setDarkBorderToWindow((HWND)window()->winId(), false);
#endif
    }
    else
    {
        //qApp->setPalette(QPalette());
        QPalette pal;
        pal.setColor(QPalette::Window, QColor(52,54,58));//QColor(54,57,63));
        pal.setColor(QPalette::Button, QColor(52,54,58));
        pal.setColor(QPalette::Disabled, QPalette::Button, QColor(40,41,45));
        pal.setColor(QPalette::Base, QColor(47,48,52));//47,48,52//72,74,78
        pal.setColor(QPalette::Disabled, QPalette::Base, QColor(35,36,40));
        pal.setColor(QPalette::AlternateBase, QColor(66,66,66));
        pal.setColor(QPalette::ToolTipBase, QColor(75,76,77));

        pal.setColor(QPalette::Dark, QColor(66,66,66));     // qframe
        pal.setColor(QPalette::Light, QColor(66,66,66));    // qframe
        pal.setColor(QPalette::Shadow, QColor(20,20,20));

        pal.setColor(QPalette::Text, QColor(230,231,232));
        pal.setColor(QPalette::Disabled, QPalette::Text, QColor(127,127,127));
        pal.setColor(QPalette::WindowText, QColor(230,231,232));
        pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127,127,127));
        pal.setColor(QPalette::ToolTipText, QColor(230,231,232));
        pal.setColor(QPalette::ButtonText, QColor(230,231,232));
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127,127,127));
        pal.setColor(QPalette::BrightText, Qt::red);
        pal.setColor(QPalette::Link, QColor(20,100,170));
        pal.setColor(QPalette::Highlight, QColor(20,100,170));
        pal.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80,80,80));
        pal.setColor(QPalette::HighlightedText, QColor(230,231,232));
        pal.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127,127,127));
        QToolTip::setPalette(pal);
        qApp->setPalette(pal);

        for (int i = 0; i < groupBoxes.size(); ++i) {
            if (groupBoxes[i]->objectName() == QStringLiteral("groupBox_LogicalButtons") ||
                    groupBoxes[i]->objectName() == QStringLiteral("groupBox_PhysicalButtons")) {
                continue;
            }

            if (groupBoxes[i]->objectName() == QStringLiteral("groupBox_AxixName")) {
                groupBoxes[i]->setStyleSheet(QStringLiteral(R"(
                        QGroupBox {
                             background: none;
                             border: 1px solid #414246;
                             border-radius: 3px;
                             margin-top: 2ex;
                             padding: 6px 0px 0px 0px ;
                         }

                        QGroupBox::title
                        {
                            color:#ffffff;
                            subcontrol-position: top left;
                            subcontrol-origin: margin;
                            left: 6px;
                            padding: -2px 0px 0px 2px;
                            background: none;
                         }
                    )"));
                continue;
            }
            groupBoxes[i]->setStyleSheet(QStringLiteral(R"(
                    QGroupBox {
                        font-weight: bold;
                        background: none;
                        border: 1px solid #414246;
                        border-radius: 3px;
                        margin-top: 2ex;
                        padding: 6px 0px 0px 0px ;
                     }

                    QGroupBox::title
                    {
                        color:#ffffff;
                        subcontrol-position: top center;
                        subcontrol-origin: margin;
                        left:-3px;
                        padding: -2px 0px 0px 2px;
                        background: none;
                     }
                )"));
        }

        ui->pushButton_Wiki->setStyleSheet(QStringLiteral(R"(
                QPushButton#pushButton_Wiki {
                    border: 1px solid;
                    border-color: rgb(173, 173, 173);
                    border-radius:15px;
                    padding:0px;
                    margin: 0px;
                    min-width: 30;
                    max-width: 30;
                    max-height: 30;
                    min-height: 30;
                    width: 30;
                    height: 30;
                }

                QPushButton#pushButton_Wiki:hover {
                    border: 1px solid;
                    border-color: rgb(45,160,250);
                    background-color: rgb(20,90,150);
                    border-radius:15px;
                }
            )"));

        // stylesheet icon: url(...); does not work in linux?
        ui->pushButton_Wiki->setIcon(QIcon(":/Images/ST_wiki_dark.png"));
        styleName = "dark";
#if defined Q_OS_WIN && _MSC_VER
        setDarkBorderToWindow((HWND)window()->winId(), true);
#endif
    }

    updateColor();

    gEnv.pAppSettings->beginGroup("StyleSettings");
    gEnv.pAppSettings->setValue("StyleSheet", styleName);
    gEnv.pAppSettings->endGroup();
}
