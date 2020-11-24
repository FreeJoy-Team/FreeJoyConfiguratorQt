#include "axescurvesplot.h"
#include "ui_axescurvesplot.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QTimer>
#include <cmath>

#include <QDebug>


AxesCurvesPlot::AxesCurvesPlot(QWidget *parent) :   // в коде много мусора, костыля для выравнивания TODO
    QWidget(parent),                                // при смене стиля менять цвет сетки в эвенте
    ui(new Ui::AxesCurvesPlot)
{
    ui->setupUi(this);
    setMouseTracking(true);

    m_pointsCount = CURVE_PLOT_POINTS_COUNT; // axescurvesconfig
    m_pointActive = false;
    m_isDeviceConnect = false;
    m_halfRadius = m_kRadius/2;

    // если вкладка не открыта, то размер не загружается и высота = 0. т.к. при изменении размера окна у меня не меняется высота,
    // то захардкодил 450, но это херовое решение
    m_height = 450;
    setMinimumHeight(m_height);

    m_curAxisPos.color = m_kPointCurrentPosColor;

    int tmp_range;
    if ((m_kMinPointValue < 0 && m_kMaxPointValue < 0) || (m_kMinPointValue >= 0 && m_kMaxPointValue >= 0)) {
        tmp_range = m_kMaxPointValue - m_kMinPointValue;
    }
    else {
        tmp_range = abs(m_kMinPointValue) + abs(m_kMaxPointValue);
    }

    for (int i = 0; i < m_pointsCount; ++i){
        AxesCurve_point* point = new AxesCurve_point;

        QLabel* label = new QLabel(this);
        m_labelPtrList.append(label);

        m_pointPtrList.append(point);
        m_pointPtrList[i]->color = m_kPointInactiveColor;
        m_pointPtrList[i]->posX = 0;
        m_pointPtrList[i]->posY = 0;
        m_pointPtrList[i]->is_drag = false;
        m_pointPtrList[i]->current_value = ((tmp_range / (m_pointsCount - 1.0)) * i) + m_kMinPointValue;    // hz

        m_labelPtrList[i]->setFont(QFont("MS Shell Dlg 2", 8));
        m_labelPtrList[i]->setNum(m_pointPtrList[i]->current_value);
        m_labelPtrList[i]->setMinimumWidth(m_kLabelWidth);
        m_labelPtrList[i]->setAlignment(Qt::AlignHCenter);
        m_labelPtrList[i]->setStyleSheet("background:transparent;");
        m_labelPtrList[i]->setVisible(true);
    }
}

AxesCurvesPlot::~AxesCurvesPlot()
{
    delete ui;
}
        ////////////////// half_radius_ - убрать                ///                 /// half_radius_
        /// сделать SetPointValue в % ?
