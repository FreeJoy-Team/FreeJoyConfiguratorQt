#include "axescurvesconfig.h"
#include "ui_axescurvesconfig.h"
#include <QSettings>
#include <QKeyEvent>
#include <QBuffer>
#include <QPainter>
#include <QDebug>

#include "axescurves.h"
#include "axescurvesprofiles.h"
#include "deviceconfig.h"
#include "global.h"

static const int PROFILES_COUNT = 8;

AxesCurvesConfig::AxesCurvesConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesCurvesConfig)
{
    ui->setupUi(this);

    for (int i = 0; i < PROFILES_COUNT; ++i) {
        AxesCurvesProfiles *p = new AxesCurvesProfiles(this);
        m_profiles.append(p);
        p->setProperty("index", i);
        ui->layoutH_Profiles->addWidget(p);
        switch (i) {
        case 0 : p->setLinear();
            break;
        case 1 : p->setLinearInvert();
            break;
        case 2 : p->setExponent();
            break;
        case 3 : p->setExponentInvert();
            break;
        case 4 : p->setShape();
            break;
        case 5 : p->setShape2();
            break;
        case 6 : p->setIDK();
            break;
        case 7 : p->setIDK2();
            break;
        default:
            break;
        }
        connect(p, &AxesCurvesProfiles::presetClicked, ui->widget_AxesCurves, &AxesCurves::setPointValues);
        connect(p, &AxesCurvesProfiles::setClicked, [this, p](){
            p->setPointValues(ui->widget_AxesCurves->pointValues());
        });
    }

    // load curves profiles from app config file
    QSettings *appS = gEnv.pAppSettings;
    appS->beginGroup("CurvesProfiles");
    bool reset = appS->value("Reset", true).toBool();
    if (reset == false) {
        for (int i = 0; i < PROFILES_COUNT; ++i) {
            appS->beginReadArray("Curve_" + QString::number(i + 1));
            for (int j = 0; j < m_profiles[i]->pointCount(); ++j) {
                appS->setArrayIndex(j);
                m_profiles[i]->setPointValue(j, appS->value("Point_" + QString::number(j), AXIS_MIN_VALUE + 20*j).toInt());
            }
            appS->endArray();
        }
    }
    appS->endGroup();

    ui->layoutV_Help->setAlignment(Qt::AlignBottom);
    setFocusPolicy(Qt::StrongFocus);

    m_toolTip = ui->info->toolTip();
    updateColor();
    installEventFilter(this);
}

AxesCurvesConfig::~AxesCurvesConfig()
{
    // save curves profiles to app config file
    QSettings *appS = gEnv.pAppSettings;
    appS->beginGroup("CurvesProfiles");
    appS->setValue("Reset", false);
    for (int i = 0; i < PROFILES_COUNT; ++i) {
        appS->beginWriteArray("Curve_" + QString::number(i + 1));
        for (int j = 0; j < m_profiles[i]->pointCount(); ++j) {
            appS->setArrayIndex(j);
            appS->setValue("Point_" + QString::number(j), m_profiles[i]->pointValue(j));
        }
        appS->endArray();
    }
    appS->endGroup();
    delete ui;
}


void AxesCurvesConfig::retranslateUi()
{
    ui->retranslateUi(this);
    ui->widget_AxesCurves->retranslateUi();
    for (int i = 0; i < m_profiles.size(); ++i) {
        m_profiles[i]->retranslateUi();
    }
}

void AxesCurvesConfig::setPoints(const QVector<int> &values)
{
    ui->widget_AxesCurves->setPointValues(values);
}

void AxesCurvesConfig::setExclusive(bool exclusive)
{
    ui->widget_AxesCurves->setExclusive(exclusive);

    if (sender() != ui->toolButton_Ctrl) {
        ui->toolButton_Ctrl->setChecked(!exclusive);
    }
    if (exclusive) {
        ui->toolButton_Ctrl->setPalette(window()->palette());
    } else {
        QPalette pal(window()->palette());
        pal.setColor(QPalette::Button, QColor(0, 128, 0));
        ui->toolButton_Ctrl->setPalette(pal);
    }
}

void AxesCurvesConfig::on_toolButton_Ctrl_toggled(bool checked)
{
    setExclusive(!checked);
}


void AxesCurvesConfig::updateAxesCurves()
{
    ui->widget_AxesCurves->updateAxis();
}

void AxesCurvesConfig::deviceStatus(bool isConnect)
{
    ui->widget_AxesCurves->deviceStatus(isConnect);
}

//! QPixmap gray-scale image (an alpha map) to colored QPixmap
QPixmap AxesCurvesConfig::coloringPixmap(QPixmap pixmap, const QColor &color)
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
    return pixmap;
}

void AxesCurvesConfig::updateColor()
{
    QColor col = QApplication::palette().color(QPalette::Text);
    QPixmap pix = coloringPixmap(QPixmap(":/Images/reset.png"), col);

    QByteArray arr;
    QBuffer buf(&arr);
    pix.save(&buf, "PNG");
    QString url = QString("data:image/png;base64,") + arr.toBase64();
    QString tt = m_toolTip;
    tt.replace(":/Images/reset.png", url);

    ui->info->setToolTip(tt);
}

bool AxesCurvesConfig::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)
    if (event->type() == QEvent::PaletteChange) {
        updateColor();
        return false;
    }
    return false;
}


void AxesCurvesConfig::readFromConfig()
{
    ui->widget_AxesCurves->readFromConfig();
}

void AxesCurvesConfig::writeToConfig()
{
    ui->widget_AxesCurves->writeToConfig();
}
