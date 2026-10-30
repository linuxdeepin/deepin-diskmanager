// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "diskmanagerservice.h"
#include "PolicyKitHelper.h"

#include <QCoreApplication>
#include <QDebug>

#include <unistd.h>

namespace DiskManager {

DiskManagerService::DiskManagerService(const QString &frontEndDBusName, QObject *parent)
    : QObject(parent)
    , m_partedcore(new PartedCore(this))
    , m_frontEndDBusName(frontEndDBusName)
{
    qCDebug(serviceLog) << "Initializing DiskManagerService with frontEndDBusName:" << frontEndDBusName;
    initConnection();
}

void DiskManagerService::initConnection()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::initConnection";
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
    qCDebug(serviceLog) << "Entering DiskManagerService::Quit";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for Quit";
        return;
    }

    qCDebug(serviceLog) << "DiskManagerService::Quit called";
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
////    qCDebug(serviceLog) << msg;
//}
void DiskManagerService::Start()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::Start";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for Start";
        return;
    }

    QString msg = "DiskManagerService::Start called";
    Q_EMIT MessageReport(msg);
}

DeviceInfo DiskManagerService::getDeviceinfo()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::getDeviceinfo";
    if (!checkAuthorization()) {
        qCWarning(serviceLog) << "Authorization failed for getDeviceinfo";
        return DeviceInfo();
    }

    QString msg = "DiskManagerService::getDeviceinfo";
    Q_EMIT MessageReport(msg);
//    qCDebug(serviceLog) << "DiskManagerService::getDeviceinfo success *******";
    return m_partedcore->getDeviceinfo();
}

//TODO： 这里感觉没有必要发送信号 等之后有时间测试一下 能否去掉多余的代码
void DiskManagerService::getalldevice()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::getalldevice";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for getalldevice";
        return;
    }

//    qCDebug(serviceLog) << "DiskManagerService::getalldevice";
//    DeviceInfoMap infores = m_partedcore->getAllDeviceinfo();
//    Q_EMIT updateDeviceInfo(infores);
    emit getAllDeviceInfomation();
}

void DiskManagerService::onGetAllDeviceInfomation()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onGetAllDeviceInfomation";
    DeviceInfoMap infores = m_partedcore->getAllDeviceinfo();
    LVMInfo lvmInfo = m_partedcore->getAllLVMinfo();
    LUKSMap luksInfo = m_partedcore->getAllLUKSinfo();
    Q_EMIT updateLUKSInfo(luksInfo);
    Q_EMIT updateDeviceInfo(infores, lvmInfo);
}

void DiskManagerService::setCurSelect(const PartitionInfo &info)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::setCurSelect";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for setCurSelect";
        return;
    }

    m_partedcore->setCurSelect(info);
}

bool DiskManagerService::unmount()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::unmount";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for unmount";
        return false;
    }

    return m_partedcore->unmount();
}

bool DiskManagerService::mount(const QString &mountpath)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::mount with mountpath:" << mountpath;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for mount";
        return false;
    }

    QDBusReply<uint> uidReply = connection().interface()->serviceUid(message().service());
    if (!uidReply.isValid()) {
        qCWarning(serviceLog) << "Failed to get invoker UID for mount, refusing operation";
        return false;
    }
    QString invokerUid = QString::number(uidReply.value());
    return m_partedcore->mountAndWriteFstab(mountpath, invokerUid);
}

bool DiskManagerService::deCrypt(const LUKS_INFO &luks)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::deCrypt";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for deCrypt";
        return false;
    }

    return m_partedcore->deCrypt(luks);
}

bool DiskManagerService::cryptMount(const LUKS_INFO &luks)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::cryptMount";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for cryptMount";
        return false;
    }

    return m_partedcore->cryptMount(luks);
}

bool DiskManagerService::cryptUmount(const LUKS_INFO &luks)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::cryptUmount";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for cryptUmount";
        return false;
    }

    return m_partedcore->cryptUmount(luks);
}

