#ifndef DMDBUSHANDLER_H
#define DMDBUSHANDLER_H

#include "dmdbusinterface.h"
#include <QObject>
#include <QDebug>
#include <QDBusConnection>

class DMDbusHandler : public QObject
{
    Q_OBJECT
public:
    static DMDbusHandler *instance(QObject *parent = nullptr);
    void Quit();
    void getDeviceinfo();

private:
    explicit DMDbusHandler(QObject *parent = nullptr);
    void initConnection();
signals:
    void sigShowSpinerWindow(bool);
    void sigUpdateDeviceInfo(const DeviceInfoMap &infomap);
private slots:
    void MessageReport(const QString &msg);

private:
    DMDBusInterface        *m_dbus = nullptr;
    static DMDbusHandler    *m_statichandeler;
    QString                 m_curdevice;
    DeviceInfoMap           m_devicemap;
};

#endif // DMDBUSHANDLER_H
