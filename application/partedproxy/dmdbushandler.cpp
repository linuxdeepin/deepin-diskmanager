// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "dmdbushandler.h"

#include <QObject>
#include <QDBusError>
#include <QDBusPendingCallWatcher>

DMDbusHandler *DMDbusHandler::m_staticHandeler = nullptr;

DMDbusHandler *DMDbusHandler::instance(QObject *parent)
{
    qDebug() << "DMDbusHandler::instance called.";
    if (parent != nullptr && m_staticHandeler == nullptr) {
        qDebug() << "Creating new DMDbusHandler instance.";
        m_staticHandeler = new DMDbusHandler(parent);
    }
    return m_staticHandeler;
}

DMDbusHandler::~DMDbusHandler()
{
    qDebug() << "DMDbusHandler::~DMDbusHandler called.";
//    Quit();
}

DMDbusHandler::DMDbusHandler(QObject *parent)
    : QObject(parent)
{
    qDebug() << "DMDbusHandler::DMDbusHandler constructor called.";
    qRegisterMetaType<DeviceInfo>("DeviceInfo");
    qRegisterMetaType<DeviceInfoMap>("DeviceInfoMap");
    qDebug() << "Registered DeviceInfo and DeviceInfoMap meta types.";

    qDBusRegisterMetaType<DeviceInfo>();
    qDBusRegisterMetaType<DeviceInfoMap>();
    qDBusRegisterMetaType<PartitionInfo>();
    qDBusRegisterMetaType<PartitionVec>();
    qDBusRegisterMetaType<stCustest>();
    qDBusRegisterMetaType<HardDiskInfo>();
    qDBusRegisterMetaType<HardDiskStatusInfo>();
    qDBusRegisterMetaType<HardDiskStatusInfoList>();
    qDebug() << "Registered various DBus meta types for structs.";

    //注册结构体 lvm dbus通信使用
    qDBusRegisterMetaType<PVRanges>();
    qDBusRegisterMetaType<LVAction>();
    qDBusRegisterMetaType<QVector<PVRanges>>();
    qDBusRegisterMetaType<VGDATA>();
    qDBusRegisterMetaType<PVData>();
    qDBusRegisterMetaType<LVDATA>();
    qDBusRegisterMetaType<LVInfo>();
    qDBusRegisterMetaType<VGInfo>();
    qDBusRegisterMetaType<PVInfo>();
    qDBusRegisterMetaType<LVMInfo>();
    qDBusRegisterMetaType<QVector<QString>>();
    qDBusRegisterMetaType<QList<PVData>>();
    qDBusRegisterMetaType<QList<LVAction>>();
    qDebug() << "Registered LVM related DBus meta types.";

    //注册结构体 luks dbus通信使用
    qDBusRegisterMetaType<LUKS_INFO>();
    qDBusRegisterMetaType<LUKS_MapperInfo>();
    qDBusRegisterMetaType<CRYPT_CIPHER_Support>();
    qDBusRegisterMetaType<LUKSInfoMap>();
    qDBusRegisterMetaType<LUKSMap>();
    qDBusRegisterMetaType<WipeAction>();
    qDebug() << "Registered LUKS related DBus meta types.";

    m_dbus = new DMDBusInterface("com.deepin.diskmanager", "/com/deepin/diskmanager",
                                 QDBusConnection::systemBus(), this);
    //Note: when dealing with remote objects, it is not always possible to determine if it exists when creating a QDBusInterface.
    if (!m_dbus->isValid() && !m_dbus->lastError().message().isEmpty()) {
        qDebug() << "m_dbus isValid false error:" << m_dbus->lastError() << m_dbus->lastError().message();
    }
    qDebug() << "m_dbus isValid true";
    initConnection();

    startService();
}

void DMDbusHandler::initConnection()
{
    qDebug() << "DMDbusHandler::initConnection called.";
    connect(m_dbus, &DMDBusInterface::MessageReport, this, &DMDbusHandler::onMessageReport);
    //  connect(m_dbus, &DMDBusInterface::sigUpdateDeviceInfo, this, &DMDbusHandler::sigUpdateDeviceInfo);
    connect(m_dbus, &DMDBusInterface::updateDeviceInfo, this, &DMDbusHandler::onUpdateDeviceInfo);
    connect(m_dbus, &DMDBusInterface::updateLUKSInfo, this, &DMDbusHandler::onUpdateLUKSInfo);
    connect(m_dbus, &DMDBusInterface::unmountPartition, this, &DMDbusHandler::onUnmountPartition);
    connect(m_dbus, &DMDBusInterface::deletePartition, this, &DMDbusHandler::onDeletePartition);
    connect(m_dbus, &DMDBusInterface::hidePartitionInfo, this, &DMDbusHandler::onHidePartition);
    connect(m_dbus, &DMDBusInterface::showPartitionInfo, this, &DMDbusHandler::onShowPartition);
    connect(m_dbus, &DMDBusInterface::createTableMessage, this, &DMDbusHandler::onCreatePartitionTable);
    connect(m_dbus, &DMDBusInterface::usbUpdated, this, &DMDbusHandler::onUpdateUsb);
    connect(m_dbus, &DMDBusInterface::checkBadBlocksCountInfo, this, &DMDbusHandler::checkBadBlocksCountInfo);
    connect(m_dbus, &DMDBusInterface::fixBadBlocksInfo, this, &DMDbusHandler::repairBadBlocksInfo);
    connect(m_dbus, &DMDBusInterface::checkBadBlocksFinished, this, &DMDbusHandler::checkBadBlocksFinished);
    connect(m_dbus, &DMDBusInterface::fixBadBlocksFinished, this, &DMDbusHandler::fixBadBlocksFinished);
    connect(m_dbus, &DMDBusInterface::clearMessage, this, &DMDbusHandler::wipeMessage);
    connect(m_dbus, &DMDBusInterface::vgCreateMessage, this, &DMDbusHandler::vgCreateMessage);
    connect(m_dbus, &DMDBusInterface::pvDeleteMessage, this, &DMDbusHandler::pvDeleteMessage);
    connect(m_dbus, &DMDBusInterface::vgDeleteMessage, this, &DMDbusHandler::vgDeleteMessage);
    connect(m_dbus, &DMDBusInterface::lvDeleteMessage, this, &DMDbusHandler::lvDeleteMessage);
    connect(m_dbus, &DMDBusInterface::deCryptMessage, this, &DMDbusHandler::deCryptMessage);
    connect(m_dbus, &DMDBusInterface::createFailedMessage, this, &DMDbusHandler::createFailedMessage);
//    connect(m_dbus, &DMDBusInterface::rootLogin, this, &DMDbusHandler::onRootLogin);
    qDebug() << "DMDbusHandler D-Bus signals connected.";
}

