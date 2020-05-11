#include "diskmanagerserviceprivate.h"
#include "diskmanagerservice.h"
#include <QCoreApplication>
#include <DLog>
#include <QDebug>
#include <unistd.h>
#include <QThread>
namespace DiskManager {

DiskManagerServicePrivate::DiskManagerServicePrivate(DiskManagerService *parent) : QObject(parent)
    , q_ptr(parent), m_partedcore(new DiskManager::PartedCore(this))
{
//    qRegisterMetaType<DeviceInfo>("DeviceInfo");
//    qDBusRegisterMetaType<DeviceInfo>();
//    qDBusRegisterMetaType<DeviceInfoMap>();
//    qDBusRegisterMetaType<stCustest>();
}

DeviceInfo DiskManagerServicePrivate::getDeviceinfo()
{
    Q_EMIT q_ptr->MessageReport(QString("m_partedcore->getDeviceinfo start,log path:%1").arg(Dtk::Core::DLogManager::getlogFilePath()));
    qDebug() << "DiskManagerServicePrivate::getDeviceinfo start";
    DeviceInfo info = m_partedcore->getDeviceinfo();
    qDebug() << "DiskManagerServicePrivate::getDeviceinfo end";
    return info;
}

void DiskManagerServicePrivate::getalldevice()
{
    DeviceInfoMap map;
    for (int i = 0; i < 2; ++i) {
        DeviceInfo info;
        info.m_path = QString("test%1").arg(i);
        info.heads = i + 1;
        info.max_prims = i + 1;
        info.cylinders = (i + 1) * 100;
        for (int b = 0; b < 2; b++) {
            stCustest sttest;
            sttest.heads = i;
            sttest.length = i + 1;
            sttest.m_path = QString("sbkebcmj%1").arg(b);
            info.partition.push_back(sttest);
        }
        map.insert(QString("uos%1").arg(i), info);
    }
    sleep(5);
    Q_EMIT q_ptr->sigUpdateDeviceInfo(map);
}

}
