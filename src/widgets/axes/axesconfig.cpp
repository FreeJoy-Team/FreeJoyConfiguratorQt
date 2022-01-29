#include "axesconfig.h"
#include "ui_axesconfig.h"
#include <QCheckBox>
#include <QSettings>
#include "global.h"

AxesConfig::AxesConfig(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AxesConfig)
{
    ui->setupUi(this);
    m_a2bButtonsCount = 0;

    ui->layoutV_Axes->setAlignment(Qt::AlignTop);
    // axes spawn
    for (int i = 0; i < MAX_AXIS_NUM; i++) {
        Axes *axis = new Axes(i, this);
        ui->layoutV_Axes->addWidget(axis);
        m_axesPtrList.append(axis);
        connect(axis, SIGNAL(a2bCountChanged(int, int)),
                this, SLOT(a2bCountCalc(int, int)));
        // added hidden axes checkboxes
        QCheckBox *chb = new QCheckBox(axesList()[i].guiName, this);
        ui->layoutH_HiddenAxes->addWidget(chb);
        chb->setProperty("index", i);
        m_hideChBoxes.append(chb);
        connect(chb, &QCheckBox::toggled, this, [&](bool hide) {
            hideAxis(sender()->property("index").toInt(), hide);
        });
    }

    gEnv.pAppSettings->beginGroup("OtherSettings");
    for (int i = 0; i < m_hideChBoxes.size(); ++i) {
        m_hideChBoxes[i]->setChecked(gEnv.pAppSettings->value(axesList()[i].guiName, "false").toBool());
    }
    gEnv.pAppSettings->endGroup();
}

AxesConfig::~AxesConfig()
{
    gEnv.pAppSettings->beginGroup("OtherSettings");
    for (int i = 0; i < m_hideChBoxes.size(); ++i) {
        gEnv.pAppSettings->setValue(axesList()[i].guiName, m_hideChBoxes[i]->isChecked());
    }
    gEnv.pAppSettings->endGroup();
    delete ui;
}

void AxesConfig::retranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        m_axesPtrList[i]->retranslateUi();
    }
}

void AxesConfig::a2bCountCalc(int count, int previousCount)
{
    m_a2bButtonsCount += count - previousCount;
    emit a2bCountChanged(m_a2bButtonsCount);
}

void AxesConfig::addOrDeleteMainSource(int sourceEnum, QString sourceName, bool isAdd)
{
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        m_axesPtrList[i]->addOrDeleteMainSource(sourceEnum, sourceName, isAdd);
    }
}

void AxesConfig::axesValueChanged()
{
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        m_axesPtrList[i]->updateAxisOut();
        m_axesPtrList[i]->updateAxisRaw();
    }
}

void AxesConfig::hideAxis(int index, bool hide)
{
    if (index < 0) index = 0;
    else if (index >= m_axesPtrList.size()) index = m_axesPtrList.size() -1;

    if (hide) {
        m_axesPtrList[index]->hide();
    } else {
        m_axesPtrList[index]->show();
    }
}

void AxesConfig::readFromConfig()
{
    for (int i = 0; i < MAX_AXIS_NUM; i++) {
        m_axesPtrList[i]->readFromConfig();
    }
}

void AxesConfig::writeToConfig()
{
    for (int i = 0; i < MAX_AXIS_NUM; i++) {
        m_axesPtrList[i]->writeToConfig();
    }
}
