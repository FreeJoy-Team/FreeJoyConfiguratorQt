#ifndef AXESCONFIG_H
#define AXESCONFIG_H

#include <QWidget>

namespace Ui {
class AxesConfig;
}

class AxesConfig : public QWidget
{
    Q_OBJECT

public:
    explicit AxesConfig(QWidget *parent = nullptr);
    ~AxesConfig();

private:
    Ui::AxesConfig *ui;
};

#endif // AXESCONFIG_H
