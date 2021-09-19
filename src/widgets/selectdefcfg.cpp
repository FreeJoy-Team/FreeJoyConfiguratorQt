#include "selectdefcfg.h"
#include "ui_selectdefcfg.h"
#include <QFileDialog>

SelectDefCfg::SelectDefCfg(const QString &filePath, bool loadOnStartUp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectDefCfg)
{
    ui->setupUi(this);

    setWindowTitle(tr("Default config file path"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->lineEdit_currentFile->setReadOnly(true);
    ui->lineEdit_currentFile->setText(filePath);

    ui->checkBox_LoadDefCfg->setChecked(loadOnStartUp);

    connect(ui->pushButton_ok, &QPushButton::clicked, this, &SelectDefCfg::accept);

    connect(ui->toolButton_selFolder, &QPushButton::clicked, this, [&] {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open Config"), QDir::currentPath() + "/", tr("Config Files (*.cfg)"));
        ui->lineEdit_currentFile->setText(fileName);
    });
}

SelectDefCfg::~SelectDefCfg()
{
    delete ui;
}


QString SelectDefCfg::filePath() const
{
    return ui->lineEdit_currentFile->text();
}

bool SelectDefCfg::loadOnStartUp() const
{
    return ui->checkBox_LoadDefCfg->isChecked();
}