QStringList DiskManagerService::getallsupportfs()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::getallsupportfs";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for getallsupportfs";
        return QStringList();
    }

    return m_partedcore->getallsupportfs();
}

bool DiskManagerService::format(const QString &fstype, const QString &name)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::format with fstype:" << fstype << "and name:" << name;
    if (!checkAuthorization()) {
        qCWarning(serviceLog) << "Authorization failed for format operation";
        return false;
    }

    qCDebug(serviceLog) << "Formatting with filesystem:" << fstype << "name:" << name;
    return m_partedcore->format(fstype, name);
}

bool DiskManagerService::clear(const WipeAction &wipe)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::clear";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for clear";
        return false;
    }

    return m_partedcore->clear(wipe);
}

bool DiskManagerService::resize(const PartitionInfo &info)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::resize";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for resize";
        return false;
    }

    return m_partedcore->resize(info);
}

bool DiskManagerService::create(const PartitionVec &infovec)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::create";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for create";
        return false;
    }

    return m_partedcore->create(infovec);
}

HardDiskInfo DiskManagerService::onGetDeviceHardInfo(const QString &devicepath)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onGetDeviceHardInfo for device:" << devicepath;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onGetDeviceHardInfo";
        return HardDiskInfo();
    }

    return m_partedcore->getDeviceHardInfo(devicepath);
}

QString DiskManagerService::onGetDeviceHardStatus(const QString &devicepath)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onGetDeviceHardStatus for device:" << devicepath;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onGetDeviceHardStatus";
        return QString();
    }

    return m_partedcore->getDeviceHardStatus(devicepath);
}

HardDiskStatusInfoList DiskManagerService::onGetDeviceHardStatusInfo(const QString &devicepath)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onGetDeviceHardStatusInfo for device:" << devicepath;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onGetDeviceHardStatusInfo";
        return HardDiskStatusInfoList();
    }

    return m_partedcore->getDeviceHardStatusInfo(devicepath);
}
bool DiskManagerService::onDeletePartition()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onDeletePartition";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onDeletePartition";
        return false;
    }

    return m_partedcore->deletePartition();
}
bool DiskManagerService::onHidePartition()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onHidePartition";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onHidePartition";
        return false;
    }

    return m_partedcore->hidePartition();
}
bool DiskManagerService::onShowPartition()
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onShowPartition";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onShowPartition";
        return false;
    }

    return m_partedcore->showPartition();
}
bool DiskManagerService::onDetectionPartitionTableError(const QString &devicePath)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onDetectionPartitionTableError for device:" << devicePath;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onDetectionPartitionTableError";
        return false;
    }

    return m_partedcore->detectionPartitionTableError(devicePath);
}
bool DiskManagerService::onCreatePartitionTable(const QString &devicePath, const QString &length, const QString &sectorSize, const QString &diskLabel)
{
    if (!checkAuthorization())
        return false;

    return m_partedcore->createPartitionTable(devicePath, length, sectorSize, diskLabel);
}
bool DiskManagerService::onCheckBadBlocksCount(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize, int flag)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onCheckBadBlocksCount for device:" << devicePath;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onCheckBadBlocksCount";
        return false;
    }

    return m_partedcore->checkBadBlocks(devicePath, blockStart, blockEnd, checkConut, checkSize, flag);
}
bool DiskManagerService::onCheckBadBlocksTime(const QString &devicePath, int blockStart, int blockEnd, const QString &checkTime, int checkSize, int flag)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onCheckBadBlocksTime for device:" << devicePath;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onCheckBadBlocksTime";
        return false;
    }

    return m_partedcore->checkBadBlocks(devicePath, blockStart, blockEnd, checkTime, checkSize, flag);
}
bool DiskManagerService::onFixBadBlocks(const QString &devicePath, QStringList badBlocksList, int checkSize, int flag)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onFixBadBlocks for device:" << devicePath;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onFixBadBlocks";
        return false;
    }

    return m_partedcore->fixBadBlocks(devicePath, badBlocksList, checkSize, flag);
}

