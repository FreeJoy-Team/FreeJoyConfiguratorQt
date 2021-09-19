#ifndef INFOLABEL_H
#define INFOLABEL_H

#include <QLabel>
#include <QProxyStyle>

class InfoLabel : public QLabel
{
    Q_OBJECT
public:
    explicit InfoLabel(QWidget *parent = nullptr);
};


//! instant tooltip for InfoLabel
class InfoProxyStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    int styleHint(StyleHint hint, const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override
    {
        if (hint == QStyle::SH_ToolTip_WakeUpDelay && widget && widget->inherits(InfoLabel::staticMetaObject.className()))
        {
            return 0;
        }

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

#endif // INFOLABEL_H
