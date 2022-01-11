#ifndef SELECTFOLDER_H
#define SELECTFOLDER_H

#include <QDialog>

namespace Ui {
class SelectFolder;
}

class SelectFolder : public QDialog
{
    Q_OBJECT

public:
    explicit SelectFolder(const QString &folderPath, QWidget *parent = nullptr);
    ~SelectFolder();

    QString folderPath() const;
    void setFolderPath(const QString &path);

private:
    Ui::SelectFolder *ui;
    void selectFolder();
    QStringList cfgFilesList(const QString &dirPath);
};

#endif // SELECTFOLDER_H