void DMDbusHandler::onUnmountPartition(const QString &unmountMessage)
{
    qDebug() << "DMDbusHandler::onUnmountPartition called with message:" << unmountMessage;
    emit unmountPartitionMessage(unmountMessage);
    emit curSelectChanged();
}

void DMDbusHandler::onDeletePartition(const QString &deleteMessage)
{
    qDebug() << "DMDbusHandler::onDeletePartition called with message:" << deleteMessage;
    emit deletePartitionMessage(deleteMessage);
}

void DMDbusHandler::onHidePartition(const QString &hideMessage)
{
    qDebug() << "DMDbusHandler::onHidePartition called with message:" << hideMessage;
    emit hidePartitionMessage(hideMessage);
    emit curSelectChanged();
}

void DMDbusHandler::onShowPartition(const QString &showMessage)
{
    qDebug() << "DMDbusHandler::onShowPartition called with message:" << showMessage;
    emit showPartitionMessage(showMessage);
    emit curSelectChanged();
}

void DMDbusHandler::onCreatePartitionTable(const bool &flag)
{
    qDebug() << "DMDbusHandler::onCreatePartitionTable called with flag:" << flag;
    emit createPartitionTableMessage(flag);
    emit curSelectChanged();
}

void DMDbusHandler::onUpdateUsb()
{
    qDebug() << "DMDbusHandler::onUpdateUsb called.";
    emit updateUsb();
}

void DMDbusHandler::onSetCurSelect(const QString &devicePath, const QString &partitionPath, Sector start, Sector end, int level)
{
    qDebug() << "DMDbusHandler::onSetCurSelect called. DevicePath:" << devicePath << ", PartitionPath:" << partitionPath << ", Level:" << level;
    //点击切换才触发
    if (DeviceType::PARTITION == level || DeviceType::DISK == level) {
        qDebug() << "Level is PARTITION or DISK.";
        if (((level != m_curLevel) || (devicePath != m_curDevicePath) || (partitionPath != m_curPartitionPath)) && m_deviceMap.size() > 0) {
            qDebug() << "Current selection changed for disk/partition.";
            m_curDevicePath = devicePath;
            m_curLevel = level;
            auto it = m_deviceMap.find(devicePath);
            if (it != m_deviceMap.end()) {
                qDebug() << "Device found in map.";
                for (PartitionInfo info : it.value().m_partition) {
                    if (info.m_sectorStart == start && info.m_sectorEnd == end) {
                        qDebug() << info.m_partitionNumber << info.m_path << Utils::fileSystemTypeToString(static_cast<FSType>(info.m_fileSystemType))
                                 << info.m_sectorStart << info.m_sectorEnd << info.m_sectorSize << info.m_devicePath;
                        m_curPartitionInfo = info;
                        break;
                    }
                }
            }
            m_dbus->setCurSelect(m_curPartitionInfo);
            emit curSelectChanged();
        }
    } else if (DeviceType::VOLUMEGROUP == level || DeviceType::LOGICALVOLUME == level) {
        qDebug() << "Level is VOLUMEGROUP or LOGICALVOLUME.";
        QMap<QString, VGInfo> mapVGInfo = m_lvmInfo.m_vgInfo;
        if (((level != m_curLevel) || (devicePath != m_curVGName) || (partitionPath != m_curPartitionPath)) && mapVGInfo.size() > 0) {
            qDebug() << "Current selection changed for LVM.";
            m_curLevel = level;
            m_curVGName = devicePath;
            m_curVGInfo = mapVGInfo.value(devicePath);
            if (DeviceType::LOGICALVOLUME == level) {
                qDebug() << "Level is LOGICALVOLUME.";
                for (int i = 0; i < m_curVGInfo.m_lvlist.count(); i++) {
                    LVInfo info = m_curVGInfo.m_lvlist.at(i);
                    if (partitionPath == info.m_lvPath) {
                        qDebug() << "Matching logical volume found. LV Path:" << info.m_lvPath;
                        m_curLVInfo = info;
                        break;
                    }
                }
            }

            emit curSelectChanged();
        }
    }
}

void DMDbusHandler::startService()
{
    qDebug() << "DMDbusHandler::startService called.";
    m_dbus->Start();
}

void DMDbusHandler::Quit()
{
    qDebug() << "DMDbusHandler::Quit called.";
    m_dbus->Quit();
}