bool DiskManagerService::onCreateVG(QString vgName, QList<PVData> devList, long long size)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onCreateVG for VG:" << vgName;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onCreateVG";
        return false;
    }

    return m_partedcore->createVG(vgName, devList, size);
}

bool DiskManagerService::onCreateLV(QString vgName, QList<LVAction> lvList)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onCreateLV for VG:" << vgName;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onCreateLV";
        return false;
    }

    return m_partedcore->createLV(vgName, lvList);
}

bool DiskManagerService::onDeleteVG(QStringList vglist)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onDeleteVG";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onDeleteVG";
        return false;
    }

    return m_partedcore->deleteVG(vglist);
}

bool DiskManagerService::onDeleteLV(QStringList lvlist)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onDeleteLV";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onDeleteLV";
        return false;
    }

    return m_partedcore->deleteLV(lvlist);
}

bool DiskManagerService::onResizeVG(QString vgName, QList<PVData> devList, long long size)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onResizeVG for VG:" << vgName;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onResizeVG";
        return false;
    }

    return m_partedcore->resizeVG(vgName, devList, size);
}

bool DiskManagerService::onResizeLV(LVAction lvAction)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onResizeLV for LV:" << lvAction.m_lvName;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onResizeLV";
        return false;
    }

    return m_partedcore->resizeLV(lvAction);
}

bool DiskManagerService::onMountLV(LVAction lvAction)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onMountLV for LV:" << lvAction.m_lvName;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onMountLV";
        return false;
    }

    return m_partedcore->mountLV(lvAction);
}

bool DiskManagerService::onUmountLV(LVAction lvAction)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onUmountLV for LV:" << lvAction.m_lvName;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onUmountLV";
        return false;
    }

    return m_partedcore->umountLV(lvAction);
}

bool DiskManagerService::onClearLV(LVAction lvAction)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onClearLV for LV:" << lvAction.m_lvName;
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onClearLV";
        return false;
    }

    return m_partedcore->clearLV(lvAction);
}

bool DiskManagerService::onDeletePVList(QList<PVData> devList)
{
    qCDebug(serviceLog) << "Entering DiskManagerService::onDeletePVList";
    if (!checkAuthorization()) {
        qCDebug(serviceLog) << "Authorization failed for onDeletePVList";
        return false;
    }

    return m_partedcore->deletePVList(devList);
}
void DiskManagerService::updateUsb()
{
    if (!checkAuthorization())
        return;

    m_partedcore->updateUsb();
}
void DiskManagerService::updateUsbRemove()
{
    if (!checkAuthorization())
        return;

    m_partedcore->updateUsbRemove();
}
void DiskManagerService::refreshFunc()
{
    if (!checkAuthorization())
        return;

    m_partedcore->refreshFunc();
}
int DiskManagerService::test()
{
    if (!checkAuthorization())
        return 0;

    return m_partedcore->test();
}

bool DiskManagerService::checkAuthorization(void)
{
#ifdef NO_DBUS_CALLER_AUTH_CHECK
    qCDebug(serviceLog) << "Authorization check was skipped";
    return true;
#else
    QString actionId("com.deepin.pkexec.deepin-diskmanager");
    QString serviceName = message().service();

    if (serviceName == m_frontEndDBusName) {
        qCDebug(serviceLog) << "Authorization granted for frontend:" << serviceName;
        return true;
    }

    QDBusReply<uint> uidReply = connection().interface()->serviceUid(serviceName);
    if (uidReply.isValid() && uidReply.value() == 0) {
        qCDebug(serviceLog) << "Authorization granted for root user";
        return true;
    }

    if (PolicyKitHelper::instance()->checkAuthorization(actionId, serviceName)) {
        qCDebug(serviceLog) << "Authorization granted via Polkit for service:" << serviceName;
        return true;
    }

    qCWarning(serviceLog) << "Authorization denied for service:" << serviceName;
    sendErrorReply(QDBusError::AccessDenied);
    return false;
#endif
}

} // namespace DiskManager
