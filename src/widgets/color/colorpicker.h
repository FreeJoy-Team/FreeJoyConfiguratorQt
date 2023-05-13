#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>

class QColorDialog;

namespace Ui {
class ColorPicker;
}

class ColorPicker : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPicker(QWidget *parent = nullptr);
    ~ColorPicker();

    QColor currentColor() const;
    void setCurrentColor(const QColor &color);

signals:
    void currentColorChanged(const QColor &color);

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void updateCurColor(const QColor &color);

private:
    Ui::ColorPicker *ui;

    QColor m_currentColor;
};

#endif // COLORPICKER_H
