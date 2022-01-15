#include "selectfolder.h"
#include "ui_selectfolder.h"
#include <QFileDialog>

SelectFolder::SelectFolder(const QString &folderPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectFolder)
{
    ui->setupUi(this);

    setWindowTitle(tr("Configs folder path"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->lineEdit_currentFolder->setReadOnly(true);
    ui->lineEdit_currentFolder->setText(folderPath);

    connect(ui->pushButton_ok, &QPushButton::clicked, this, &SelectFolder::accept);

    connect(ui->toolButton_selFolder, &QPushButton::clicked, this, &SelectFolder::selectFolder);
}

SelectFolder::~SelectFolder()
{
    delete ui;
}

QString SelectFolder::folderPath() const
{
    return ui->lineEdit_currentFolder->text();
}

void SelectFolder::setFolderPath(const QString &path)
{
    if (path == ui->lineEdit_currentFolder->text()) { ////////// ????????????????
        return;
    }
    ui->lineEdit_currentFolder->setText(path);
    QStringList list = cfgFilesList(path);
}

void SelectFolder::selectFolder()
{
    QString folder = QFileDialog::getExistingDirectory(0,
                                                       (tr("Select configs folder")),
                                                       ui->lineEdit_currentFolder->text());
    if (folder.isEmpty()) {
        return;
    }
    setFolderPath(folder);
}

// get config file list
QStringList SelectFolder::cfgFilesList(const QString &dirPath)
{
    QDir dir(dirPath);
    QStringList cfgs = dir.entryList(QStringList() << "*.cfg", QDir::Files);
    for (auto &line : cfgs) {
        line.remove(line.size() - 4, 4);// 4 = ".cfg" characters count
    }
    cfgs.sort(Qt::CaseInsensitive);
    return cfgs;
}
