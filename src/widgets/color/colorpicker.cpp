#include "colorpicker.h"
#include "ui_ColorPicker.h"

#include <QColorDialog>
#include <QLabel>

ColorPicker::ColorPicker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorPicker),
    m_currentColor(QColor(Qt::white))
{
    ui->setupUi(this);

    ui->widget_valueSlider->setOrientation(Qt::Vertical);
    ui->widget_valueSlider->setMaximum(255);
    ui->widget_valueSlider->setMinimum(0);
    ui->widget_valueSlider->setValue(255);
    //ui->widget_valueSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->widget_valueSlider->setMinimumWidth(25);

    ui->widget_colorWheel->setMinimumSize(150, 150);

    updateCurColor(m_currentColor);

    connect(ui->widget_colorWheel, &ColorWheel::currentColorChanged, this, [&](const QColor &color) {
        ui->widget_valueSlider->setFirstColor(color);
    });

    connect(ui->widget_valueSlider, &ColorValueSlider::valueChanged, this, [&](int value) {
        m_currentColor.setHsv(m_currentColor.hue(), m_currentColor.saturation(), value);
        updateCurColor(m_currentColor);

        emit currentColorChanged(m_currentColor);
    });

    connect(ui->widget_valueSlider, &ColorValueSlider::currentColorChanged, this, [&](const QColor &color) {
        m_currentColor = color;
        updateCurColor(m_currentColor);

        emit currentColorChanged(m_currentColor);
    });

    connect(ui->widget_colorCell, &ColorCells::currentColorChanged, this, [&](const QColor &color) {
        m_currentColor = color;
        updateCurColor(m_currentColor);

        ui->widget_valueSlider->setCurrentColor(color);
        ui->widget_colorWheel->setCurrentColor(color);

        emit currentColorChanged(m_currentColor);
    });
}

ColorPicker::~ColorPicker()
{
    delete ui;
}


void ColorPicker::updateCurColor(const QColor &color)
{
    QString sh("QLabel { background-color: rgb(%1, %2, %3); }");
    sh = sh.arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue()));
    ui->label_color->setStyleSheet(sh);

    ui->spinBox_red->setValue(color.red());
    ui->spinBox_green->setValue(color.green());
    ui->spinBox_blue->setValue(color.blue());
    ui->spinBox_value->setValue(color.value());
}

QColor ColorPicker::currentColor() const
{
    return m_currentColor;
}

void ColorPicker::setCurrentColor(const QColor &color)
{
    m_currentColor = color;
    //ui->widget_valueSlider->setCurrentColor(color);
    ui->widget_valueSlider->setCurrentColor(color);
    ui->widget_colorWheel->setCurrentColor(color);
}


void ColorPicker::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        if (isEnabled()) {
            QString sh("QLabel { background-color: rgb(%1, %2, %3); }");
            sh = sh.arg(QString::number(m_currentColor.red()), QString::number(m_currentColor.green()), QString::number(m_currentColor.blue()));
            ui->label_color->setStyleSheet(sh);
        } else {
            QString sh("QLabel { background-color: rgb(%1, %2, %3); }");
            QColor col(Qt::gray);   // disabled color
            sh = sh.arg(QString::number(col.red()), QString::number(col.green()), QString::number(col.blue()));
            ui->label_color->setStyleSheet(sh);
        }
    }
}
