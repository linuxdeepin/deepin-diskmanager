#include "diskmanagerservice.h"
#include "diskmanagerserviceprivate.h"
#include <QCoreApplication>
#include <QDebug>
#include <unistd.h>

namespace DiskManager {

DiskManagerService::DiskManagerService(QObject *parent) :
    QObject(parent), d_ptr(new DiskManagerServicePrivate(this))
{


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
    Q_D(DiskManagerService);
    QString msg = "DiskManagerService::getDeviceinfo";
    Q_EMIT MessageReport(msg);
    qDebug() << "DiskManagerService::getDeviceinfo success *******";
    return d->getDeviceinfo();
}

void DiskManagerService::getalldevice()
{
    Q_D(DiskManagerService);
    qDebug() << "DiskManagerService::getalldevice";
    d->getalldevice();
}

stCustest DiskManagerService::interfacetest()
{
    stCustest stcus;
    stcus.heads = 1000;
    stcus.length = 2000;
    stcus.m_path = "uos";
    return stcus;
}

}
