#include "ledrgbconfig.h"
#include "ui_ledrgbconfig.h"
#include "common_types.h"
#include "deviceconfig.h"
#include "global.h"

#include <QPainter>
#include <QIcon>
namespace {     // много копий этой функции, сделать отдельно хидер
    //! QPixmap gray-scale image (an alpha map) to colored QIcon
    QIcon pixmapToIcon(QPixmap pixmap, const QColor &color)
    {
        // initialize painter to draw on a pixmap and set composition mode
        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);//CompositionMode_SourceIn
        // set color
        painter.setBrush(color);
        painter.setPen(color);
        // paint rect
        painter.drawRect(pixmap.rect());
        // here is our new colored icon
        return QIcon(pixmap);
    }
}

LedRGBConfig::LedRGBConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LedRGBConfig)
{
    ui->setupUi(this);

    ui->horizontalLayout_Colors->addWidget(&m_colorPicker);
    ui->spinBox_ledsCount->setMaximum(NUM_RGB_LEDS);

    connect(ui->listWidget_leds, &QListWidget::itemClicked, this, &LedRGBConfig::itemClicked);
    connect(&m_colorPicker, &ColorPicker::currentColorChanged, this, &LedRGBConfig::setColorToSelectedItems);
}

LedRGBConfig::~LedRGBConfig()
{
    delete ui;
}

void LedRGBConfig::retranslateUi()
{
    ui->retranslateUi(this);
}


void LedRGBConfig::itemClicked(QListWidgetItem *item)
{
    for (int i = 0; i < m_rgbPtrList.size(); ++i) {
        if (m_rgbPtrList[i]->item() == item) {
            m_colorPicker.setCurrentColor(m_rgbPtrList[i]->color());
        }
    }
}

void LedRGBConfig::setColorToSelectedItems(const QColor &color)
{
    QIcon icon(pixmapToIcon(QPixmap("://Images/buttonLed.png"), color));
    for (int i = 0; i < m_rgbPtrList.size(); ++i) {
        if (m_rgbPtrList[i]->isSelected()) {
            m_rgbPtrList[i]->setIcon(icon, color);
        }
    }
}


void LedRGBConfig::on_radioButton_staticColor_clicked(bool checked)
{
    if (checked) {
        m_colorPicker.setEnabled(true);
        ui->listWidget_leds->setEnabled(true);
        ui->spinBox_delay->setEnabled(false);
        ui->label_delay->setEnabled(false);
        ui->spinBox_RBBrightness->setEnabled(false);
        ui->label_RBBrightness->setEnabled(false);
    }
}

void LedRGBConfig::on_radioButton_rainbow_clicked(bool checked)
{
    if (checked) {
        m_colorPicker.setEnabled(false);
        ui->listWidget_leds->setEnabled(false);
        ui->spinBox_delay->setEnabled(true);
        ui->label_delay->setEnabled(true);
        ui->spinBox_RBBrightness->setEnabled(true);
        ui->label_RBBrightness->setEnabled(true);
    }
}

void LedRGBConfig::on_radioButton_simHub_clicked(bool checked)
{
    if (checked) {
        m_colorPicker.setEnabled(false);
        ui->listWidget_leds->setEnabled(false);
        ui->spinBox_delay->setEnabled(false);
        ui->label_delay->setEnabled(false);
        ui->spinBox_RBBrightness->setEnabled(false);
        ui->label_RBBrightness->setEnabled(false);
    }
}

void LedRGBConfig::on_radioButton_flow_clicked(bool checked)
{
    if (checked) {
        m_colorPicker.setEnabled(true);
        ui->listWidget_leds->setEnabled(true);
        ui->spinBox_delay->setEnabled(true);
        ui->label_delay->setEnabled(true);
        ui->spinBox_RBBrightness->setEnabled(false);
        ui->label_RBBrightness->setEnabled(false);
    }
}

void LedRGBConfig::on_spinBox_ledsCount_valueChanged(int value)
{
    int curCount = m_rgbPtrList.size();
    if (value == curCount) return;

    dev_config_t *devc = &gEnv.pDeviceConfig->config;

    if (value > curCount) {
        for (int i = curCount; i < value; ++i) {
            LedRGB *led = new LedRGB(this);
            led->setText(QString::number(i+1));
            int red = devc->rgb_leds[i].r;
            int green = devc->rgb_leds[i].g;
            int blue = devc->rgb_leds[i].b;
            led->setColor(QColor(red, green, blue));
            m_rgbPtrList.append(led);
            ui->listWidget_leds->addItem(led->item());
        }
    } else {
        for (int i = curCount; i > value; --i) {
            delete ui->listWidget_leds->takeItem(ui->listWidget_leds->row(m_rgbPtrList.last()->item()));
            m_rgbPtrList.last()->deleteLater();
            m_rgbPtrList.removeLast();
        }
    }
}


void LedRGBConfig::readFromConfig()
{
    dev_config_t *devc = &gEnv.pDeviceConfig->config;

    on_spinBox_ledsCount_valueChanged(devc->rgb_count); // change
    // ИЗМЕНИТЬ ЭТУ ХУЙНЮ !!!!
    for (int i = 0; i < devc->rgb_count; ++i) {
        int red = devc->rgb_leds[i].r;
        int green = devc->rgb_leds[i].g;
        int blue = devc->rgb_leds[i].b;
        m_rgbPtrList[i]->setColor(QColor(red, green, blue));
    }

    switch(devc->rgb_effect) {
        case WS2812B_STATIC:
            ui->radioButton_staticColor->setChecked(true);
            on_radioButton_staticColor_clicked(true);
            break;
        case WS2812B_RAINBOW:
            ui->radioButton_rainbow->setChecked(true);
            on_radioButton_rainbow_clicked(true);
            break;
        case WS2812B_SIMHUB:
            ui->radioButton_simHub->setChecked(true);
            on_radioButton_simHub_clicked(true);
            break;
        case WS2812B_FLOW:
            ui->radioButton_flow->setChecked(true);
            on_radioButton_flow_clicked(true);
            break;
        default:
            break;
    }

    ui->spinBox_ledsCount->setValue(devc->rgb_count);
    ui->spinBox_delay->setValue(devc->rgb_delay_ms);
    ui->spinBox_RBBrightness->setValue(devc->rgb_brightness);
}

void LedRGBConfig::writeToConfig()
{
    dev_config_t *devc = &gEnv.pDeviceConfig->config;
    for (int i = 0; i < m_rgbPtrList.size(); ++i) {
        QColor color = m_rgbPtrList[i]->color();
        devc->rgb_leds[i].r = color.red();
        devc->rgb_leds[i].g = color.green();
        devc->rgb_leds[i].b = color.blue();
    }

    if (ui->radioButton_staticColor->isChecked()) {
        devc->rgb_effect = WS2812B_STATIC;
    } else if (ui->radioButton_rainbow->isChecked()) {
        devc->rgb_effect = WS2812B_RAINBOW;
    } else if (ui->radioButton_simHub->isChecked()) {
        devc->rgb_effect = WS2812B_SIMHUB;
    } else if (ui->radioButton_flow->isChecked()) {
        devc->rgb_effect = WS2812B_FLOW;
    }

    devc->rgb_count = ui->spinBox_ledsCount->value();
    devc->rgb_delay_ms = ui->spinBox_delay->value();
    devc->rgb_brightness = ui->spinBox_RBBrightness->value();
}
