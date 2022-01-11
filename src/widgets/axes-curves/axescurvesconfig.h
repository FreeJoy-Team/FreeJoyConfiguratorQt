#ifndef AXESCURVESCONFIG_H
#define AXESCURVESCONFIG_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QComboBox;
QT_END_NAMESPACE
class AxesCurvesProfiles;

namespace Ui {
class AxesCurvesConfig;
}

class AxesCurvesConfig : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurvesConfig(QWidget *parent = nullptr);
    ~AxesCurvesConfig();

    void readFromConfig();
    void writeToConfig();

    void retranslateUi();

    void setExclusive(bool exclusive);

    void updateAxesCurves();
    void deviceStatus(bool isConnect);

private slots:
    void setPoints(const QVector<int> &values);
    void on_toolButton_Ctrl_toggled(bool checked);

private:
    Ui::AxesCurvesConfig *ui;
    QList<AxesCurvesProfiles *> m_profiles;
};

#endif // AXESCURVESCONFIG_H