#include <QElapsedTimer>
void AxesCurvesPlot::paintEvent(QPaintEvent *event)     // жирно, можно оптимизировать
{
    Q_UNUSED(event)
    QPainter painter;
    int tmp_x, tmp_y;

    painter.begin(this);
    if (QWidget::palette().color(QWidget::backgroundRole()).value() < 80) {
        painter.setPen(QColor(110,110,110));
    } else {
        painter.setPen(Qt::lightGray);
    }
//    #include <QElapsedTimer>
//    QElapsedTimer timer;
//    timer.start();
//    for (int i = 0; i < 1000; ++i) {
//    }
//    qDebug()<<timer.elapsed();

    //paint columns //330 67
    for (int i = 0; i < m_kColumnsCount + 1; ++i){
        tmp_x = (i * m_columnWidth) + m_kOffset;
        painter.drawLine(tmp_x, m_kOffset, tmp_x, m_height - m_kOffset);
    }
    // paint rows
    for (int i = 0; i < m_kRowsCount + 1; ++i){
        tmp_y = (i * m_rowHeight) + m_kOffset;
        painter.drawLine(m_kOffset, tmp_y, m_width - m_kOffset, tmp_y);
    }

    // Antialiasing         // можно светху но всё станет мазнёй //105 21
    painter.setRenderHint(QPainter::Antialiasing, true);
    // paint line       // можно в цикл paint rect, но лучше отдельно
    QPen pen;
    pen.setWidth(2);
    pen.setColor(m_kPointInactiveColor);
    for (int i = 0; i < m_pointPtrList.size(); ++i)   // проверка на драг=труе + соседние
    {
        painter.setPen(pen);
        if (i < m_pointPtrList.size() - 1){
            painter.drawLine(m_pointPtrList[i]->posX  + m_halfRadius, m_pointPtrList[i]->posY  + m_halfRadius,
                             m_pointPtrList[i + 1]->posX  + m_halfRadius, m_pointPtrList[i + 1]->posY  + m_halfRadius);
        }
    }

    // coordinates for rect //250 78
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        m_pointPtrList[i]->area.setRect(m_pointPtrList[i]->posX, m_pointPtrList[i]->posY,
                                      m_kRadius, m_kRadius);
    }
    // paint rect
    //painter.setPen(Qt::lightGray);
    for (int i = 0; i < m_pointPtrList.size(); ++i)   // проверка на драг=труе
    {
        painter.setPen(m_pointPtrList[i]->color);
        painter.setBrush(m_pointPtrList[i]->color);
        painter.drawEllipse(m_pointPtrList[i]->area);

        painter.setPen(m_kPointMoveColor);
        painter.setBrush(m_kPointMoveColor);
        painter.drawEllipse(m_pointPtrList[i]->area.x() + m_halfRadius/2, m_pointPtrList[i]->area.y() + m_halfRadius/2,
                            m_halfRadius, m_halfRadius);
    }

    // paint current axix position
    if (m_isDeviceConnect == true)
    {
        // coordinates for rect
        m_curAxisPos.area.setRect(m_curAxisPos.posX, m_curAxisPos.posY,
                                  m_kRadius, m_kRadius);
        // paint rect
        painter.setPen(m_curAxisPos.color);
        painter.setBrush(m_curAxisPos.color);
        painter.drawEllipse(m_curAxisPos.area);
    }

    painter.end();
}

void AxesCurvesPlot::updateAxis(int posX, int posY)
{
    m_curAxisPos.posX = calcPointPosX(posX);   // увеличить Х
    m_curAxisPos.posY = calcPointPos(posY);
    update();
}


int AxesCurvesPlot::pointValue(int point_number)
{
    return m_pointPtrList[point_number]->current_value = calcPointValue(m_pointPtrList[point_number]->posY);
}

int AxesCurvesPlot::pointCount() const
{
    return m_pointsCount;
}

void AxesCurvesPlot::setPointValue(int pointNumber, int value)
{
    m_pointPtrList[pointNumber]->posY = calcPointPos(value);
    m_pointPtrList[pointNumber]->current_value = value;
    m_labelPtrList[pointNumber]->setNum(value);
    updateLabelPos();
    update();
}

void AxesCurvesPlot::setLinear()
{
    int tmpY, tmpVal;
    for (int i = 0; i < m_pointPtrList.size(); ++i){
        tmpY = (i * m_rowHeight) + m_kOffset;
        tmpVal = calcPointValue(m_pointPtrList[(m_pointPtrList.size() - 1) - i]->posY);
        m_pointPtrList[(m_pointPtrList.size() -1) -i]->posY = tmpY - m_halfRadius;
        m_pointPtrList[(m_pointPtrList.size() -1) -i]->current_value = tmpVal;
        m_labelPtrList[(m_pointPtrList.size() -1) -i]->setNum(tmpVal);
    }
    updateLabelPos();
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
    }
    update();
}

void AxesCurvesPlot::setLinearInvert()
{
    int tmp_y;
    for (int i = 0; i < m_pointPtrList.size(); ++i){
        tmp_y = (i * m_rowHeight) + m_kOffset;
        m_pointPtrList[i]->posY = tmp_y - m_halfRadius;
        m_pointPtrList[i]->current_value = calcPointValue(m_pointPtrList[i]->posY);
        m_labelPtrList[i]->setNum(m_pointPtrList[i]->current_value);
    }
    updateLabelPos();
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
    }
    update();
}