void DMDbusHandler::refresh()
{
    qDebug() << "DMDbusHandler::refresh called.";
    emit showSpinerWindow(true, tr("Refreshing data..."));

    m_dbus->refreshFunc();
}

void DMDbusHandler::onRootLogin(const QString &loginMessage)
{
    qDebug() << "DMDbusHandler::onRootLogin called with message:" << loginMessage;
    m_loginMessage = loginMessage;

    emit rootLogin();
}

QString DMDbusHandler::getRootLoginResult()
{
    qDebug() << "DMDbusHandler::getRootLoginResult called. Returning:" << m_loginMessage;
    return m_loginMessage;
}

void DMDbusHandler::getAllDevice()
{
    qDebug() << "DMDbusHandler::getAllDevice called.";
    QDBusPendingCall pendingCall = m_dbus->getalldevice();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);

    // 连接完成信号
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
        [=](QDBusPendingCallWatcher *watcher) {
            qDebug() << "getAllDevice D-Bus call finished.";
            QDBusPendingReply<> reply = *watcher;
            if (reply.isError()) {
                qDebug() << __FUNCTION__ << reply.error().message();
                QTimer::singleShot(500, this, SLOT(getAllDevice()));
            }
            watcher->deleteLater();
        });
}

void DMDbusHandler::getDeviceInfo()
{
    qDebug() << "DMDbusHandler::getDeviceInfo called.";
    emit showSpinerWindow(true, tr("Initializing data..."));

    getAllDevice();
    qDebug() << __FUNCTION__ << "-------";
}

DeviceInfoMap &DMDbusHandler::probDeviceInfo()
{
    qDebug() << "DMDbusHandler::probDeviceInfo called.";
    QFile file("/etc/mtab");
    QString mounts;
    if (file.open(QIODevice::ReadOnly)) {
        mounts = file.readAll();
        file.close();
        qDebug() << "Successfully read /etc/mtab.";
    } else {
        qDebug() << "Failed to open /etc/mtab.";
    }
    if (!mounts.isEmpty()) {
        qDebug() << "Mounts data is not empty, processing device map.";
        for (auto &disk : m_deviceMap) {
            for (auto &partition : disk.m_partition) {
                if (!mounts.contains(partition.m_path)) {
                    qDebug() << "Partition" << partition.m_path << "not found in /etc/mtab, clearing mount points.";
                    partition.m_mountPoints.clear();
                }
            }
        }
    }
    return m_deviceMap;
}

PartitionVec DMDbusHandler::getCurDevicePartitionArr()
{
    qDebug() << "DMDbusHandler::getCurDevicePartitionArr called.";
    return m_deviceMap.find(m_curDevicePath).value().m_partition;
}

const PartitionInfo &DMDbusHandler::getCurPartititonInfo()
{
    qDebug() << "DMDbusHandler::getCurPartititonInfo called.";
    return m_curPartitionInfo;
}

const DeviceInfo &DMDbusHandler::getCurDeviceInfo()
{
    qDebug() << "DMDbusHandler::getCurDeviceInfo called.";
    return m_deviceMap.find(m_curDevicePath).value();
}

const Sector &DMDbusHandler::getCurDeviceInfoLength()
{
    qDebug() << "DMDbusHandler::getCurDeviceInfoLength called.";
    return m_deviceMap.find(m_curDevicePath).value().m_length;
}

