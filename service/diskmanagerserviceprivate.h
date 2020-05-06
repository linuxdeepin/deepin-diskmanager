#ifndef DISKMANAGERSERVICEPRIVATE_H
#define DISKMANAGERSERVICEPRIVATE_H

#include <QObject>
#include <QtDBus/QtDBus>
#include "diskoperation/partedcore.h"

class DiskManagerService;
class DiskManagerServicePrivate : public QObject
{
    Q_OBJECT
public:
    explicit DiskManagerServicePrivate(DiskManagerService *parent = nullptr);
    DeviceInfo getDeviceinfo();
    DeviceInfoMap getalldevice();
signals:

public slots:


private:
    DiskManager::PartedCore *m_partedcore;

public:
    DiskManagerService *q_ptr;
    Q_DECLARE_PUBLIC(DiskManagerService)
};

#endif // DISKMANAGERSERVICEPRIVATE_H
