#include "axesconfig.h"
#include "ui_axesconfig.h"

AxesConfig::AxesConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesConfig)
{
    ui->setupUi(this);
    a2b_buttons_count_ = 0;

    ui->layoutV_Axes->setAlignment(Qt::AlignTop);
    // axes spawn
    for (int i = 0; i < MAX_AXIS_NUM; i++)
    {
        Axes * axis = new Axes(i, this);
        ui->layoutV_Axes->addWidget(axis);
        AxesPtrList_.append(axis);
        connect(axis, SIGNAL(a2bCountChanged(int, int)),
                this, SLOT(a2bCountCalc(int, int)));
    }
}

AxesConfig::~AxesConfig()
{
    delete ui;
}

void AxesConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < AxesPtrList_.size(); ++i) {
        AxesPtrList_[i]->RetranslateUi();
    }
}

void AxesConfig::a2bCountCalc(int count, int previous_count)
{
    a2b_buttons_count_ += count - previous_count;
    emit a2bCountChanged(a2b_buttons_count_);
}

void AxesConfig::addOrDeleteMainSource(int source_enum, bool is_add)
{
    for (int i = 0; i < AxesPtrList_.size(); ++i) {
        AxesPtrList_[i]->AddOrDeleteMainSource(source_enum, is_add);
    }
}

void AxesConfig::AxesValueChanged()
{
    for (int i = 0; i < AxesPtrList_.size(); ++i) {
        AxesPtrList_[i]->UpdateAxisOut();
        AxesPtrList_[i]->UpdateAxisRaw();
    }
}

void AxesConfig::ReadFromConfig()
{
    for (int i = 0; i < MAX_AXIS_NUM; i++)
    {
        AxesPtrList_[i]->ReadFromConfig();
    }
}

void AxesConfig::WriteToConfig()
{
    for (int i = 0; i < MAX_AXIS_NUM; i++)
    {
        AxesPtrList_[i]->WriteToConfig();
    }
}
