#ifndef DMDBUSHANDLER_H
#define DMDBUSHANDLER_H

#include "dmdbusinterface.h"
#include <QObject>
#include <QDebug>
#include <QDBusConnection>
#include "../widgets/customcontrol/dmtreeviewdelegate.h"

class DMDbusHandler : public QObject
{
    Q_OBJECT
public:
    static DMDbusHandler *instance(QObject *parent = nullptr);
    void Quit();
    void getDeviceinfo();
    DeviceInfoMap probDeviceInfo()const;

private:
    explicit DMDbusHandler(QObject *parent = nullptr);
    void initConnection();
signals:
    void sigShowSpinerWindow(bool);
    void sigUpdateDeviceInfo();
    void sigCurSelectChanged(DiskInfoData infodata);
private slots:
    void MessageReport(const QString &msg);
    void slotUpdateDeviceInfo(const DeviceInfoMap &infomap);

private:
    DMDBusInterface        *m_dbus = nullptr;
    static DMDbusHandler    *m_statichandeler;
    QString                 m_curdevice;
    DeviceInfoMap           m_devicemap;
};

#endif // DMDBUSHANDLER_H