void AxesCurvesPlot::setExponent()
{
    int tmp_range;
    if ((m_kMinPointValue < 0 && m_kMaxPointValue < 0) || (m_kMinPointValue >= 0 && m_kMaxPointValue >= 0)) {
        tmp_range = m_kMaxPointValue - m_kMinPointValue;
    }
    else {
        tmp_range = abs(m_kMinPointValue) + abs(m_kMaxPointValue);
    }

    int tmp_value;
    for (int i = 0; i < m_pointPtrList.size(); ++i)
    {
        tmp_value = round(exp(i * log(tmp_range) / (m_pointsCount - 1)) + m_kMinPointValue);
        if (tmp_value <= m_kMinPointValue +1){
            tmp_value = m_kMinPointValue;
        } else if (tmp_value >= m_kMaxPointValue -1){
            tmp_value = m_kMaxPointValue;
        }
        setPointValue(i, tmp_value);
    }
    updateLabelPos();
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
    }
}

void AxesCurvesPlot::setExponentInvert()
{
    int tmp_range;
    if ((m_kMinPointValue < 0 && m_kMaxPointValue < 0) || (m_kMinPointValue >= 0 && m_kMaxPointValue >= 0)) {
        tmp_range = m_kMaxPointValue - m_kMinPointValue;
    }
    else {
        tmp_range = abs(m_kMinPointValue) + abs(m_kMaxPointValue);
    }

    int tmp_value;
    for (int i = 0; i < m_pointPtrList.size(); ++i)
    {
        tmp_value = round(exp(i * log(tmp_range) / (m_pointsCount - 1)) + m_kMinPointValue);
        if (tmp_value <= m_kMinPointValue +1){
            tmp_value = m_kMinPointValue;
        } else if (tmp_value >= m_kMaxPointValue -1){
            tmp_value = m_kMaxPointValue;
        }
        setPointValue((m_pointsCount - 1) - i, tmp_value);
    }
    updateLabelPos();
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
    }
}

void AxesCurvesPlot::setShape()
{
    if (m_pointPtrList.size() >= 11)
    {
        setPointValue(0, -100);
        setPointValue(1, -60);
        setPointValue(2, -20);
        setPointValue(3, -6);
        setPointValue(4, -2);
        setPointValue(5, 0);
        setPointValue(6, 2);
        setPointValue(7, 6);
        setPointValue(8, 20);
        setPointValue(9, 60);
        setPointValue(10, 100);
    }
    updateLabelPos();
    for (int i = 0; i < m_pointPtrList.size(); ++i) {
        emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
    }
}

void AxesCurvesPlot::updateLabelPos()
{
    for (int i = 0; i < m_labelPtrList.size(); ++i) {
        m_labelPtrList[i]->move(m_pointPtrList[i]->posX, m_pointPtrList[i]->posY + m_kRadius);
    }
}

int AxesCurvesPlot::calcPointValue(int currentPos) const     // ужоснах
{
    int value = 0;
    float half_height = (m_height) / 2.0 - m_kOffset;
    //current_pos -= - half_radius_;
    currentPos = currentPos - 13;     // костыль
    //qDebug()<<"height"<<height_;
    if (currentPos > half_height){
        float tmp_min = half_height / m_kMinPointValue;
        value = floor((currentPos - half_height) / float(tmp_min));
        //qDebug()<<"value ="<<value;
        return value;
    } else {
        float tmp_max = half_height / m_kMaxPointValue;
        value = -((currentPos - half_height) / float(tmp_max));
        //qDebug()<<"value ="<<value;
        return value;
    }
    //qDebug()<<"value"<<value;
    return value;
}
     // ужоснах
int AxesCurvesPlot::calcPointPos(int value) const     // хз, центр посередине, а дальше скейлится, в зависимости от значений min и max
{                                               // наверно лучше потом сделать центр в зависимости от значений min и max
    int pos = 0;
    float half_height = (m_height) / 2.0 - m_kOffset;    // отдельной переменной для оптимизации?

    if (value < m_kMaxPointValue - m_kMinPointValue){
        float tmp_min = half_height / m_kMinPointValue;      // эта функция запускается до инииализации height_, неопределённое поведение
        pos = round(value * tmp_min + half_height);
    } else {
        float tmp_max = half_height / m_kMaxPointValue;
        pos = round(value * tmp_max + half_height);
    }

    return pos + 12;        // костыль
}

