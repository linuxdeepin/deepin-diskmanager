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
public slots:

    void MessageReport(const QString &msg);

private:
    DMDBusInterface        *m_dbus = nullptr;
    static DMDbusHandler *m_statichandeler;
};

#endif // DMDBUSHANDLER_H
