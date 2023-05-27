/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "colorcells.h"
#include <QPainter>
#include <QStyleOption>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QColor>
#include <QMimeData>
#include <QDrag>
#include <QApplication>
#include <QLayout>

//#define DRAG_AND_NDROP

//////////// QWellArray START
namespace {
class QWellArray : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int selectedColumn READ selectedColumn)
    Q_PROPERTY(int selectedRow READ selectedRow)
public:
    QWellArray(int rows, int cols, QWidget* parent=nullptr);
    ~QWellArray() {}
    QString cellContent(int row, int col) const;
    int selectedColumn() const { return selCol; }
    int selectedRow() const { return selRow; }
    virtual void setCurrent(int row, int col);
    virtual void setSelected(int row, int col);
    QSize sizeHint() const override;
    inline int cellWidth() const
    { return cellw; }
    inline int cellHeight() const
    { return cellh; }
    inline int rowAt(int y) const
    { return y / cellh; }
    inline int columnAt(int x) const
    { if (isRightToLeft()) return ncols - (x / cellw) - 1; return x / cellw; }
    inline int rowY(int row) const
    { return cellh * row; }
    inline int columnX(int column) const
    { if (isRightToLeft()) return cellw * (ncols - column - 1); return cellw * column; }
    inline int numRows() const
    { return nrows; }
    inline int numCols() const
    {return ncols; }
    inline QRect cellRect() const
    { return QRect(0, 0, cellw, cellh); }
    inline QSize gridSize() const
    { return QSize(ncols * cellw, nrows * cellh); }
    QRect cellGeometry(int row, int column)
    {
        QRect r;
        if (row >= 0 && row < nrows && column >= 0 && column < ncols)
            r.setRect(columnX(column), rowY(row), cellw, cellh);
        return r;
    }
    inline void updateCell(int row, int column) { update(cellGeometry(row, column)); }
signals:
    void selected(int row, int col);
    void currentChanged(int row, int col);
    void colorChanged(int index, QRgb color);
protected:
    virtual void paintCell(QPainter *, int row, int col, const QRect&);
    virtual void paintCellContents(QPainter *, int row, int col, const QRect&);
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void focusInEvent(QFocusEvent*) override;
    void focusOutEvent(QFocusEvent*) override;
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
private:
    Q_DISABLE_COPY(QWellArray)
    int nrows;
    int ncols;
    int cellw;
    int cellh;
    int curRow;
    int curCol;
    int selRow;
    int selCol;
    int margin;
};

void QWellArray::paintEvent(QPaintEvent *e)
{
    QRect r = e->rect();
    int cx = r.x();
    int cy = r.y();
    int ch = r.height();
    int cw = r.width();
    int colfirst = columnAt(cx);
    int collast = columnAt(cx + cw);
    int rowfirst = rowAt(cy);
    int rowlast = rowAt(cy + ch);
    if (isRightToLeft()) {
        int t = colfirst;
        colfirst = collast;
        collast = t;
    }
    QPainter painter(this);
    QPainter *p = &painter;
    QRect rect(0, 0, cellWidth(), cellHeight());
    if (collast < 0 || collast >= ncols)
        collast = ncols-1;
    if (rowlast < 0 || rowlast >= nrows)
        rowlast = nrows-1;
    // Go through the rows
    for (int r = rowfirst; r <= rowlast; ++r) {
        // get row position and height
        int rowp = rowY(r);
        // Go through the columns in the row r
        // if we know from where to where, go through [colfirst, collast],
        // else go through all of them
        for (int c = colfirst; c <= collast; ++c) {
            // get position and width of column c
            int colp = columnX(c);
            // Translate painter and draw the cell
            rect.translate(colp, rowp);
            paintCell(p, r, c, rect);
            rect.translate(-colp, -rowp);
        }
    }
}
QWellArray::QWellArray(int rows, int cols, QWidget *parent)
    : QWidget(parent)
    ,nrows(rows), ncols(cols)
{
    setFocusPolicy(Qt::StrongFocus);
    cellw = 28;
    cellh = 24;
    curCol = 0;
    curRow = 0;
    selCol = -1;
    selRow = -1;
    margin = 3;
}
QSize QWellArray::sizeHint() const
{
    ensurePolished();
    return gridSize().boundedTo(QSize(640, 480));
}
void QWellArray::paintCell(QPainter* p, int row, int col, const QRect &rect)
{
    const QPalette & g = palette();
    QStyleOptionFrame opt;
    opt.initFrom(this);
    int dfw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt);
    opt.lineWidth = dfw;
    opt.midLineWidth = 1;
    opt.rect = rect.adjusted(margin, margin, -margin, -margin);
    opt.palette = g;
    opt.state = QStyle::State_Enabled | QStyle::State_Sunken;
    style()->drawPrimitive(QStyle::PE_Frame, &opt, p, this);
    //b += dfw;
    if ((row == curRow) && (col == curCol)) {
        if (hasFocus()) {
            QStyleOptionFocusRect opt;
            opt.palette = g;
            opt.rect = rect;
            opt.state = QStyle::State_None | QStyle::State_KeyboardFocusChange;
            style()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, p, this);
        }
    }
    paintCellContents(p, row, col, opt.rect.adjusted(dfw, dfw, -dfw, -dfw));
}
/*
  Reimplement this function to change the contents of the well array.
 */
