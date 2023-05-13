
#ifndef LEDRGB_H
#define LEDRGB_H


#include <QObject>
#include <QListWidgetItem>


class LedRGB : public QObject
{
    Q_OBJECT
public:
    explicit LedRGB(QObject *parent = nullptr);
    ~LedRGB();

    QListWidgetItem *item() const; //?
    bool isSelected() const;

    QColor color() const;
    void setColor(const QColor &color);

    QIcon icon() const;
    void setIcon(const QIcon &icon, const QColor &color);

    QString text() const;
    void setText(const QString &text);

signals:
    //void ledSelected();

private:
    int m_number;
    QColor m_color;
    //QScopedPointer<QListWidgetItem> m_item;
    QListWidgetItem *m_item;

    static QIcon m_baseIcon;
};

#endif // LEDRGB_H
