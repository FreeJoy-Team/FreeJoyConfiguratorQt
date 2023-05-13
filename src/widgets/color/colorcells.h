
#ifndef COLORCELLS_H
#define COLORCELLS_H


#include <QWidget>

class ColorCellsPrivate;

class ColorCells : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ColorCells)
public:
    ColorCells(QWidget *parent = nullptr);

signals:
    void currentColorChanged(const QColor &color);

protected:
    ColorCellsPrivate *const d_ptr;
    //ColorCells(ColorCellsPrivate &dd, QWidget * parent);
};

#endif // COLORCELLS_H
