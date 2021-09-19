#ifndef CONFIGTOFILE_H
#define CONFIGTOFILE_H

#include <QObject>

class QSettings;
#include "common_types.h"

class ConfigToFile : public QObject
{
    Q_OBJECT
public:
//    explicit ConfigToFile(QObject *parent = nullptr);

    static void loadDeviceConfigFromFile(QWidget *parent, const QString &fileName, dev_config_t &devC);
    static void saveDeviceConfigToFile(const QString &fileName, dev_config_t &devC);
//signals:

private:
    static void oldConfigHandler(QWidget *parent, dev_config_t &devC);

};

#endif // CONFIGTOFILE_H
