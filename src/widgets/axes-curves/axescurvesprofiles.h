#ifndef AXESCURVESPROFILES_H
#define AXESCURVESPROFILES_H

#include <QPushButton>
#include "axescurvesplot.h"

class AxesCurvesProfiles : public AxesCurvesPlot
{
    Q_OBJECT

public:
    explicit AxesCurvesProfiles(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

//    void enterEvent(QEvent *event) override;
//    void leaveEvent(QEvent *event) override;
//bool eventFilter(QObject *obj, QEvent *event) override;
private:
    bool m_toggled;
};

#endif // AXESCURVESPROFILES_H