void DMDbusHandler::mount(const QString &mountPath)
{
    qDebug() << "DMDbusHandler::mount called with mountPath:" << mountPath;
    emit showSpinerWindow(true, tr("Mounting %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->mount(mountPath);
}

void DMDbusHandler::unmount()
{
    qDebug() << "DMDbusHandler::unmount called.";
    emit showSpinerWindow(true, tr("Unmounting %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->unmount();
}

QStringList DMDbusHandler::getAllSupportFileSystem()
{
    qDebug() << "DMDbusHandler::getAllSupportFileSystem called.";
    if (m_supportFileSystem.size() <= 0) {
        qDebug() << "m_supportFileSystem is empty, fetching from D-Bus.";
        QDBusPendingReply<QStringList> reply = m_dbus->getallsupportfs();

        reply.waitForFinished();

        if (reply.isError()) {
            qDebug() << reply.error().message();
        } else {
            m_supportFileSystem = reply.value();
            qDebug() << "Successfully fetched supported file systems.";
        }
    }

    return m_supportFileSystem;
}

void DMDbusHandler::format(const QString &fstype, const QString &name)
{
    qDebug() << "DMDbusHandler::format called with fstype:" << fstype << ", name:" << name;
    //   bool success = false;
    //    QDBusPendingReply<bool> reply = m_dbus->format(fstype, name);
    //    reply.waitForFinished();
    //    if (reply.isError()) {
    //        qDebug() << reply.error().message();
    //    } else {
    //        success = reply.value();
    //    }
    emit showSpinerWindow(true);

    m_dbus->format(fstype, name);
}

void DMDbusHandler::resize(const PartitionInfo &info)
{
    qDebug() << "DMDbusHandler::resize called for partition:" << info.m_path;
    emit showSpinerWindow(true, tr("Resizing %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->resize(info);
}

void DMDbusHandler::create(const PartitionVec &infovec)
{
    qDebug() << "DMDbusHandler::create called to create new partition(s).";
    emit showSpinerWindow(true, tr("Creating a new partition..."));

    m_dbus->create(infovec);
}

void DMDbusHandler::onMessageReport(const QString &msg)
{
    qDebug() << "MessageReport:" << msg;
}

void DMDbusHandler::onUpdateDeviceInfo(const DeviceInfoMap &infoMap, const LVMInfo &lvmInfo)
{
    qDebug() << "DMDbusHandler::onUpdateDeviceInfo called. InfoMap size:" << infoMap.size() << ", LVMInfo VG count:" << lvmInfo.m_vgInfo.size();
    m_deviceMap = infoMap;
    m_lvmInfo = lvmInfo;
    m_isExistUnallocated.clear();
    m_deviceNameList.clear();
    m_vgNameList.clear();
    qDebug() << "Device info maps and lists cleared.";

    for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
        DeviceInfo info = it.value();
//        qDebug() << __FUNCTION__ << info.m_path << info.m_length << info.m_heads << info.m_sectors
//                 << info.m_cylinders << info.m_cylsize << info.m_model << info.m_serialNumber << info.m_disktype
//                 << info.m_sectorSize << info.m_maxPrims << info.m_highestBusy << info.m_readonly
//                 << info.m_maxPartitionNameLength << info.m_mediaType << info.m_interface << info.m_vgFlag;

        m_cryptSupport = info.m_crySupport;
        if (info.m_path.isEmpty() || info.m_path.contains("/dev/mapper")) {
            qDebug() << "Skipping empty or /dev/mapper device path:" << info.m_path;
            continue;
        }

        m_deviceNameList << info.m_path;
        QString isExistUnallocated = "false";
        QString isJoinAllVG = "true";
        QString isAllEncryption = "true";
        for (auto it = info.m_partition.begin(); it != info.m_partition.end(); it++) {
//                    qDebug() << it->m_path << it->m_devicePath << it->m_partitionNumber << it->m_sectorsUsed << it->m_sectorsUnused << it->m_sectorStart << it->m_sectorEnd;
            if (it->m_path == "unallocated") {
                qDebug() << "Found unallocated partition.";
                isExistUnallocated = "true";
            }

            if (it->m_vgFlag == LVMFlag::LVM_FLAG_NOT_PV) {
                qDebug() << "Partition is not PV.";
                isJoinAllVG = "false";
            }

            if (it->m_luksFlag == LUKSFlag::NOT_CRYPT_LUKS) {
                qDebug() << "Partition is not LUKS encrypted.";
                isAllEncryption = "false";
            }
        }

        if (("unrecognized" == info.m_disktype || "none" == info.m_disktype) && (info.m_vgFlag != LVMFlag::LVM_FLAG_NOT_PV)) {
            qDebug() << "Disk type unrecognized/none and not PV, setting isJoinAllVG to true.";
            isJoinAllVG = "true";
        }

        // 处理磁盘无分区时，磁盘图标显示错误
        if (info.m_partition.size() == 0 && info.m_vgFlag == LVMFlag::LVM_FLAG_NOT_PV) {
            qDebug() << "Disk has no partitions and not PV, setting isJoinAllVG to false.";
            isJoinAllVG = "false";
        }

        if (info.m_partition.size() == 0 && info.m_luksFlag == LUKSFlag::NOT_CRYPT_LUKS) {
            qDebug() << "Disk has no partitions and not LUKS encrypted, setting isAllEncryption to false.";
            isAllEncryption = "false";
        }

        m_isExistUnallocated[info.m_path] = isExistUnallocated;
        m_isJoinAllVG[info.m_path] = isJoinAllVG;
        m_isAllEncryption[info.m_path] = isAllEncryption;
        qDebug() << "Updated unallocated, join VG, and encryption flags for device:" << info.m_path;
    }

    QMap<QString, VGInfo> lstVGInfo = lvmInfo.m_vgInfo;
    for (auto vgInfo = lstVGInfo.begin(); vgInfo != lstVGInfo.end(); vgInfo++) {
        VGInfo vgInformation = vgInfo.value();
        qDebug() << "Processing volume group:" << vgInformation.m_vgName;

        if (vgInformation.m_vgName.isEmpty()) {
            qDebug() << "Skipping empty VG name.";
            continue;
        }

        m_vgNameList << vgInformation.m_vgName;
        QString isExistUnallocated = "false";
        QString isAllEncryption = "true";
        for (int i = 0; i < vgInformation.m_lvlist.count(); i++) {
            LVInfo info = vgInformation.m_lvlist.at(i);
            if (info.m_lvName.isEmpty() && info.m_lvUuid.isEmpty()) {
                qDebug() << "Found unallocated LV.";
                isExistUnallocated = "true";
            }

            if (info.m_luksFlag == LUKSFlag::NOT_CRYPT_LUKS) {
                qDebug() << "LV is not LUKS encrypted.";
                isAllEncryption = "false";
            }
        }

        m_isExistUnallocated[vgInformation.m_vgName] = isExistUnallocated;
        m_isAllEncryption[vgInformation.m_vgName] = isAllEncryption;
        qDebug() << "Updated unallocated and encryption flags for VG:" << vgInformation.m_vgName;
    }

    emit updateDeviceInfo();
    emit showSpinerWindow(false, "");
    qDebug() << "DMDbusHandler::onUpdateDeviceInfo finished.";
}

void DMDbusHandler::onUpdateLUKSInfo(const LUKSMap &infomap)
{
    qDebug() << "DMDbusHandler::onUpdateLUKSInfo called. LUKSMap size:" << infomap.m_luksMap.size();
    m_curLUKSInfoMap = infomap;
}

QMap<QString, QString> DMDbusHandler::getIsExistUnallocated()
{
    qDebug() << "DMDbusHandler::getIsExistUnallocated called.";
    return m_isExistUnallocated;
}

QStringList DMDbusHandler::getDeviceNameList()
{
    qDebug() << "DMDbusHandler::getDeviceNameList called.";
    return m_deviceNameList;
}

HardDiskInfo DMDbusHandler::getHardDiskInfo(const QString &devicePath)
{
    qDebug() << "DMDbusHandler::getHardDiskInfo called for device:" << devicePath;
    QDBusPendingReply<HardDiskInfo> reply = m_dbus->onGetDeviceHardInfo(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        m_hardDiskInfo = reply.value();
        qDebug() << "Successfully retrieved hard disk info for" << devicePath;
    }

    return m_hardDiskInfo;
}

QString DMDbusHandler::getDeviceHardStatus(const QString &devicePath)
{
    qDebug() << "DMDbusHandler::getDeviceHardStatus called for device:" << devicePath;
    QDBusPendingReply<QString> reply = m_dbus->onGetDeviceHardStatus(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        m_deviceHardStatus = reply.value();
        qDebug() << "Successfully retrieved device hard status for" << devicePath << ":" << m_deviceHardStatus;
    }

    return m_deviceHardStatus;
}

HardDiskStatusInfoList DMDbusHandler::getDeviceHardStatusInfo(const QString &devicePath)
{
    qDebug() << "DMDbusHandler::getDeviceHardStatusInfo called for device:" << devicePath;
    QDBusPendingReply<HardDiskStatusInfoList> reply = m_dbus->onGetDeviceHardStatusInfo(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        m_hardDiskStatusInfoList = reply.value();
        qDebug() << "Successfully retrieved device hard status info for" << devicePath << ". Count:" << m_hardDiskStatusInfoList.count();
    }

    return m_hardDiskStatusInfoList;
}

void DMDbusHandler::deletePartition()
{
    qDebug() << "DMDbusHandler::deletePartition called.";
    emit showSpinerWindow(true, tr("Deleting %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->onDeletePartition();
}

void DMDbusHandler::hidePartition()
{
    qDebug() << "DMDbusHandler::hidePartition called.";
    emit showSpinerWindow(true);

    m_dbus->onHidePartition();
}

void DMDbusHandler::unhidePartition()
{
    qDebug() << "DMDbusHandler::unhidePartition called.";
    emit showSpinerWindow(true);

    m_dbus->onShowPartition();
}

bool DMDbusHandler::detectionPartitionTableError(const QString &devicePath)
{
    qDebug() << "DMDbusHandler::detectionPartitionTableError called for device:" << devicePath;
    QDBusPendingReply<bool> reply = m_dbus->onDetectionPartitionTableError(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        m_partitionTableError = reply.value();
        qDebug() << "Partition table error detection result for" << devicePath << ":" << m_partitionTableError;
    }

    return m_partitionTableError;
}

void DMDbusHandler::checkBadSectors(const QString &devicePath, int blockStart, int blockEnd, int checkNumber, int checkSize, int flag)
{
    qDebug() << "DMDbusHandler::checkBadSectors called for device:" << devicePath << ", checkNumber:" << checkNumber;
    if (checkNumber > 16) {
        qDebug() << "checkNumber > 16, calling onCheckBadBlocksTime.";
        m_dbus->onCheckBadBlocksTime(devicePath, blockStart, blockEnd, QString::number(checkNumber), checkSize, flag);
    } else {
        qDebug() << "checkNumber <= 16, calling onCheckBadBlocksCount.";
        m_dbus->onCheckBadBlocksCount(devicePath, blockStart, blockEnd, checkNumber, checkSize, flag);
    }

}

void DMDbusHandler::repairBadBlocks(const QString &devicePath, QStringList badBlocksList, int repairSize, int flag)
{
    qDebug() << "DMDbusHandler::repairBadBlocks called for device:" << devicePath << ", repairSize:" << repairSize;
    m_dbus->onFixBadBlocks(devicePath, badBlocksList, repairSize, flag);
}

void DMDbusHandler::createPartitionTable(const QString &devicePath, const QString &length, const QString &sectorSize, const QString &diskLabel, const QString &curType)
{
    qDebug() << "DMDbusHandler::createPartitionTable called. DevicePath:" << devicePath << ", Type:" << curType;
    if (curType == "create") {
        emit showSpinerWindow(true, tr("Creating a partition table of %1 ...").arg(devicePath));
        qDebug() << "Creating new partition table.";
    } else {
        emit showSpinerWindow(true, tr("Replacing the partition table of %1 ...").arg(devicePath));
        qDebug() << "Replacing existing partition table.";
    }
    m_curCreateType = curType;

    m_dbus->onCreatePartitionTable(devicePath, length, sectorSize, diskLabel);
}

QString DMDbusHandler::getCurCreatePartitionTableType()
{
    qDebug() << "DMDbusHandler::getCurCreatePartitionTableType called. Returning:" << m_curCreateType;
    return m_curCreateType;
}

int DMDbusHandler::getCurLevel()
{
    qDebug() << "DMDbusHandler::getCurLevel called. Returning:" << m_curLevel;
    return m_curLevel;
}

QString DMDbusHandler::getCurDevicePath()
{
    qDebug() << "DMDbusHandler::getCurDevicePath called. Returning:" << m_curDevicePath;
    return m_curDevicePath;
}

void DMDbusHandler::clear(const WipeAction &wipe)
{
    qDebug() << "DMDbusHandler::clear called";
    m_dbus->clear(wipe);
}

const LVMInfo &DMDbusHandler::probLVMInfo() const
{
    qDebug() << "DMDbusHandler::probLVMInfo called.";
    return m_lvmInfo;
}

const VGInfo &DMDbusHandler::getCurVGInfo()
{
    qDebug() << "DMDbusHandler::getCurVGInfo called.";
    return m_curVGInfo;
}

const LVInfo &DMDbusHandler::getCurLVInfo()
{
    qDebug() << "DMDbusHandler::getCurLVInfo called.";
    return m_curLVInfo;
}

QStringList DMDbusHandler::getVGNameList()
{
    qDebug() << "DMDbusHandler::getVGNameList called.";
    return m_vgNameList;
}

QString DMDbusHandler::getCurVGName()
{
    qDebug() << "DMDbusHandler::getCurVGName called.";
    return m_curVGName;
}

QMap<QString, QString> DMDbusHandler::getIsJoinAllVG()
{
    qDebug() << "DMDbusHandler::getIsJoinAllVG called.";
    return m_isJoinAllVG;
}

bool DMDbusHandler::isExistMountLV(const VGInfo &info)
{
    qDebug() << "DMDbusHandler::isExistMountLV called for VG:" << info.m_vgName;
    for (int i = 0; i < info.m_lvlist.count(); i++) {
        if (lvIsMount(info.m_lvlist.at(i))) {
            qDebug() << "Found mounted LV in VG:" << info.m_vgName;
            return true;
        }
    }

    qDebug() << "No mounted LV found in VG:" << info.m_vgName;
    return false;
}

bool DMDbusHandler::lvIsMount(const LVInfo &lvInfo)
{
    qDebug() << "DMDbusHandler::lvIsMount called";
    if (lvInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
        qDebug() << "LV is LUKS encrypted.";
        LUKS_INFO luksInfo = probLUKSInfo().m_luksMap.value(lvInfo.m_lvPath);
        if (luksInfo.isDecrypt) {
            qDebug() << "LUKS LV is decrypted.";
            if (!luksInfo.m_mapper.m_mountPoints.isEmpty()) {
                qDebug() << "LUKS LV is mounted.";
                return true;
            }
        }
    } else {
        qDebug() << "LV is not LUKS encrypted.";
        if (!lvInfo.m_mountPoints.isEmpty()) {
            qDebug() << "LV is mounted.";
            return true;
        }
    }

    qDebug() << "LV is not mounted.";
    return false;
}

bool DMDbusHandler::isExistMountPartition(const DeviceInfo &info)
{
    qDebug() << "DMDbusHandler::isExistMountPartition called for device:" << info.m_path;
    for (int i = 0; i < info.m_partition.size(); i++) {
        if (partitionISMount(info.m_partition.at(i))) {
            qDebug() << "Found mounted partition on device:" << info.m_path;
            return true;
        }
    }

    qDebug() << "No mounted partition found on device:" << info.m_path;
    return false;
}

bool DMDbusHandler::partitionISMount(const PartitionInfo &info)
{
    qDebug() << "DMDbusHandler::partitionISMount called for partition:" << info.m_path;
    if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
        qDebug() << "Partition is LUKS encrypted.";
        LUKS_INFO luksInfo = probLUKSInfo().m_luksMap.value(info.m_path);
        if (luksInfo.isDecrypt) {
            qDebug() << "LUKS partition is decrypted.";
            if (!luksInfo.m_mapper.m_mountPoints.isEmpty()) {
                qDebug() << "LUKS partition is mounted.";
                return true;
            }
        }
    } else {
        qDebug() << "Partition is not LUKS encrypted.";
        if (!info.m_mountPoints.isEmpty()) {
            qDebug() << "Partition is mounted.";
            return true;
        }
    }

    qDebug() << "Partition is not mounted.";
    return false;
}

void DMDbusHandler::createVG(const QString &vgName, const QList<PVData> &devList, const long long &size)
{
    qDebug() << "DMDbusHandler::createVG called for VG:" << vgName << ", PV count:" << devList.count();
    m_dbus->onCreateVG(vgName, devList, size);
}

void DMDbusHandler::createLV(const QString &vgName, const QList<LVAction> &lvList)
{
    qDebug() << "DMDbusHandler::createLV called for VG:" << vgName << ", LV count:" << lvList.count();
    emit showSpinerWindow(true, tr("Creating..."));

    m_dbus->onCreateLV(vgName, lvList);
}

void DMDbusHandler::deleteLV(const QStringList &lvlist)
{
    qDebug() << "DMDbusHandler::deleteLV called for LVs:" << lvlist;
    emit showSpinerWindow(true, tr("Deleting..."));

    m_dbus->onDeleteLV(lvlist);
}

void DMDbusHandler::deleteVG(const QStringList &vglist)
{
    qDebug() << "DMDbusHandler::deleteVG called for VGs:" << vglist;
    emit showSpinerWindow(true, tr("Deleting..."));

    m_dbus->onDeleteVG(vglist);
}

void DMDbusHandler::resizeVG(const QString &vgName, const QList<PVData> &devList, const long long &size)
{
    qDebug() << "DMDbusHandler::resizeVG called for VG:" << vgName << ", PV count:" << devList.count();
    m_dbus->onResizeVG(vgName, devList, size);
}

void DMDbusHandler::resizeLV(LVAction act)
{
    qDebug() << "DMDbusHandler::resizeLV called";
    emit showSpinerWindow(true, tr("Resizing space..."));

    m_dbus->onResizeLV(act);
}

void DMDbusHandler::onMountLV(LVAction act)
{
    qDebug() << "DMDbusHandler::onMountLV called";
    emit showSpinerWindow(true, tr("Mounting %1 ...").arg(m_curLVInfo.m_lvName));

    m_dbus->onMountLV(act);
}

void DMDbusHandler::onUmountLV(LVAction act)
{
    qDebug() << "DMDbusHandler::onUmountLV called";
    emit showSpinerWindow(true, tr("Unmounting %1 ...").arg(m_curLVInfo.m_lvName));

    m_dbus->onUmountLV(act);
}

void DMDbusHandler::onClearLV(LVAction act)
{
    qDebug() << "DMDbusHandler::onClearLV called";
    m_dbus->onClearLV(act);
}

void DMDbusHandler::onDeletePVList(QList<PVData> devList)
{
    qDebug() << "DMDbusHandler::onDeletePVList called for PVs count:" << devList.count();
    m_dbus->onDeletePVList(devList);
}

QStringList DMDbusHandler::getEncryptionFormate(const QStringList &formateList)
{
    qDebug() << "DMDbusHandler::getEncryptionFormate called.";
    bool isSupportAES = m_cryptSupport.supportEncrypt(m_cryptSupport.aes_xts_plain64);
    bool isSupportSM4 = m_cryptSupport.supportEncrypt(m_cryptSupport.sm4_xts_plain64);
    qDebug() << "AES support:" << isSupportAES << ", SM4 support:" << isSupportSM4;

    QStringList lstFormate;
    for (int i = 0; i < formateList.count(); ++i) {
        lstFormate << formateList.at(i);
        if (isSupportAES) {
            lstFormate << QString("%1 (%2)").arg(formateList.at(i)).arg(tr("AES Encryption"));
            if (isSupportSM4) {
                lstFormate << QString("%1 (%2)").arg(formateList.at(i)).arg(tr("SM4 Encryption"));
            }
        }
    }

    qDebug() << "Generated encryption formats. Count:" << lstFormate.count();
    return lstFormate;
}

const LUKSMap &DMDbusHandler::probLUKSInfo() const
{
    qDebug() << "DMDbusHandler::probLUKSInfo called.";
    return m_curLUKSInfoMap;
}

void DMDbusHandler::updateLUKSInfo(const QString &devPath, const LUKS_INFO &luks)
{
    qDebug() << "DMDbusHandler::updateLUKSInfo called for device:" << devPath;
    m_curLUKSInfoMap.m_luksMap[devPath] = luks;
}

bool DMDbusHandler::getIsSystemDisk(const QString &devName)
{
    qDebug() << "DMDbusHandler::getIsSystemDisk called for device:" << devName << ", current level:" << m_curLevel;
    if (m_curLevel == DeviceType::DISK || m_curLevel == DeviceType::PARTITION) {
        qDebug() << "Current level is DISK or PARTITION.";
        DeviceInfo devInfo = m_deviceMap.value(devName);
        for (int i = 0; i < devInfo.m_partition.size(); ++i) {
            PartitionInfo partitionInfo = devInfo.m_partition.at(i);
            if (partitionInfo.m_flag == 4) {
                qDebug() << "Found system partition by flag:" << partitionInfo.m_path;
                return true;
            }

            for (int j = 0; j < partitionInfo.m_mountPoints.size(); ++j) {
                if (partitionInfo.m_mountPoints[j] == "/boot/efi" || partitionInfo.m_mountPoints[j] == "/"
                        || partitionInfo.m_mountPoints[j] == "/recovery" || partitionInfo.m_mountPoints[j] == "/boot") {
                    qDebug() << "Found system partition by mount point:" << partitionInfo.m_mountPoints[j];
                    return true;
                }
            }
        }
    } else if (m_curLevel == DeviceType::VOLUMEGROUP || m_curLevel == DeviceType::LOGICALVOLUME) {
        qDebug() << "Current level is VOLUMEGROUP or LOGICALVOLUME.";
        VGInfo vgInfo = m_lvmInfo.m_vgInfo.value(devName);
        for (int i = 0; i < vgInfo.m_lvlist.size(); ++i) {
            LVInfo lvInfo = vgInfo.m_lvlist.at(i);
            for (int j = 0; j < lvInfo.m_mountPoints.size(); ++j) {
                if (lvInfo.m_mountPoints[j] == "/boot/efi" || lvInfo.m_mountPoints[j] == "/"
                        || lvInfo.m_mountPoints[j] == "/recovery" || lvInfo.m_mountPoints[j] == "/boot") {
                    qDebug() << "Found system LV by mount point:" << lvInfo.m_mountPoints[j];
                    return true;
                }
            }
        }
    }

    qDebug() << "No system disk/partition/LV found for:" << devName;
    return false;
}

void DMDbusHandler::deCrypt(const LUKS_INFO &luks)
{
    m_dbus->deCrypt(luks);
}

void DMDbusHandler::cryptMount(const LUKS_INFO &luks, const QString &devName)
{
    qDebug() << "DMDbusHandler::cryptMount called for device:" << devName;
    emit showSpinerWindow(true, tr("Mounting %1 ...").arg(devName));

    m_dbus->cryptMount(luks);
}

void DMDbusHandler::cryptUmount(const LUKS_INFO &luks, const QString &devName)
{
    qDebug() << "DMDbusHandler::cryptUmount called for device:" << devName;
    emit showSpinerWindow(true, tr("Unmounting %1 ...").arg(devName));

    m_dbus->cryptUmount(luks);
}

QMap<QString, QString> DMDbusHandler::getIsAllEncryption()
{
    qDebug() << "DMDbusHandler::getIsAllEncryption called.";
    return m_isAllEncryption;
}

void DMDbusHandler::refreshMainWindowData()
{
    qDebug() << "DMDbusHandler::refreshMainWindowData called.";
    emit curSelectChanged();
}

QString DMDbusHandler::getFailedMessage(const QString &key, const int &value, const QString &devPath)
{
    qDebug() << "DMDbusHandler::getFailedMessage called. Key:" << key << ", Value:" << value << ", DevPath:" << devPath;
    QString failedMessage = "";
    if (key == "CRYPTError") {
        switch (value) {
        case CRYPTError::CRYPT_ERR_ENCRYPT_FAILED: {
            failedMessage = tr("Failed to encrypt %1, please try again!").arg(devPath);
            qDebug() << "CRYPT_ERR_ENCRYPT_FAILED for" << devPath;
            break;
        }
        case CRYPTError::CRYPT_ERR_DECRYPT_FAILED: {
            failedMessage = tr("Failed to decrypt %1, please try again!").arg(devPath);
            qDebug() << "CRYPT_ERR_DECRYPT_FAILED for" << devPath;
            break;
        }
        case CRYPTError::CRYPT_ERR_CLOSE_FAILED: {
            failedMessage = tr("%1 failed to close the crypto map").arg(devPath);
            qDebug() << "CRYPT_ERR_CLOSE_FAILED for" << devPath;
            break;
        }
        default:
            qDebug() << "Unknown CRYPTError value:" << value;
            break;
        }
    } else if (key == "DISK_ERROR") {
        switch (value) {
        case DISK_ERROR::DISK_ERR_CREATE_PART_FAILED: {
            failedMessage = tr("Failed to create partitions, please try again!");
            qDebug() << "DISK_ERR_CREATE_PART_FAILED.";
            break;
        }
        case DISK_ERROR::DISK_ERR_CREATE_FS_FAILED: {
            failedMessage = tr("Failed to create %1 file system, please try again!").arg(devPath);
            qDebug() << "DISK_ERR_CREATE_FS_FAILED for" << devPath;
            break;
        }
        case DISK_ERROR::DISK_ERR_UPDATE_KERNEL_FAILED: {
            failedMessage = tr("Failed to submit the request to the kernel");
            qDebug() << "DISK_ERR_UPDATE_KERNEL_FAILED.";
            break;
        }
        case DISK_ERROR::DISK_ERR_DBUS_ARGUMENT: {
            failedMessage = tr("DBUS parameter error");
            qDebug() << "DISK_ERR_DBUS_ARGUMENT.";
            break;
        }
        case DISK_ERROR::DISK_ERR_MOUNT_FAILED: {
            failedMessage = tr("Failed to mount %1").arg(devPath);
            qDebug() << "DISK_ERR_MOUNT_FAILED for" << devPath;
            break;
        }
        case DISK_ERROR::DISK_ERR_CREATE_MOUNTDIR_FAILED: {
            failedMessage = tr("%1 failed to create mounting folders").arg(devPath);
            qDebug() << "DISK_ERR_CREATE_MOUNTDIR_FAILED for" << devPath;
            break;
        }
        case DISK_ERROR::DISK_ERR_CHOWN_FAILED: {
            failedMessage = tr("%1 failed to change the owner of mounting folders").arg(devPath);
            qDebug() << "DISK_ERR_CHOWN_FAILED for" << devPath;
            break;
        }
        case DISK_ERROR::DISK_ERR_CREATE_PARTTAB_FAILED: {
            failedMessage = tr("Creating partition table failed");
            qDebug() << "DISK_ERR_CREATE_PARTTAB_FAILED.";
            break;
        }
        default:
            qDebug() << "Unknown DISK_ERROR value:" << value;
            break;
        }
    } else if (key == "LVMError") {
        switch (value) {
        case LVMError::LVM_ERR_LV_CREATE_FAILED: {
            failedMessage = tr("Failed to create a logical volume, please try again!");
            qDebug() << "LVM_ERR_LV_CREATE_FAILED.";
            break;
        }
        case LVMError::LVM_ERR_LV_CREATE_FS_FAILED: {
            failedMessage = tr("Failed to create %1 file system, please try again!").arg(devPath);
            qDebug() << "LVM_ERR_LV_CREATE_FS_FAILED for" << devPath;
            break;
        }
        case LVMError::LVM_ERR_LV_ARGUMENT: {
            failedMessage = tr("DBUS parameter error");
            qDebug() << "LVM_ERR_LV_ARGUMENT.";
            break;
        }
        default:
            qDebug() << "Unknown LVMError value:" << value;
            break;
        }
    }

    qDebug() << "Returning failed message:" << failedMessage;
    return failedMessage;
}

QString DMDbusHandler::getEncryptionFsType(const LUKS_INFO &luksInfo)
{
    qDebug() << "DMDbusHandler::getEncryptionFsType called";
    if (luksInfo.isDecrypt) {
        qDebug() << "LUKS device is decrypted.";
        if (luksInfo.m_crypt == CRYPT_CIPHER::AES_XTS_PLAIN64) {
            qDebug() << "Encryption type: AES_XTS_PLAIN64.";
            return QString("%1 (%2)").arg(Utils::fileSystemTypeToString(luksInfo.m_mapper.m_luksFs)).arg(tr("AES Encryption"));
        } else if (luksInfo.m_crypt == CRYPT_CIPHER::SM4_XTS_PLAIN64){
            qDebug() << "Encryption type: SM4_XTS_PLAIN64.";
            return QString("%1 (%2)").arg(Utils::fileSystemTypeToString(luksInfo.m_mapper.m_luksFs)).arg(tr("SM4 Encryption"));
        }
    }

    qDebug() << "LUKS device not decrypted or unknown encryption type. Returning default LUKS FS type.";
    return Utils::fileSystemTypeToString(FSType::FS_LUKS);
}

