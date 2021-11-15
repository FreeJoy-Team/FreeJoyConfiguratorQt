#include "axescurvesbutton.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QTimer>
#include <QStyle>
#include <cmath>
#include <QStyleOption>

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QBuffer>

#include <QDebug>

AxesCurvesButton::AxesCurvesButton(QWidget *parent) :
    AxesCurvesPlot(false, parent)
    , m_dragStartPos(0, 0)
    , m_toggled(false)
    , m_autoExclusive(false)
    , m_checkable(false)
    , m_draggable(true)
{
    setMouseTracking(false);

    setBorderOffset(5);
    setPointRadius(2);
    setLineWidth(1);
    setProperty("checked", false);
    setProperty("lastChecked", false);
    setProperty("hover", false);
    setProperty("pressed", false);
    setProperty("drop", false);

    installStyleSheet();

    setFocusPolicy(Qt::StrongFocus);
    installEventFilter(this);
    setAcceptDrops(true);
}


void AxesCurvesButton::paintEvent(QPaintEvent *event)
{
    AxesCurvesPlot::paintEvent(event);

    QPainter p(this);
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // Fusion style outline
    QRect r;
    r = opt.rect.adjusted(0, 1, -1, 0);
    if (style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt, this) == 0) {
        QColor darkOutline(window()->palette().window().color().darker(140));

        p.setRenderHint(QPainter::Antialiasing, true);
        p.translate(0.5, -0.5);

        p.setPen(!isEnabled() ? QPen(darkOutline.lighter(115)) : QPen(darkOutline));
        p.drawRoundedRect(r, 2.0, 2.0);

        p.setPen(QColor(255, 255, 255, 30));
        p.drawRoundedRect(r.adjusted(1, 1, -1, -1), 2.0, 2.0);
    }
}

void AxesCurvesButton::installStyleSheet()
{
    setStyleSheet(QStringLiteral(R"(
        AxesCurvesButton[checked="true"], AxesCurvesButton[pressed="true"] {
            background-color: rgba(45, 160, 250, 25);
        }
        AxesCurvesButton[checked="false"] {

        }
        AxesCurvesButton[hover="true"] {
            border: 1px solid;
            border-color: rgb(45,160,250);
        }
        AxesCurvesButton[hover="false"] {
            border: 0px solid;
        }
        AxesCurvesButton[drop="true"] {
            border: 1px solid;
            border-color: rgb(0,200,0);
            background-color: rgba(0, 250, 0, 25);
        }


        AxesCurvesButton[lastChecked="true"] {
            border: 1px solid;
            border-color: rgb(45,160,250);
        }
        AxesCurvesButton[lastChecked="false"][hover="false"][drop="false"] {
            border: 0px solid;
        }
    )"));
}


bool AxesCurvesButton::isChecked() const
{
    return m_toggled;
}

void AxesCurvesButton::setChecked(bool checked)
{
    if (m_checkable) {
        if (checked || m_toggled) {
            for(auto &item : queryButtonList()) {
                if (item->property("lastChecked").toBool()) {
                    item->setProperty("lastChecked", false);
                    style()->polish(item);
                    item->update();
                }
            }
            setProperty("lastChecked", true);
        }

        m_toggled = checked;
        setProperty("checked", m_toggled);

        style()->polish(this);
        update();
        emit toggled(m_toggled);
    }
}

void AxesCurvesButton::setCheckable(bool chackable)
{
    m_checkable = chackable;
}

void AxesCurvesButton::setDraggable(bool draggable)
{
    m_draggable = draggable;
}

bool AxesCurvesButton::autoExclusive() const
{
    return m_autoExclusive;
}

void AxesCurvesButton::setAutoExclusive(bool enabled)
{
    m_autoExclusive = enabled;
}

QList <AxesCurvesButton *> AxesCurvesButton::queryButtonList() const
{
    QList<AxesCurvesButton*>candidates = parent()->findChildren<AxesCurvesButton *>();
    if (m_autoExclusive) {
        auto isNoMemberOfMyAutoExclusiveGroup = [](AxesCurvesButton *candidate) {
            return !candidate->autoExclusive();
        };
        candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                        isNoMemberOfMyAutoExclusiveGroup),
                         candidates.end());
    }
    return candidates;
}

