#include "diskmanagerservice.h"
#include <QCoreApplication>
#include <QDebug>
#include <unistd.h>

namespace DiskManager {

DiskManagerService::DiskManagerService(QObject *parent) :
    QObject(parent), m_partedcore(new PartedCore(this))
{
    initConnection();

}

void DiskManagerService::Quit()
{
    qDebug() << "DiskManagerService::Quit called";
    QCoreApplication::exit(0);
}

void DiskManagerService::Start()
{
    QString msg = "DiskManagerService::Start called";
    Q_EMIT MessageReport(msg);
    qDebug() << msg;
}

DeviceInfo DiskManagerService::getDeviceinfo()
{
    QString msg = "DiskManagerService::getDeviceinfo";
    Q_EMIT MessageReport(msg);
    qDebug() << "DiskManagerService::getDeviceinfo success *******";
    return m_partedcore->getDeviceinfo();
}

void DiskManagerService::getalldevice()
{
    qDebug() << "DiskManagerService::getalldevice";
    DeviceInfoMap infores = m_partedcore->getAllDeviceinfo();
    Q_EMIT sigUpdateDeviceInfo(infores);

}

void DiskManagerService::setCurSelect(const PartitionInfo &info)
{
    m_partedcore->setCurSelect(info);
}

bool DiskManagerService::unmount()
{
    return m_partedcore->unmount();
}

bool DiskManagerService::mount(const QString &mountpath)
{
    return m_partedcore->mount(mountpath);
}

QStringList DiskManagerService::getallsupportfs()
{
    return m_partedcore->getallsupportfs();
}

bool DiskManagerService::format(const QString &fstype, const QString &name)
{
    return m_partedcore->format(fstype, name);
}

bool DiskManagerService::resize(const PartitionInfo &info)
{
    return m_partedcore->resize(info);
}

bool DiskManagerService::create(const PartitionVec &infovec)
{

}

stCustest DiskManagerService::interfacetest()
{
    stCustest stcus;
    stcus.heads = 1000;
    stcus.length = 2000;
    stcus.m_path = "uos";
    return stcus;
}

void DiskManagerService::initConnection()
{
    connect(m_partedcore, &PartedCore::sigUpdateDeviceInfo, this, &DiskManagerService::sigUpdateDeviceInfo);
}

}
