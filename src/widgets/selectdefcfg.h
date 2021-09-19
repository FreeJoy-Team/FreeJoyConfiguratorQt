#ifndef SELECTDEFCFG_H
#define SELECTDEFCFG_H

#include <QDialog>

namespace Ui {
class SelectDefCfg;
}

class SelectDefCfg : public QDialog
{
    Q_OBJECT

public:
    explicit SelectDefCfg(const QString &filePath, bool loadOnStartUp, QWidget *parent = nullptr);
    ~SelectDefCfg();

    QString filePath() const;
    bool loadOnStartUp() const;

private:
    Ui::SelectDefCfg *ui;
};

#endif // SELECTDEFCFG_H
