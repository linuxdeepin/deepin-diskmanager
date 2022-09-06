// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "diskmanagerservice.h"

#include <QCoreApplication>
#include <QDebug>

#include <unistd.h>

namespace DiskManager {

DiskManagerService::DiskManagerService(QObject *parent)
    : QObject(parent)
    , m_partedcore(new PartedCore(this))
{
    initConnection();
}

void DiskManagerService::initConnection()
{
    connect(m_partedcore, &PartedCore::updateDeviceInfo, this, &DiskManagerService::updateDeviceInfo);
    connect(m_partedcore, &PartedCore::updateLUKSInfo, this, &DiskManagerService::updateLUKSInfo);
    connect(m_partedcore, &PartedCore::deletePartitionMessage, this, &DiskManagerService::deletePartition);
    connect(m_partedcore, &PartedCore::hidePartitionInfo, this, &DiskManagerService::hidePartitionInfo);
    connect(m_partedcore, &PartedCore::showPartitionInfo, this, &DiskManagerService::showPartitionInfo);
    connect(m_partedcore, &PartedCore::usbUpdated, this, &DiskManagerService::usbUpdated);
    connect(m_partedcore, &PartedCore::checkBadBlocksCountInfo, this, &DiskManagerService::checkBadBlocksCountInfo);
    connect(m_partedcore, &PartedCore::fixBadBlocksInfo, this, &DiskManagerService::fixBadBlocksInfo);
    connect(m_partedcore, &PartedCore::checkBadBlocksFinished, this, &DiskManagerService::checkBadBlocksFinished);
    connect(m_partedcore, &PartedCore::fixBadBlocksFinished, this, &DiskManagerService::fixBadBlocksFinished);
    connect(m_partedcore, &PartedCore::unmountPartition, this, &DiskManagerService::unmountPartition);
    connect(m_partedcore, &PartedCore::createTableMessage, this, &DiskManagerService::createTableMessage);
    connect(m_partedcore, &PartedCore::vgCreateMessage, this, &DiskManagerService::vgCreateMessage);
    connect(m_partedcore, &PartedCore::pvDeleteMessage, this, &DiskManagerService::pvDeleteMessage);
    connect(m_partedcore, &PartedCore::vgDeleteMessage, this, &DiskManagerService::vgDeleteMessage);
    connect(m_partedcore, &PartedCore::lvDeleteMessage, this, &DiskManagerService::lvDeleteMessage);
    connect(m_partedcore, &PartedCore::clearMessage, this, &DiskManagerService::clearMessage);
    connect(m_partedcore, &PartedCore::deCryptMessage, this, &DiskManagerService::deCryptMessage);
    connect(m_partedcore, &PartedCore::createFailedMessage, this, &DiskManagerService::createFailedMessage);
    connect(this, &DiskManagerService::getAllDeviceInfomation, this, &DiskManagerService::onGetAllDeviceInfomation);
}

void DiskManagerService::Quit()
{
    qDebug() << "DiskManagerService::Quit called";
    m_partedcore->delTempMountFile();
    QCoreApplication::exit(0);
}

//void DiskManagerService::Start(qint64 applicationPid)
//{
//    if (PolicyKitHelper::instance()->checkAuthorization("com.deepin.diskmanager.Start", applicationPid)) {
//       emit rootLogin("1");
//    } else {
//        emit rootLogin("0");
//        Quit();
//    }
//    QString msg = "DiskManagerService::Start called";
//    Q_EMIT MessageReport(msg);
////    qDebug() << msg;
//}
void DiskManagerService::Start()
{
    QString msg = "DiskManagerService::Start called";
    Q_EMIT MessageReport(msg);
}

DeviceInfo DiskManagerService::getDeviceinfo()
{
    QString msg = "DiskManagerService::getDeviceinfo";
    Q_EMIT MessageReport(msg);
//    qDebug() << "DiskManagerService::getDeviceinfo success *******";
    return m_partedcore->getDeviceinfo();
}

//TODO： 这里感觉没有必要发送信号 等之后有时间测试一下 能否去掉多余的代码
void DiskManagerService::getalldevice()
{
//    qDebug() << "DiskManagerService::getalldevice";
//    DeviceInfoMap infores = m_partedcore->getAllDeviceinfo();
//    Q_EMIT updateDeviceInfo(infores);
    emit getAllDeviceInfomation();
}

void DiskManagerService::onGetAllDeviceInfomation()
{
    DeviceInfoMap infores = m_partedcore->getAllDeviceinfo();
    LVMInfo lvmInfo = m_partedcore->getAllLVMinfo();
    LUKSMap luksInfo = m_partedcore->getAllLUKSinfo();
    Q_EMIT updateLUKSInfo(luksInfo);
    Q_EMIT updateDeviceInfo(infores, lvmInfo);
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
    return m_partedcore->mountAndWriteFstab(mountpath);
}

bool DiskManagerService::deCrypt(const LUKS_INFO &luks)
{
    return m_partedcore->deCrypt(luks);
}

bool DiskManagerService::cryptMount(const LUKS_INFO &luks)
{
    return m_partedcore->cryptMount(luks);
}

bool DiskManagerService::cryptUmount(const LUKS_INFO &luks)
{
    return m_partedcore->cryptUmount(luks);
}

QStringList DiskManagerService::getallsupportfs()
{
    return m_partedcore->getallsupportfs();
}

bool DiskManagerService::format(const QString &fstype, const QString &name)
{
    return m_partedcore->format(fstype, name);
}

bool DiskManagerService::clear(const WipeAction &wipe)
{
    return m_partedcore->clear(wipe);
}

bool DiskManagerService::resize(const PartitionInfo &info)
{
    return m_partedcore->resize(info);
}

bool DiskManagerService::create(const PartitionVec &infovec)
{
    return m_partedcore->create(infovec);
}

HardDiskInfo DiskManagerService::onGetDeviceHardInfo(const QString &devicepath)
{
    return m_partedcore->getDeviceHardInfo(devicepath);
}

QString DiskManagerService::onGetDeviceHardStatus(const QString &devicepath)
{
    return m_partedcore->getDeviceHardStatus(devicepath);
}

HardDiskStatusInfoList DiskManagerService::onGetDeviceHardStatusInfo(const QString &devicepath)
{
    return m_partedcore->getDeviceHardStatusInfo(devicepath);
}
bool DiskManagerService::onDeletePartition()
{
    return m_partedcore->deletePartition();
}
bool DiskManagerService::onHidePartition()
{
    return m_partedcore->hidePartition();
}
bool DiskManagerService::onShowPartition()
{
    return m_partedcore->showPartition();
}
bool DiskManagerService::onDetectionPartitionTableError(const QString &devicePath)
{
    return m_partedcore->detectionPartitionTableError(devicePath);
}
bool DiskManagerService::onCreatePartitionTable(const QString &devicePath, const QString &length, const QString &sectorSize, const QString &diskLabel)
{
    return m_partedcore->createPartitionTable(devicePath, length, sectorSize, diskLabel);
}
bool DiskManagerService::onCheckBadBlocksCount(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize, int flag)
{
    return m_partedcore->checkBadBlocks(devicePath, blockStart, blockEnd, checkConut, checkSize, flag);
}
bool DiskManagerService::onCheckBadBlocksTime(const QString &devicePath, int blockStart, int blockEnd, const QString &checkTime, int checkSize, int flag)
{
    return m_partedcore->checkBadBlocks(devicePath, blockStart, blockEnd, checkTime, checkSize, flag);
}
bool DiskManagerService::onFixBadBlocks(const QString &devicePath, QStringList badBlocksList, int checkSize, int flag)
{
    return m_partedcore->fixBadBlocks(devicePath, badBlocksList, checkSize, flag);
}

bool DiskManagerService::onCreateVG(QString vgName, QList<PVData> devList, long long size)
{
    return m_partedcore->createVG(vgName, devList, size);
}

bool DiskManagerService::onCreateLV(QString vgName, QList<LVAction> lvList)
{
    return m_partedcore->createLV(vgName, lvList);
}

bool DiskManagerService::onDeleteVG(QStringList vglist)
{
    return m_partedcore->deleteVG(vglist);
}

bool DiskManagerService::onDeleteLV(QStringList lvlist)
{
    return m_partedcore->deleteLV(lvlist);
}

bool DiskManagerService::onResizeVG(QString vgName, QList<PVData> devList, long long size)
{
    return m_partedcore->resizeVG(vgName, devList, size);
}

bool DiskManagerService::onResizeLV(LVAction lvAction)
{
    return m_partedcore->resizeLV(lvAction);
}

bool DiskManagerService::onMountLV(LVAction lvAction)
{
    return m_partedcore->mountLV(lvAction);
}

bool DiskManagerService::onUmountLV(LVAction lvAction)
{
    return m_partedcore->umountLV(lvAction);
}

bool DiskManagerService::onClearLV(LVAction lvAction)
{
    return m_partedcore->clearLV(lvAction);
}

bool DiskManagerService::onDeletePVList(QList<PVData> devList)
{
    return m_partedcore->deletePVList(devList);
}
void DiskManagerService::updateUsb()
{
    m_partedcore->updateUsb();
}
void DiskManagerService::updateUsbRemove()
{
    m_partedcore->updateUsbRemove();
}
void DiskManagerService::refreshFunc()
{
    m_partedcore->refreshFunc();
}
int DiskManagerService::test()
{
    return m_partedcore->test();
}
} // namespace DiskManager