void QWellArray::paintCellContents(QPainter *p, int row, int col, const QRect &r)
{
    Q_UNUSED(row);
    Q_UNUSED(col);
    p->fillRect(r, Qt::white);
    p->setPen(Qt::black);
    p->drawLine(r.topLeft(), r.bottomRight());
    p->drawLine(r.topRight(), r.bottomLeft());
}
void QWellArray::mousePressEvent(QMouseEvent *e)
{
    // The current cell marker is set to the cell the mouse is pressed in
    QPoint pos = e->pos();
    setCurrent(rowAt(pos.y()), columnAt(pos.x()));
}
void QWellArray::mouseReleaseEvent(QMouseEvent * /* event */)
{
    // The current cell marker is set to the cell the mouse is clicked in
    setSelected(curRow, curCol);
}
/*
  Sets the cell currently having the focus. This is not necessarily
  the same as the currently selected cell.
*/
void QWellArray::setCurrent(int row, int col)
{
    if ((curRow == row) && (curCol == col))
        return;
    if (row < 0 || col < 0)
        row = col = -1;
    int oldRow = curRow;
    int oldCol = curCol;
    curRow = row;
    curCol = col;
    updateCell(oldRow, oldCol);
    updateCell(curRow, curCol);
    emit currentChanged(curRow, curCol);
}
/*
  Sets the currently selected cell to \a row, \a column. If \a row or
  \a column are less than zero, the current cell is unselected.
  Does not set the position of the focus indicator.
*/
void QWellArray::setSelected(int row, int col)
{
    int oldRow = selRow;
    int oldCol = selCol;
    if (row < 0 || col < 0)
        row = col = -1;
    selCol = col;
    selRow = row;
    updateCell(oldRow, oldCol);
    updateCell(selRow, selCol);
    if (row >= 0)
        emit selected(row, col);
#if QT_CONFIG(menu)
    if (isVisible() && qobject_cast<QMenu*>(parentWidget()))
        parentWidget()->close();
#endif
}
void QWellArray::focusInEvent(QFocusEvent*)
{
    updateCell(curRow, curCol);
    emit currentChanged(curRow, curCol);
}
void QWellArray::focusOutEvent(QFocusEvent*)
{
    updateCell(curRow, curCol);
}
void QWellArray::keyPressEvent(QKeyEvent* e)
{
    switch(e->key()) {                        // Look at the key code
    case Qt::Key_Left:                                // If 'left arrow'-key,
        if(curCol > 0)                        // and cr't not in leftmost col
            setCurrent(curRow, curCol - 1);        // set cr't to next left column
        break;
    case Qt::Key_Right:                                // Correspondingly...
        if(curCol < numCols()-1)
            setCurrent(curRow, curCol + 1);
        break;
    case Qt::Key_Up:
        if(curRow > 0)
            setCurrent(curRow - 1, curCol);
        break;
    case Qt::Key_Down:
        if(curRow < numRows()-1)
            setCurrent(curRow + 1, curCol);
        break;
#if 0
    // bad idea that shouldn't have been implemented; very counterintuitive
    case Qt::Key_Return:
    case Qt::Key_Enter:
        /*
          ignore the key, so that the dialog get it, but still select
          the current row/col
        */
        e->ignore();
        // fallthrough intended
#endif
    case Qt::Key_Space:
        setSelected(curRow, curCol);
        break;
    default:                                // If not an interesting key,
        e->ignore();                        // we don't accept the event
        return;
    }
}
}
void QWellArray::resizeEvent(QResizeEvent *e)
{
    cellh = height() / nrows;
    cellw = width() / ncols;
    update();
}
//////////// QWellArray END


//////////// QColorWell START
namespace {
class QColorWell : public QWellArray
{
public:
    QColorWell(QWidget *parent, int r, int c, const QRgb *vals)
        :QWellArray(r, c, parent), values(vals), mousePressed(false), oldCurrent(-1, -1)
    { setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum)); }
protected:
    void paintCellContents(QPainter *, int row, int col, const QRect&) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
#ifdef DRAG_AND_NDROP
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;
#endif
private:
    const QRgb *values;
    bool mousePressed;
    QPoint pressPos;
    QPoint oldCurrent;
};

