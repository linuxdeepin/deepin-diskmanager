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
    Q_EMIT q_ptr->MessageReport(QString("DiskManagerServicePrivate::getalldevice"));
    DeviceInfoMap infores = m_partedcore->getAllDeviceinfo();
    Q_EMIT q_ptr->sigUpdateDeviceInfo(infores);
}

}
