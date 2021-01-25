#include "axescurvesprofiles.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QTimer>
#include <QStyle>
#include <cmath>

#include <QDebug>
AxesCurvesProfiles::AxesCurvesProfiles(QWidget *parent) :
    AxesCurvesPlot(false, parent)
{
    //setMouseTracking(false);
    m_toggled = false;

    setBorderOffset(2);
    setPointRadius(2);
    setLineWidth(1);


    setStyleSheet("AxesCurvesProfiles[checked=""true""] {background-color: rgb(170, 0, 255); border: 15px solid #32414B;border-radius: 4px;}");

    //installEventFilter(this);
}

//#include <QApplication>
//#include <QStyleOptionButton>
//#include <QStylePainter>
//bool AxesCurvesProfiles::eventFilter(QObject *obj, QEvent *event)
//{
//    if (event->type() == QEvent::Paint)
//    {
//        obj->removeEventFilter(this);
//        QApplication::sendEvent(obj, event);
//        obj->installEventFilter(this);

//        QStylePainter p(this);
//        QStyleOptionButton option;
//        option.state |= QStyle::State_On;
//        option.state |= QStyle::State_HasFocus;
//        p.drawControl(QStyle::CE_PushButton, option);

//        return true;
//    }
//    return false;
//}

void AxesCurvesProfiles::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void AxesCurvesProfiles::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        qDebug()<<"press";
        setProperty("checked", true);
        //style()->unpolish(this);
        style()->polish(this);
        //update();
    }
}

void AxesCurvesProfiles::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        qDebug()<<"unpress";
        setProperty("checked", false);
        //style()->unpolish(this);
        style()->polish(this);
        //update();

        m_toggled = !m_toggled;
    }
}

void AxesCurvesProfiles::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    //setStyleSheet("border: 3px solid #31363b;");
}

void AxesCurvesProfiles::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    //setStyleSheet("border: 0px solid #31363b;");
}

