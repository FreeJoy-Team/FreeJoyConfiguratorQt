#include "axesconfig.h"
#include "ui_axesconfig.h"

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
    }
}

AxesConfig::~AxesConfig()
{
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

void AxesConfig::addOrDeleteMainSource(int sourceEnum, bool isAdd)
{
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        m_axesPtrList[i]->addOrDeleteMainSource(sourceEnum, isAdd);
    }
}

void AxesConfig::axesValueChanged()
{
    for (int i = 0; i < MAX_AXIS_NUM; ++i) {
        m_axesPtrList[i]->updateAxisOut();
        m_axesPtrList[i]->updateAxisRaw();
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
