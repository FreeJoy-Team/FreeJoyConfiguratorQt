#include "ledrgb.h"
#include <QPainter>
#include <QIcon>

namespace {
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
QIcon LedRGB::m_baseIcon = QIcon("://Images/buttonLed.png");

LedRGB::LedRGB(QObject *parent)
    : QObject{parent}
    , m_item(new QListWidgetItem)
{
    m_item->setIcon(m_baseIcon);
}

LedRGB::~LedRGB()
{
    //delete m_item;
}


bool LedRGB::isSelected() const
{
    return m_item->isSelected();
}

QListWidgetItem *LedRGB::item() const
{
    return m_item;
}

QColor LedRGB::color() const
{
    return m_color;
}

void LedRGB::setColor(const QColor &color)
{
    m_color = color;
    m_item->setIcon(pixmapToIcon(QPixmap("://Images/buttonLed.png"), color));
}

QIcon LedRGB::icon() const
{
    return m_baseIcon;
}

void LedRGB::setIcon(const QIcon &icon, const QColor &color)
{
    m_color = color;
    m_item->setIcon(icon);
}

QString LedRGB::text() const
{
    return m_item->text();
}

void LedRGB::setText(const QString &text)
{
    m_item->setText(text);
}