void QColorWell::paintCellContents(QPainter *p, int row, int col, const QRect &r)
{
    int i = row + col*numRows();
    if (isEnabled()) {
        p->fillRect(r, QColor(values[i]));
    } else {
        // convert to greyscale when disabled
        QRgb rgb;
        rgb = qGray(values[i]);
        rgb = qRgba(rgb, rgb, rgb, qAlpha(rgb) / 3);
        p->fillRect(r, QColor(rgb));
    }
}
void QColorWell::mousePressEvent(QMouseEvent *e)
{
    oldCurrent = QPoint(selectedRow(), selectedColumn());
    QWellArray::mousePressEvent(e);
    mousePressed = true;
    pressPos = e->pos();
}
void QColorWell::mouseMoveEvent(QMouseEvent *e)
{
    QWellArray::mouseMoveEvent(e);
#ifdef DRAG_AND_NDROP
    if (!mousePressed)
        return;
    if ((pressPos - e->pos()).manhattanLength() > QApplication::startDragDistance()) {
        setCurrent(oldCurrent.x(), oldCurrent.y());
        int i = rowAt(pressPos.y()) + columnAt(pressPos.x()) * numRows();
        QColor col(values[i]);
        QMimeData *mime = new QMimeData;
        mime->setColorData(col);
        QPixmap pix(cellWidth(), cellHeight());
        pix.fill(col);
        QPainter p(&pix);
        p.drawRect(0, 0, pix.width() - 1, pix.height() - 1);
        p.end();
        QDrag *drg = new QDrag(this);
        drg->setMimeData(mime);
        drg->setPixmap(pix);
        mousePressed = false;
        drg->exec(Qt::CopyAction);
    }
#endif
}
#ifdef DRAG_AND_NDROP
void QColorWell::dragEnterEvent(QDragEnterEvent *e)
{
    if (qvariant_cast<QColor>(e->mimeData()->colorData()).isValid())
        e->accept();
    else
        e->ignore();
}
void QColorWell::dragLeaveEvent(QDragLeaveEvent *)
{
    if (hasFocus())
        parentWidget()->setFocus();
}
void QColorWell::dragMoveEvent(QDragMoveEvent *e)
{
    if (qvariant_cast<QColor>(e->mimeData()->colorData()).isValid()) {
        setCurrent(rowAt(e->pos().y()), columnAt(e->pos().x()));
        e->accept();
    } else {
        e->ignore();
    }
}
void QColorWell::dropEvent(QDropEvent *e)
{
    QColor col = qvariant_cast<QColor>(e->mimeData()->colorData());
    if (col.isValid()) {
        int i = rowAt(e->pos().y()) + columnAt(e->pos().x()) * numRows();
        emit colorChanged(i, col.rgb());
        e->accept();
    } else {
        e->ignore();
    }
}
#endif // QT_CONFIG(draganddrop)
void QColorWell::mouseReleaseEvent(QMouseEvent *e)
{
    if (!mousePressed)
        return;
    QWellArray::mouseReleaseEvent(e);
    mousePressed = false;
}
}
//////////// QColorWell END


class ColorCellsPrivate {
    Q_DISABLE_COPY(ColorCellsPrivate)
    Q_DECLARE_PUBLIC(ColorCells)
public:
    ColorCellsPrivate(){};
    virtual ~ColorCellsPrivate();

    void init();
    void _q_newStandard(int r, int c);

    ColorCells *q_ptr;
    QWellArray *m_standard;
    static const int m_colorsCount = 48;//48
    static const int m_rowCount = 6;//6
    static const int m_columnCount = 8;//8
    QRgb m_colors[m_colorsCount];
};

ColorCellsPrivate::~ColorCellsPrivate()
{
}

void ColorCellsPrivate::init()
{
    Q_Q(ColorCells);
    int i = 0;
    for (int g = 0; g < 4; ++g)
        for (int r = 0;  r < 4; ++r)
            for (int b = 0; b < 3; ++b)
                m_colors[i++] = qRgb(r * 255 / 3, g * 255 / 3, b * 255 / 2);

    // for lower cells count
    int size = m_rowCount * m_columnCount;
    float scale = (float)m_colorsCount / size;
    for (int i = 0; i < size; ++i) {
        int index = roundf(scale * i);
        m_colors[i] = m_colors[index];
    }

    m_standard = new QColorWell(q, m_rowCount, m_columnCount, m_colors);
    QObject::connect(m_standard, &QColorWell::currentChanged, q, [this](int r, int c){ _q_newStandard(r, c); });
}

void ColorCellsPrivate::_q_newStandard(int r, int c)
{
    Q_Q(ColorCells);
    int index = r + c * m_rowCount;
    if (uint(index) >= m_colorsCount) emit q->currentColorChanged(QColor(Qt::white));
    emit q->currentColorChanged(QColor::fromRgb(m_colors[index]));
}


ColorCells::ColorCells(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new ColorCellsPrivate())
{
    Q_D(ColorCells);
    d->q_ptr = this;

    d->init();
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->addWidget(d->m_standard);
    //mainLay->setAlignment(Qt::AlignCenter);
}

//ColorCells::ColorCells(ColorCellsPrivate &dd, QWidget * parent)
//    :QWidget(parent)
//    ,d_ptr(&dd)
//{
//    Q_D(ColorCells);
//    d->q_ptr = this;
//}

#include "colorcells.moc"
#include "moc_colorcells.cpp"