int AxesCurvesPlot::calcPointPosX(int valueX) const     // хз, центр посередине, а дальше скейлится, в зависимости от значений min и max
{                                                   // наверно лучше потом сделать центр в зависимости от значений min и max
    int pos = 0;
    float half_width = (m_width) / 2.0 - m_kOffset;    // отдельной переменной для оптимизации?

    float tmp_max = half_width / 100;// эта функция запускается до инииализации height_, неопределённое поведение
    pos = round(valueX * tmp_max);

    return pos + 12;        // костыль
}


void AxesCurvesPlot::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)

    int tmp_x;//, tmp_y;
    m_width = this->width();
    m_height = this->height();

    m_columnWidth = (m_width - m_kOffset*2) / float(m_kColumnsCount);
    m_rowHeight = (m_height - m_kOffset*2) / float(m_kRowsCount);


    for (int i = 0; i < m_pointPtrList.size(); ++i){
        tmp_x = (i * m_columnWidth) + m_kOffset;
        m_pointPtrList[i]->posX = tmp_x - m_halfRadius;      // temp
        m_pointPtrList[i]->posY = calcPointPos(m_pointPtrList[i]->current_value);
        updateLabelPos();
    }
    for (int i = 0; i < m_pointPtrList.size(); ++i){
        //////tmp_y = (i * row_height_) + offset_;
        //PointAdrList[i]->posY = CalcPointPos(PointAdrList[i]->current_value);//tmp_y - radius_/2;      // temp
    }
//    for (int i = 0; i < rows_count_ + 1; ++i){
//        tmp_y = (i * row_height_) + offset_;
//        PointAdrList[i]->posY = tmp_y - radius_/2;      // temp
//    }

    // size for a2b
    //int x = this->geometry().x();
    int width = m_width-m_kOffset;
    emit sizeChanged(width);
}

void AxesCurvesPlot::mouseMoveEvent(QMouseEvent *event)
{
    // оптимизировать, добавить проверку только по columns line + radius/2 ?
    for (int i = 0; i < m_pointPtrList.size(); ++i)
    {
        if (m_pointPtrList[i]->is_drag == false){
            if (m_pointPtrList[i]->area.contains(event->pos()))   // наверх
            {
                m_pointPtrList[i]->color = m_kPointActiveColor;
                m_pointActive = true;       // ????
                update();
                break;  //?
            } else if (m_pointActive == true){
                m_pointPtrList[i]->color = m_kPointInactiveColor;
                update();
                //break;  //?
            }
        }
        else if (m_pointPtrList[i]->is_drag == true){     // if nah

            if (event->pos().y() < m_kOffset || event->pos().y() > this->height() - m_kOffset) {        // event->pos().y() сделать переменную для оптимизации?
                return;
            }

            if (event->buttons() & Qt::LeftButton){
                m_pointPtrList[i]->posY = event->pos().y() - m_halfRadius;   // half_radius_ - убрать ненужные во всём коде
                m_pointPtrList[i]->color = m_kPointMoveColor;

                m_pointPtrList[i]->current_value = calcPointValue(m_pointPtrList[i]->posY);
                m_labelPtrList[i]->setNum(m_pointPtrList[i]->current_value);
                updateLabelPos();
                update();
                emit pointValueChanged(&i, &m_pointPtrList[i]->current_value);
//                qDebug()<<"event->pos().y()"<<event->pos().y();
//                qDebug()<<"value"<<PointAdrList[i]->current_value;
                //CalcPointValue(event->pos().y());
            }
            break;
        }
    }
    //update();
}

void AxesCurvesPlot::mousePressEvent(QMouseEvent *event)
{
    for (int i = 0; i < m_pointPtrList.size(); ++i){
        if (m_pointPtrList[i]->area.contains(event->pos())) {
            m_pointPtrList[i]->is_drag = true;
            break;
        }
    }
}
void AxesCurvesPlot::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    for (int i = 0; i < m_pointPtrList.size(); ++i){
        if(m_pointPtrList[i]->is_drag == true){
            m_pointPtrList[i]->is_drag = false;
            m_pointPtrList[i]->color = m_kPointInactiveColor;
            break;
        }
    }
    update();
}

void AxesCurvesPlot::deviceStatus(bool isConnect)
{
    m_isDeviceConnect = isConnect;
    update();
}