AxesCurvesButton *AxesCurvesButton::queryCheckedButton() const
{
    QList<AxesCurvesButton *> buttonList = queryButtonList();
    if (!m_autoExclusive || buttonList.count() == 1) // no group
        return 0;
    for (int i = 0; i < buttonList.count(); ++i) {
        AxesCurvesButton *b = buttonList.at(i);
        if (isChecked() && b != this)
            return b;
    }
    return isChecked()  ? const_cast<AxesCurvesButton *>(this) : 0;
}


void AxesCurvesButton::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggable) {
        if (!(event->buttons() & Qt::LeftButton)) {
            return;
        }
        if ((event->pos() - m_dragStartPos).manhattanLength() <
                QApplication::startDragDistance()) {
            return;
        }

        setProperty("pressed", false);
        style()->polish(this);

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

//        QPixmap pix(size());
//        render(&pix);

        QImage bitmap(size(), QImage::Format_ARGB32);
        bitmap.fill(Qt::transparent);
        QPainter painter(&bitmap);
        render(&painter);

        QByteArray output;
        QDataStream stream(&output, QIODevice::WriteOnly);
        stream << pointValues();

        mimeData->setData("AxesCurvesPoints", output);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap::fromImage(bitmap));

        drag->exec(Qt::MoveAction); // ???????????????
    }
}

void AxesCurvesButton::mousePressEvent(QMouseEvent *event)
{
    if (m_checkable == false && event->button() == Qt::LeftButton) {
        m_dragStartPos = event->pos();
        setProperty("pressed", true);
        style()->polish(this);
    }
}

void AxesCurvesButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setProperty("pressed", false);

        if (rect().contains(event->pos())) {
            if (m_autoExclusive) {
                if (isChecked() && queryCheckedButton() == this) {  // ???????
                    qDebug()<<"return";
                    return;
                }
                for(auto &item : queryButtonList()) {
                    item->setChecked(false);
                }
            } else {
                int count = 0;
                for(auto &item : queryButtonList()) {
                    if (item->isChecked()) {
                        count++;
                    }
                }
                if (count == 1 && isChecked()) {
                    return;
                }
            }


            if (m_checkable) {
                m_toggled = !m_toggled;
                setProperty("checked", m_toggled);

                for(auto &item : queryButtonList()) {
                    if (item->property("lastChecked").toBool()) {
                        item->setProperty("lastChecked", false);
                        style()->polish(item);
                        item->update();
                    }
                }
                setProperty("lastChecked", true);

                emit toggled(m_toggled);
            }
            emit clicked(m_toggled);
        }

        style()->polish(this);  // ^^^^^^^^^^^^^
        update();
    }
}


void AxesCurvesButton::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("AxesCurvesPoints") && event->source() != this) {
        setProperty("drop", true);
        style()->polish(this);
        repaint();
        event->acceptProposedAction(); // ???????????????
    }
}

void AxesCurvesButton::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)
    setProperty("drop", false);
    style()->polish(this);
    repaint();
}

void AxesCurvesButton::dropEvent(QDropEvent *event)
{
    QDataStream in(event->mimeData()->data("AxesCurvesPoints"));
    QVector<int> vec;
    in >> vec;
    setPointValues(vec);

    setProperty("drop", false);
    style()->polish(this);

    event->acceptProposedAction(); // ???????????????
}


void AxesCurvesButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    setProperty("hover", true);
    style()->polish(this);
    update();
}

void AxesCurvesButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    setProperty("hover", false);
    style()->polish(this);
    update();
}

bool AxesCurvesButton::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    if (event->type() == QEvent::FocusOut) {
        setProperty("pressed", false);
        //style()->polish(this);
    }
    return false;
}

