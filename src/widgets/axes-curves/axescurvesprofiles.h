#ifndef AXESCURVESPROFILES_H
#define AXESCURVESPROFILES_H

#include <QWidget>
#include <functional>

namespace Ui {
class AxesCurvesProfiles;
}

class AxesCurvesProfiles : public QWidget
{
    Q_OBJECT

public:
    explicit AxesCurvesProfiles(QWidget *parent = nullptr);
    ~AxesCurvesProfiles();
    void retranslateUi();

    void setPointValue(int pointIndex, int value);
    void setPointValues(const QVector<int> &values);

    int pointCount() const;
    int pointValue(int pointIndex) const;
    QVector<int> pointValues() const;

    void setLinear();
    void setLinearInvert();
    void setExponent();
    void setExponentInvert();
    void setShape();
    void setShape2();
    void setIDK();
    void setIDK2();

signals:
    void presetClicked(QVector <int> values);
    void setClicked();
    void resetClicked();

private slots:
    void CurveClicked();

    void on_toolButton_Set_clicked();
    void on_toolButton_Reset_clicked();

private:
    Ui::AxesCurvesProfiles *ui;
    //void (AxesCurvesProfiles::*m_pPresetFunc)();
    std::function<void(AxesCurvesProfiles*)> m_pPresetFunc;
};

#endif // AXESCURVESPROFILES_H
