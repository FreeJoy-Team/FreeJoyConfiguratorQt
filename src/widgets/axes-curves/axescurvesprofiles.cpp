#include "axescurvesprofiles.h"
#include "ui_axescurvesprofiles.h"
#include <QPainter>
#include <QStyleOption>
#include <cmath>

AxesCurvesProfiles::AxesCurvesProfiles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurvesProfiles)
{
    ui->setupUi(this);

    ui->toolButton_Set->setToolTip(tr("Set current axis curve to this preset"));

    ui->widget_Curve->setLineWidth(2);
    ui->widget_Curve->setPointRadius(0);
    ui->widget_Curve->setBorderOffset(10);
    ui->widget_Curve->setGridEnabled(false);

    m_pPresetFunc = &AxesCurvesProfiles::setLinear;

    connect(ui->widget_Curve, &AxesCurvesButton::clicked, this, &AxesCurvesProfiles::CurveClicked);

    updateColor();
    installEventFilter(this);
}

AxesCurvesProfiles::~AxesCurvesProfiles()
{
    delete ui;
}

void AxesCurvesProfiles::retranslateUi()
{
    ui->retranslateUi(this);
}


void AxesCurvesProfiles::setPointValue(int pointIndex, int value)
{
    ui->widget_Curve->setPointValue(pointIndex, value);
}

void AxesCurvesProfiles::setPointValues(const QVector<int> &values)
{
    ui->widget_Curve->setPointValues(values);
}

int AxesCurvesProfiles::pointCount() const
{
    return ui->widget_Curve->pointCount();
}

int AxesCurvesProfiles::pointValue(int pointIndex) const
{
    return ui->widget_Curve->pointValue(pointIndex);
}

QVector<int> AxesCurvesProfiles::pointValues() const
{
    return ui->widget_Curve->pointValues();
}


void AxesCurvesProfiles::setLinear()
{
    AxesCurvesButton *c = ui->widget_Curve;
    if (c->pointCount() < 2) {
        return;
    }
    int interval = (abs(c->minimum()) + abs(c->maximum())) / (c->pointCount() -1);
    QVector<int> vec(ui->widget_Curve->pointCount());

    for (int i = 0; i < ui->widget_Curve->pointCount(); ++i) {
        vec[i] = c->minimum() + interval * i;
    }
    ui->toolButton_Reset->setToolTip(tr("Reset to Linear"));
    m_pPresetFunc = &AxesCurvesProfiles::setLinear;
}

void AxesCurvesProfiles::setLinearInvert()
{
    AxesCurvesButton *c = ui->widget_Curve;
    int count = c->pointCount();
    if (count < 2) {
        return;
    }
    int interval = (abs(c->minimum()) + abs(c->maximum())) / (count -1);
    QVector<int> vec(count);

    for (int i = 0; i < count; ++i) {
        vec[i] = c->maximum() - interval * i;
    }
    c->setPointValues(vec);
    ui->toolButton_Reset->setToolTip(tr("Reset to Linear Invert"));
    m_pPresetFunc = &AxesCurvesProfiles::setLinearInvert;
}

void AxesCurvesProfiles::setExponent()
{
    AxesCurvesButton *c = ui->widget_Curve;
    int count = c->pointCount();

    int range, value, min = c->minimum(), max = c->maximum();
    QVector<int> vec(count);

    if ((min < 0 && max < 0) || (min >= 0 && max >= 0)) {
        range = max - min;
    } else {
        range = abs(min) + abs(max);
    }

    for (int i = 0; i < count; ++i)
    {
        value = qRound(expf(i * logf(range) / (count - 1)) + min);
        if (value <= min + 1) {
            value = min;
        } else if (value >= max - 1) {
            value = max;
        }
        vec[i] = value;
    }
    c->setPointValues(vec);
    ui->toolButton_Reset->setToolTip(tr("Reset to Exponent"));
    m_pPresetFunc = &AxesCurvesProfiles::setExponent;
}

void AxesCurvesProfiles::setExponentInvert()
{
    AxesCurvesButton *c = ui->widget_Curve;
    int count = c->pointCount();

    int range, value, min = c->minimum(), max = c->maximum();
    QVector<int> vec(count);

    if ((min < 0 && max < 0) || (min >= 0 && max >= 0)) {
        range = max - min;
    } else {
        range = abs(min) + abs(max);
    }

    for (int i = 0; i < count; ++i)
    {
        value = qRound(expf(i * logf(range) / (count - 1)) + min);
        if (value <= min + 1) {
            value = min;
        } else if (value >= max - 1) {
            value = max;
        }
        vec[count - 1 - i] = value;
    }
    c->setPointValues(vec);
    ui->toolButton_Reset->setToolTip(tr("Reset to Exponent Invert"));
    m_pPresetFunc = &AxesCurvesProfiles::setExponentInvert;
}

void AxesCurvesProfiles::setShape()
{
    QVector<int> vec {-100, -60, -20, -6, -2, 0, 2, 6, 20, 60, 100};
    ui->widget_Curve->setPointValues(vec);
    ui->toolButton_Reset->setToolTip(tr("Reset to Shape"));
    m_pPresetFunc = &AxesCurvesProfiles::setShape;
}

void AxesCurvesProfiles::setShape2()
{
    QVector<int> vec {-100, -60, -40, -24, -12, 0, 12, 24, 40, 60, 100};
    ui->widget_Curve->setPointValues(vec);
    ui->toolButton_Reset->setToolTip(tr("Reset to Shape2"));
    m_pPresetFunc = &AxesCurvesProfiles::setShape2;
}

void AxesCurvesProfiles::setIDK()
{
    QVector<int> vec {-100, 100, -100, 100, -100, 100, -100, 100, -100, 100, -100};
    ui->widget_Curve->setPointValues(vec);
    ui->toolButton_Reset->setToolTip(tr("Reset to Pulse"));
    m_pPresetFunc = &AxesCurvesProfiles::setIDK;
}

void AxesCurvesProfiles::setIDK2()
{
    QVector<int> vec {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ui->widget_Curve->setPointValues(vec);
    ui->toolButton_Reset->setToolTip(tr("Reset to no Pulse"));
    m_pPresetFunc = &AxesCurvesProfiles::setIDK2;
}


void AxesCurvesProfiles::CurveClicked()
{
    AxesCurvesButton *curve = qobject_cast<AxesCurvesButton*>(sender());
    if (curve) {
        emit presetClicked(curve->pointValues());
    }
}

void AxesCurvesProfiles::on_toolButton_Set_clicked()
{
    emit setClicked();
}

void AxesCurvesProfiles::on_toolButton_Reset_clicked()
{
    m_pPresetFunc(this);
    //emit resetClicked();
}


//! QPixmap gray-scale image (an alpha map) to colored QIcon
QIcon AxesCurvesProfiles::pixmapToIcon(QPixmap pixmap, const QColor &color)
{
    // initialize painter to draw on a pixmap and set composition mode
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    // set color
    painter.setBrush(color);
    painter.setPen(color);
    // paint rect
    painter.drawRect(pixmap.rect());
    // here is our new colored icon
    return QIcon(pixmap);
}

void AxesCurvesProfiles::updateColor()
{
    QColor col = QApplication::palette().color(QPalette::Text);
    ui->toolButton_Reset->setIcon(pixmapToIcon(QPixmap(":/Images/reset.png"), col));
}

bool AxesCurvesProfiles::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)
    if (event->type() == QEvent::PaletteChange) {
        updateColor();
        return false;
    }
    return false;
}
