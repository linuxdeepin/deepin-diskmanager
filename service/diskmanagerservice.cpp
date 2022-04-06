/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file diskmanagerservice.cpp
 *
 * @brief 磁盘分区操作封装类
 *
 * @date 2020-09-04 11:10
 *
 * Author: liweigang  <liweigang@uniontech.com>
 *
 * Maintainer: liweigang  <liweigang@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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

QStringList DiskManagerService::getallsupportfs()
{
    return m_partedcore->getallsupportfs();
}

bool DiskManagerService::format(const QString &fstype, const QString &name)
{
    return m_partedcore->format(fstype, name);
}

bool DiskManagerService::clear(const QString &fstype, const QString &path, const QString &name, const QString &user, const int &diskType, const int &clearType)
{
    return m_partedcore->clear(fstype, path, name, user, diskType, clearType);
}

bool DiskManagerService::resize(const PartitionInfo &info)
{
    return m_partedcore->resize(info);
}

bool DiskManagerService::create(const PartitionVec &infovec)
{
    return m_partedcore->create(infovec);
}

void DiskManagerService::initConnection()
{
    connect(m_partedcore, &PartedCore::updateDeviceInfo, this, &DiskManagerService::updateDeviceInfo);
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
    connect(this, &DiskManagerService::getAllDeviceInfomation, this, &DiskManagerService::onGetAllDeviceInfomation);
    connect(m_partedcore, &PartedCore::clearMessage, this, &DiskManagerService::clearMessage);
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
