// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "dmdbushandler.h"

#include <QObject>
#include <QDBusError>
#include <QDBusPendingCallWatcher>

DMDbusHandler *DMDbusHandler::m_staticHandeler = nullptr;

DMDbusHandler *DMDbusHandler::instance(QObject *parent)
{
    qCDebug(appLog) << "DMDbusHandler::instance called.";
    if (parent != nullptr && m_staticHandeler == nullptr) {
        qCDebug(appLog) << "Creating new DMDbusHandler instance.";
        m_staticHandeler = new DMDbusHandler(parent);
    }
    return m_staticHandeler;
}

DMDbusHandler::~DMDbusHandler()
{
    qCDebug(appLog) << "DMDbusHandler::~DMDbusHandler called.";
//    Quit();
}

DMDbusHandler::DMDbusHandler(QObject *parent)
    : QObject(parent)
{
    qCDebug(appLog) << "DMDbusHandler::DMDbusHandler constructor called.";
    qRegisterMetaType<DeviceInfo>("DeviceInfo");
    qRegisterMetaType<DeviceInfoMap>("DeviceInfoMap");
    qCDebug(appLog) << "Registered DeviceInfo and DeviceInfoMap meta types.";

    qDBusRegisterMetaType<DeviceInfo>();
    qDBusRegisterMetaType<DeviceInfoMap>();
    qDBusRegisterMetaType<PartitionInfo>();
    qDBusRegisterMetaType<PartitionVec>();
    qDBusRegisterMetaType<stCustest>();
    qDBusRegisterMetaType<HardDiskInfo>();
    qDBusRegisterMetaType<HardDiskStatusInfo>();
    qDBusRegisterMetaType<HardDiskStatusInfoList>();
    qCDebug(appLog) << "Registered various DBus meta types for structs.";

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
    qCDebug(appLog) << "Registered LVM related DBus meta types.";

    //注册结构体 luks dbus通信使用
    qDBusRegisterMetaType<LUKS_INFO>();
    qDBusRegisterMetaType<LUKS_MapperInfo>();
    qDBusRegisterMetaType<CRYPT_CIPHER_Support>();
    qDBusRegisterMetaType<LUKSInfoMap>();
    qDBusRegisterMetaType<LUKSMap>();
    qDBusRegisterMetaType<WipeAction>();
    qCDebug(appLog) << "Registered LUKS related DBus meta types.";

    m_dbus = new DMDBusInterface("com.deepin.diskmanager", "/com/deepin/diskmanager",
                                 QDBusConnection::systemBus(), this);
    //Note: when dealing with remote objects, it is not always possible to determine if it exists when creating a QDBusInterface.
    if (!m_dbus->isValid() && !m_dbus->lastError().message().isEmpty()) {
        qCDebug(appLog) << "m_dbus isValid false error:" << m_dbus->lastError() << m_dbus->lastError().message();
    }
    qCDebug(appLog) << "m_dbus isValid true";
    initConnection();

    startService();
}

void DMDbusHandler::initConnection()
{
    qCDebug(appLog) << "DMDbusHandler::initConnection called.";
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
    qCDebug(appLog) << "DMDbusHandler D-Bus signals connected.";
}

void DMDbusHandler::onUnmountPartition(const QString &unmountMessage)
{
    qCDebug(appLog) << "DMDbusHandler::onUnmountPartition called with message:" << unmountMessage;
    emit unmountPartitionMessage(unmountMessage);
    emit curSelectChanged();
}

void DMDbusHandler::onDeletePartition(const QString &deleteMessage)
{
    qCDebug(appLog) << "DMDbusHandler::onDeletePartition called with message:" << deleteMessage;
    emit deletePartitionMessage(deleteMessage);
}

void DMDbusHandler::onHidePartition(const QString &hideMessage)
{
    qCDebug(appLog) << "DMDbusHandler::onHidePartition called with message:" << hideMessage;
    emit hidePartitionMessage(hideMessage);
    emit curSelectChanged();
}

void DMDbusHandler::onShowPartition(const QString &showMessage)
{
    qCDebug(appLog) << "DMDbusHandler::onShowPartition called with message:" << showMessage;
    emit showPartitionMessage(showMessage);
    emit curSelectChanged();
}

void DMDbusHandler::onCreatePartitionTable(const bool &flag)
{
    qCDebug(appLog) << "DMDbusHandler::onCreatePartitionTable called with flag:" << flag;
    emit createPartitionTableMessage(flag);
    emit curSelectChanged();
}

void DMDbusHandler::onUpdateUsb()
{
    qCDebug(appLog) << "DMDbusHandler::onUpdateUsb called.";
    emit updateUsb();
}

void DMDbusHandler::onSetCurSelect(const QString &devicePath, const QString &partitionPath, Sector start, Sector end, int level)
{
    qCDebug(appLog) << "DMDbusHandler::onSetCurSelect called. DevicePath:" << devicePath << ", PartitionPath:" << partitionPath << ", Level:" << level;
    //点击切换才触发
    if (DeviceType::PARTITION == level || DeviceType::DISK == level) {
        qCDebug(appLog) << "Level is PARTITION or DISK.";
        if (((level != m_curLevel) || (devicePath != m_curDevicePath) || (partitionPath != m_curPartitionPath)) && m_deviceMap.size() > 0) {
            qCDebug(appLog) << "Current selection changed for disk/partition.";
            m_curDevicePath = devicePath;
            m_curLevel = level;
            auto it = m_deviceMap.find(devicePath);
            if (it != m_deviceMap.end()) {
                qCDebug(appLog) << "Device found in map.";
                for (PartitionInfo info : it.value().m_partition) {
                    if (info.m_sectorStart == start && info.m_sectorEnd == end) {
                        qCDebug(appLog) << info.m_partitionNumber << info.m_path << Utils::fileSystemTypeToString(static_cast<FSType>(info.m_fileSystemType))
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
        qCDebug(appLog) << "Level is VOLUMEGROUP or LOGICALVOLUME.";
        QMap<QString, VGInfo> mapVGInfo = m_lvmInfo.m_vgInfo;
        if (((level != m_curLevel) || (devicePath != m_curVGName) || (partitionPath != m_curPartitionPath)) && mapVGInfo.size() > 0) {
            qCDebug(appLog) << "Current selection changed for LVM.";
            m_curLevel = level;
            m_curVGName = devicePath;
            m_curVGInfo = mapVGInfo.value(devicePath);
            if (DeviceType::LOGICALVOLUME == level) {
                qCDebug(appLog) << "Level is LOGICALVOLUME.";
                for (int i = 0; i < m_curVGInfo.m_lvlist.count(); i++) {
                    LVInfo info = m_curVGInfo.m_lvlist.at(i);
                    if (partitionPath == info.m_lvPath) {
                        qCDebug(appLog) << "Matching logical volume found. LV Path:" << info.m_lvPath;
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
    qCDebug(appLog) << "DMDbusHandler::startService called.";
    m_dbus->Start();
}

void DMDbusHandler::Quit()
{
    qCDebug(appLog) << "DMDbusHandler::Quit called.";
    m_dbus->Quit();
}

void DMDbusHandler::refresh()
{
    qCDebug(appLog) << "DMDbusHandler::refresh called.";
    emit showSpinerWindow(true, tr("Refreshing data..."));

    m_dbus->refreshFunc();
}

void DMDbusHandler::onRootLogin(const QString &loginMessage)
{
    qCDebug(appLog) << "DMDbusHandler::onRootLogin called with message:" << loginMessage;
    m_loginMessage = loginMessage;

    emit rootLogin();
}

QString DMDbusHandler::getRootLoginResult()
{
    qCDebug(appLog) << "DMDbusHandler::getRootLoginResult called. Returning:" << m_loginMessage;
    return m_loginMessage;
}

void DMDbusHandler::getAllDevice()
{
    qCDebug(appLog) << "DMDbusHandler::getAllDevice called.";
    QDBusPendingCall pendingCall = m_dbus->getalldevice();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);

    // 连接完成信号
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished,
        [=](QDBusPendingCallWatcher *watcher) {
            qCDebug(appLog) << "getAllDevice D-Bus call finished.";
            QDBusPendingReply<> reply = *watcher;
            if (reply.isError()) {
                qCDebug(appLog) << __FUNCTION__ << reply.error().message();
                QTimer::singleShot(500, this, SLOT(getAllDevice()));
            }
            watcher->deleteLater();
        });
}

void DMDbusHandler::getDeviceInfo()
{
    qCDebug(appLog) << "DMDbusHandler::getDeviceInfo called.";
    emit showSpinerWindow(true, tr("Initializing data..."));

    getAllDevice();
    qCDebug(appLog) << __FUNCTION__ << "-------";
}

DeviceInfoMap &DMDbusHandler::probDeviceInfo()
{
    qCDebug(appLog) << "DMDbusHandler::probDeviceInfo called.";
    QFile file("/etc/mtab");
    QString mounts;
    if (file.open(QIODevice::ReadOnly)) {
        mounts = file.readAll();
        file.close();
        qCDebug(appLog) << "Successfully read /etc/mtab.";
    } else {
        qCDebug(appLog) << "Failed to open /etc/mtab.";
    }
    if (!mounts.isEmpty()) {
        qCDebug(appLog) << "Mounts data is not empty, processing device map.";
        for (auto &disk : m_deviceMap) {
            for (auto &partition : disk.m_partition) {
                if (!mounts.contains(partition.m_path)) {
                    qCDebug(appLog) << "Partition" << partition.m_path << "not found in /etc/mtab, clearing mount points.";
                    partition.m_mountPoints.clear();
                }
            }
        }
    }
    return m_deviceMap;
}

PartitionVec DMDbusHandler::getCurDevicePartitionArr()
{
    qCDebug(appLog) << "DMDbusHandler::getCurDevicePartitionArr called.";
    return m_deviceMap.find(m_curDevicePath).value().m_partition;
}

const PartitionInfo &DMDbusHandler::getCurPartititonInfo()
{
    qCDebug(appLog) << "DMDbusHandler::getCurPartititonInfo called.";
    return m_curPartitionInfo;
}

const DeviceInfo &DMDbusHandler::getCurDeviceInfo()
{
    qCDebug(appLog) << "DMDbusHandler::getCurDeviceInfo called.";
    return m_deviceMap.find(m_curDevicePath).value();
}

const Sector &DMDbusHandler::getCurDeviceInfoLength()
{
    qCDebug(appLog) << "DMDbusHandler::getCurDeviceInfoLength called.";
    return m_deviceMap.find(m_curDevicePath).value().m_length;
}

void DMDbusHandler::mount(const QString &mountPath)
{
    qCDebug(appLog) << "DMDbusHandler::mount called with mountPath:" << mountPath;
    emit showSpinerWindow(true, tr("Mounting %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->mount(mountPath);
}

void DMDbusHandler::unmount()
{
    qCDebug(appLog) << "DMDbusHandler::unmount called.";
    emit showSpinerWindow(true, tr("Unmounting %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->unmount();
}

QStringList DMDbusHandler::getAllSupportFileSystem()
{
    qCDebug(appLog) << "DMDbusHandler::getAllSupportFileSystem called.";
    if (m_supportFileSystem.size() <= 0) {
        qCDebug(appLog) << "m_supportFileSystem is empty, fetching from D-Bus.";
        QDBusPendingReply<QStringList> reply = m_dbus->getallsupportfs();

        reply.waitForFinished();

        if (reply.isError()) {
            qCDebug(appLog) << reply.error().message();
        } else {
            m_supportFileSystem = reply.value();
            qCDebug(appLog) << "Successfully fetched supported file systems.";
        }
    }

    return m_supportFileSystem;
}

void DMDbusHandler::format(const QString &fstype, const QString &name)
{
    qCDebug(appLog) << "DMDbusHandler::format called with fstype:" << fstype << ", name:" << name;
    //   bool success = false;
    //    QDBusPendingReply<bool> reply = m_dbus->format(fstype, name);
    //    reply.waitForFinished();
    //    if (reply.isError()) {
    //        qCDebug(appLog) << reply.error().message();
    //    } else {
    //        success = reply.value();
    //    }
    emit showSpinerWindow(true);

    m_dbus->format(fstype, name);
}

void DMDbusHandler::resize(const PartitionInfo &info)
{
    qCDebug(appLog) << "DMDbusHandler::resize called for partition:" << info.m_path;
    emit showSpinerWindow(true, tr("Resizing %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->resize(info);
}

void DMDbusHandler::create(const PartitionVec &infovec)
{
    qCDebug(appLog) << "DMDbusHandler::create called to create new partition(s).";
    emit showSpinerWindow(true, tr("Creating a new partition..."));

    m_dbus->create(infovec);
}

void DMDbusHandler::onMessageReport(const QString &msg)
{
    qCDebug(appLog) << "MessageReport:" << msg;
}

void DMDbusHandler::onUpdateDeviceInfo(const DeviceInfoMap &infoMap, const LVMInfo &lvmInfo)
{
    qCDebug(appLog) << "DMDbusHandler::onUpdateDeviceInfo called. InfoMap size:" << infoMap.size() << ", LVMInfo VG count:" << lvmInfo.m_vgInfo.size();
    m_deviceMap = infoMap;
    m_lvmInfo = lvmInfo;
    m_isExistUnallocated.clear();
    m_deviceNameList.clear();
    m_vgNameList.clear();
    qCDebug(appLog) << "Device info maps and lists cleared.";

    for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
        DeviceInfo info = it.value();
//        qCDebug(appLog) << __FUNCTION__ << info.m_path << info.m_length << info.m_heads << info.m_sectors
//                 << info.m_cylinders << info.m_cylsize << info.m_model << info.m_serialNumber << info.m_disktype
//                 << info.m_sectorSize << info.m_maxPrims << info.m_highestBusy << info.m_readonly
//                 << info.m_maxPartitionNameLength << info.m_mediaType << info.m_interface << info.m_vgFlag;

        m_cryptSupport = info.m_crySupport;
        if (info.m_path.isEmpty() || info.m_path.contains("/dev/mapper")) {
            qCDebug(appLog) << "Skipping empty or /dev/mapper device path:" << info.m_path;
            continue;
        }

        m_deviceNameList << info.m_path;
        QString isExistUnallocated = "false";
        QString isJoinAllVG = "true";
        QString isAllEncryption = "true";
        for (auto it = info.m_partition.begin(); it != info.m_partition.end(); it++) {
//                    qCDebug(appLog) << it->m_path << it->m_devicePath << it->m_partitionNumber << it->m_sectorsUsed << it->m_sectorsUnused << it->m_sectorStart << it->m_sectorEnd;
            if (it->m_path == "unallocated") {
                qCDebug(appLog) << "Found unallocated partition.";
                isExistUnallocated = "true";
            }

            if (it->m_vgFlag == LVMFlag::LVM_FLAG_NOT_PV) {
                qCDebug(appLog) << "Partition is not PV.";
                isJoinAllVG = "false";
            }

            if (it->m_luksFlag == LUKSFlag::NOT_CRYPT_LUKS) {
                qCDebug(appLog) << "Partition is not LUKS encrypted.";
                isAllEncryption = "false";
            }
        }

        if (("unrecognized" == info.m_disktype || "none" == info.m_disktype) && (info.m_vgFlag != LVMFlag::LVM_FLAG_NOT_PV)) {
            qCDebug(appLog) << "Disk type unrecognized/none and not PV, setting isJoinAllVG to true.";
            isJoinAllVG = "true";
        }

        // 处理磁盘无分区时，磁盘图标显示错误
        if (info.m_partition.size() == 0 && info.m_vgFlag == LVMFlag::LVM_FLAG_NOT_PV) {
            qCDebug(appLog) << "Disk has no partitions and not PV, setting isJoinAllVG to false.";
            isJoinAllVG = "false";
        }

        if (info.m_partition.size() == 0 && info.m_luksFlag == LUKSFlag::NOT_CRYPT_LUKS) {
            qCDebug(appLog) << "Disk has no partitions and not LUKS encrypted, setting isAllEncryption to false.";
            isAllEncryption = "false";
        }

        m_isExistUnallocated[info.m_path] = isExistUnallocated;
        m_isJoinAllVG[info.m_path] = isJoinAllVG;
        m_isAllEncryption[info.m_path] = isAllEncryption;
        qCDebug(appLog) << "Updated unallocated, join VG, and encryption flags for device:" << info.m_path;
    }

    QMap<QString, VGInfo> lstVGInfo = lvmInfo.m_vgInfo;
    for (auto vgInfo = lstVGInfo.begin(); vgInfo != lstVGInfo.end(); vgInfo++) {
        VGInfo vgInformation = vgInfo.value();
        qCDebug(appLog) << "Processing volume group:" << vgInformation.m_vgName;

        if (vgInformation.m_vgName.isEmpty()) {
            qCDebug(appLog) << "Skipping empty VG name.";
            continue;
        }

        m_vgNameList << vgInformation.m_vgName;
        QString isExistUnallocated = "false";
        QString isAllEncryption = "true";
        for (int i = 0; i < vgInformation.m_lvlist.count(); i++) {
            LVInfo info = vgInformation.m_lvlist.at(i);
            if (info.m_lvName.isEmpty() && info.m_lvUuid.isEmpty()) {
                qCDebug(appLog) << "Found unallocated LV.";
                isExistUnallocated = "true";
            }

            if (info.m_luksFlag == LUKSFlag::NOT_CRYPT_LUKS) {
                qCDebug(appLog) << "LV is not LUKS encrypted.";
                isAllEncryption = "false";
            }
        }

        m_isExistUnallocated[vgInformation.m_vgName] = isExistUnallocated;
        m_isAllEncryption[vgInformation.m_vgName] = isAllEncryption;
        qCDebug(appLog) << "Updated unallocated and encryption flags for VG:" << vgInformation.m_vgName;
    }

    emit updateDeviceInfo();
    emit showSpinerWindow(false, "");
    qCDebug(appLog) << "DMDbusHandler::onUpdateDeviceInfo finished.";
}

void DMDbusHandler::onUpdateLUKSInfo(const LUKSMap &infomap)
{
    qCDebug(appLog) << "DMDbusHandler::onUpdateLUKSInfo called. LUKSMap size:" << infomap.m_luksMap.size();
    m_curLUKSInfoMap = infomap;
}

QMap<QString, QString> DMDbusHandler::getIsExistUnallocated()
{
    qCDebug(appLog) << "DMDbusHandler::getIsExistUnallocated called.";
    return m_isExistUnallocated;
}

QStringList DMDbusHandler::getDeviceNameList()
{
    qCDebug(appLog) << "DMDbusHandler::getDeviceNameList called.";
    return m_deviceNameList;
}

HardDiskInfo DMDbusHandler::getHardDiskInfo(const QString &devicePath)
{
    qCDebug(appLog) << "DMDbusHandler::getHardDiskInfo called for device:" << devicePath;
    QDBusPendingReply<HardDiskInfo> reply = m_dbus->onGetDeviceHardInfo(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qCDebug(appLog) << reply.error().message();
    } else {
        m_hardDiskInfo = reply.value();
        qCDebug(appLog) << "Successfully retrieved hard disk info for" << devicePath;
    }

    return m_hardDiskInfo;
}

QString DMDbusHandler::getDeviceHardStatus(const QString &devicePath)
{
    qCDebug(appLog) << "DMDbusHandler::getDeviceHardStatus called for device:" << devicePath;
    QDBusPendingReply<QString> reply = m_dbus->onGetDeviceHardStatus(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qCDebug(appLog) << reply.error().message();
    } else {
        m_deviceHardStatus = reply.value();
        qCDebug(appLog) << "Successfully retrieved device hard status for" << devicePath << ":" << m_deviceHardStatus;
    }

    return m_deviceHardStatus;
}

HardDiskStatusInfoList DMDbusHandler::getDeviceHardStatusInfo(const QString &devicePath)
{
    qCDebug(appLog) << "DMDbusHandler::getDeviceHardStatusInfo called for device:" << devicePath;
    QDBusPendingReply<HardDiskStatusInfoList> reply = m_dbus->onGetDeviceHardStatusInfo(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qCDebug(appLog) << reply.error().message();
    } else {
        m_hardDiskStatusInfoList = reply.value();
        qCDebug(appLog) << "Successfully retrieved device hard status info for" << devicePath << ". Count:" << m_hardDiskStatusInfoList.count();
    }

    return m_hardDiskStatusInfoList;
}

void DMDbusHandler::deletePartition()
{
    qCDebug(appLog) << "DMDbusHandler::deletePartition called.";
    emit showSpinerWindow(true, tr("Deleting %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->onDeletePartition();
}

void DMDbusHandler::hidePartition()
{
    qCDebug(appLog) << "DMDbusHandler::hidePartition called.";
    emit showSpinerWindow(true);

    m_dbus->onHidePartition();
}

void DMDbusHandler::unhidePartition()
{
    qCDebug(appLog) << "DMDbusHandler::unhidePartition called.";
    emit showSpinerWindow(true);

    m_dbus->onShowPartition();
}

bool DMDbusHandler::detectionPartitionTableError(const QString &devicePath)
{
    qCDebug(appLog) << "DMDbusHandler::detectionPartitionTableError called for device:" << devicePath;
    QDBusPendingReply<bool> reply = m_dbus->onDetectionPartitionTableError(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qCDebug(appLog) << reply.error().message();
    } else {
        m_partitionTableError = reply.value();
        qCDebug(appLog) << "Partition table error detection result for" << devicePath << ":" << m_partitionTableError;
    }

    return m_partitionTableError;
}

void DMDbusHandler::checkBadSectors(const QString &devicePath, int blockStart, int blockEnd, int checkNumber, int checkSize, int flag)
{
    qCDebug(appLog) << "DMDbusHandler::checkBadSectors called for device:" << devicePath << ", checkNumber:" << checkNumber;
    if (checkNumber > 16) {
        qCDebug(appLog) << "checkNumber > 16, calling onCheckBadBlocksTime.";
        m_dbus->onCheckBadBlocksTime(devicePath, blockStart, blockEnd, QString::number(checkNumber), checkSize, flag);
    } else {
        qCDebug(appLog) << "checkNumber <= 16, calling onCheckBadBlocksCount.";
        m_dbus->onCheckBadBlocksCount(devicePath, blockStart, blockEnd, checkNumber, checkSize, flag);
    }

}

void DMDbusHandler::repairBadBlocks(const QString &devicePath, QStringList badBlocksList, int repairSize, int flag)
{
    qCDebug(appLog) << "DMDbusHandler::repairBadBlocks called for device:" << devicePath << ", repairSize:" << repairSize;
    m_dbus->onFixBadBlocks(devicePath, badBlocksList, repairSize, flag);
}

void DMDbusHandler::createPartitionTable(const QString &devicePath, const QString &length, const QString &sectorSize, const QString &diskLabel, const QString &curType)
{
    qCDebug(appLog) << "DMDbusHandler::createPartitionTable called. DevicePath:" << devicePath << ", Type:" << curType;
    if (curType == "create") {
        emit showSpinerWindow(true, tr("Creating a partition table of %1 ...").arg(devicePath));
        qCDebug(appLog) << "Creating new partition table.";
    } else {
        emit showSpinerWindow(true, tr("Replacing the partition table of %1 ...").arg(devicePath));
        qCDebug(appLog) << "Replacing existing partition table.";
    }
    m_curCreateType = curType;

    m_dbus->onCreatePartitionTable(devicePath, length, sectorSize, diskLabel);
}

QString DMDbusHandler::getCurCreatePartitionTableType()
{
    qCDebug(appLog) << "DMDbusHandler::getCurCreatePartitionTableType called. Returning:" << m_curCreateType;
    return m_curCreateType;
}

int DMDbusHandler::getCurLevel()
{
    qCDebug(appLog) << "DMDbusHandler::getCurLevel called. Returning:" << m_curLevel;
    return m_curLevel;
}

QString DMDbusHandler::getCurDevicePath()
{
    qCDebug(appLog) << "DMDbusHandler::getCurDevicePath called. Returning:" << m_curDevicePath;
    return m_curDevicePath;
}

void DMDbusHandler::clear(const WipeAction &wipe)
{
    qCDebug(appLog) << "DMDbusHandler::clear called";
    m_dbus->clear(wipe);
}

const LVMInfo &DMDbusHandler::probLVMInfo() const
{
    qCDebug(appLog) << "DMDbusHandler::probLVMInfo called.";
    return m_lvmInfo;
}

const VGInfo &DMDbusHandler::getCurVGInfo()
{
    qCDebug(appLog) << "DMDbusHandler::getCurVGInfo called.";
    return m_curVGInfo;
}

const LVInfo &DMDbusHandler::getCurLVInfo()
{
    qCDebug(appLog) << "DMDbusHandler::getCurLVInfo called.";
    return m_curLVInfo;
}

QStringList DMDbusHandler::getVGNameList()
{
    qCDebug(appLog) << "DMDbusHandler::getVGNameList called.";
    return m_vgNameList;
}

QString DMDbusHandler::getCurVGName()
{
    qCDebug(appLog) << "DMDbusHandler::getCurVGName called.";
    return m_curVGName;
}

QMap<QString, QString> DMDbusHandler::getIsJoinAllVG()
{
    qCDebug(appLog) << "DMDbusHandler::getIsJoinAllVG called.";
    return m_isJoinAllVG;
}

bool DMDbusHandler::isExistMountLV(const VGInfo &info)
{
    qCDebug(appLog) << "DMDbusHandler::isExistMountLV called for VG:" << info.m_vgName;
    for (int i = 0; i < info.m_lvlist.count(); i++) {
        if (lvIsMount(info.m_lvlist.at(i))) {
            qCDebug(appLog) << "Found mounted LV in VG:" << info.m_vgName;
            return true;
        }
    }

    qCDebug(appLog) << "No mounted LV found in VG:" << info.m_vgName;
    return false;
}

bool DMDbusHandler::lvIsMount(const LVInfo &lvInfo)
{
    qCDebug(appLog) << "DMDbusHandler::lvIsMount called";
    if (lvInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
        qCDebug(appLog) << "LV is LUKS encrypted.";
        LUKS_INFO luksInfo = probLUKSInfo().m_luksMap.value(lvInfo.m_lvPath);
        if (luksInfo.isDecrypt) {
            qCDebug(appLog) << "LUKS LV is decrypted.";
            if (!luksInfo.m_mapper.m_mountPoints.isEmpty()) {
                qCDebug(appLog) << "LUKS LV is mounted.";
                return true;
            }
        }
    } else {
        qCDebug(appLog) << "LV is not LUKS encrypted.";
        if (!lvInfo.m_mountPoints.isEmpty()) {
            qCDebug(appLog) << "LV is mounted.";
            return true;
        }
    }

    qCDebug(appLog) << "LV is not mounted.";
    return false;
}

bool DMDbusHandler::isExistMountPartition(const DeviceInfo &info)
{
    qCDebug(appLog) << "DMDbusHandler::isExistMountPartition called for device:" << info.m_path;
    for (int i = 0; i < info.m_partition.size(); i++) {
        if (partitionISMount(info.m_partition.at(i))) {
            qCDebug(appLog) << "Found mounted partition on device:" << info.m_path;
            return true;
        }
    }

    qCDebug(appLog) << "No mounted partition found on device:" << info.m_path;
    return false;
}

bool DMDbusHandler::partitionISMount(const PartitionInfo &info)
{
    qCDebug(appLog) << "DMDbusHandler::partitionISMount called for partition:" << info.m_path;
    if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
        qCDebug(appLog) << "Partition is LUKS encrypted.";
        LUKS_INFO luksInfo = probLUKSInfo().m_luksMap.value(info.m_path);
        if (luksInfo.isDecrypt) {
            qCDebug(appLog) << "LUKS partition is decrypted.";
            if (!luksInfo.m_mapper.m_mountPoints.isEmpty()) {
                qCDebug(appLog) << "LUKS partition is mounted.";
                return true;
            }
        }
    } else {
        qCDebug(appLog) << "Partition is not LUKS encrypted.";
        if (!info.m_mountPoints.isEmpty()) {
            qCDebug(appLog) << "Partition is mounted.";
            return true;
        }
    }

    qCDebug(appLog) << "Partition is not mounted.";
    return false;
}

void DMDbusHandler::createVG(const QString &vgName, const QList<PVData> &devList, const long long &size)
{
    qCDebug(appLog) << "DMDbusHandler::createVG called for VG:" << vgName << ", PV count:" << devList.count();
    m_dbus->onCreateVG(vgName, devList, size);
}

void DMDbusHandler::createLV(const QString &vgName, const QList<LVAction> &lvList)
{
    qCDebug(appLog) << "DMDbusHandler::createLV called for VG:" << vgName << ", LV count:" << lvList.count();
    emit showSpinerWindow(true, tr("Creating..."));

    m_dbus->onCreateLV(vgName, lvList);
}

void DMDbusHandler::deleteLV(const QStringList &lvlist)
{
    qCDebug(appLog) << "DMDbusHandler::deleteLV called for LVs:" << lvlist;
    emit showSpinerWindow(true, tr("Deleting..."));

    m_dbus->onDeleteLV(lvlist);
}

void DMDbusHandler::deleteVG(const QStringList &vglist)
{
    qCDebug(appLog) << "DMDbusHandler::deleteVG called for VGs:" << vglist;
    emit showSpinerWindow(true, tr("Deleting..."));

    m_dbus->onDeleteVG(vglist);
}

void DMDbusHandler::resizeVG(const QString &vgName, const QList<PVData> &devList, const long long &size)
{
    qCDebug(appLog) << "DMDbusHandler::resizeVG called for VG:" << vgName << ", PV count:" << devList.count();
    m_dbus->onResizeVG(vgName, devList, size);
}

void DMDbusHandler::resizeLV(LVAction act)
{
    qCDebug(appLog) << "DMDbusHandler::resizeLV called";
    emit showSpinerWindow(true, tr("Resizing space..."));

    m_dbus->onResizeLV(act);
}

void DMDbusHandler::onMountLV(LVAction act)
{
    qCDebug(appLog) << "DMDbusHandler::onMountLV called";
    emit showSpinerWindow(true, tr("Mounting %1 ...").arg(m_curLVInfo.m_lvName));

    m_dbus->onMountLV(act);
}

void DMDbusHandler::onUmountLV(LVAction act)
{
    qCDebug(appLog) << "DMDbusHandler::onUmountLV called";
    emit showSpinerWindow(true, tr("Unmounting %1 ...").arg(m_curLVInfo.m_lvName));

    m_dbus->onUmountLV(act);
}

void DMDbusHandler::onClearLV(LVAction act)
{
    qCDebug(appLog) << "DMDbusHandler::onClearLV called";
    m_dbus->onClearLV(act);
}

void DMDbusHandler::onDeletePVList(QList<PVData> devList)
{
    qCDebug(appLog) << "DMDbusHandler::onDeletePVList called for PVs count:" << devList.count();
    m_dbus->onDeletePVList(devList);
}

QStringList DMDbusHandler::getEncryptionFormate(const QStringList &formateList)
{
    qCDebug(appLog) << "DMDbusHandler::getEncryptionFormate called.";
    bool isSupportAES = m_cryptSupport.supportEncrypt(m_cryptSupport.aes_xts_plain64);
    bool isSupportSM4 = m_cryptSupport.supportEncrypt(m_cryptSupport.sm4_xts_plain64);
    qCDebug(appLog) << "AES support:" << isSupportAES << ", SM4 support:" << isSupportSM4;

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

    qCDebug(appLog) << "Generated encryption formats. Count:" << lstFormate.count();
    return lstFormate;
}

const LUKSMap &DMDbusHandler::probLUKSInfo() const
{
    qCDebug(appLog) << "DMDbusHandler::probLUKSInfo called.";
    return m_curLUKSInfoMap;
}

void DMDbusHandler::updateLUKSInfo(const QString &devPath, const LUKS_INFO &luks)
{
    qCDebug(appLog) << "DMDbusHandler::updateLUKSInfo called for device:" << devPath;
    m_curLUKSInfoMap.m_luksMap[devPath] = luks;
}

bool DMDbusHandler::getIsSystemDisk(const QString &devName)
{
    qCDebug(appLog) << "DMDbusHandler::getIsSystemDisk called for device:" << devName << ", current level:" << m_curLevel;
    if (m_curLevel == DeviceType::DISK || m_curLevel == DeviceType::PARTITION) {
        qCDebug(appLog) << "Current level is DISK or PARTITION.";
        DeviceInfo devInfo = m_deviceMap.value(devName);
        for (int i = 0; i < devInfo.m_partition.size(); ++i) {
            PartitionInfo partitionInfo = devInfo.m_partition.at(i);
            if (partitionInfo.m_flag == 4) {
                qCDebug(appLog) << "Found system partition by flag:" << partitionInfo.m_path;
                return true;
            }

            for (int j = 0; j < partitionInfo.m_mountPoints.size(); ++j) {
                if (partitionInfo.m_mountPoints[j] == "/boot/efi" || partitionInfo.m_mountPoints[j] == "/"
                        || partitionInfo.m_mountPoints[j] == "/recovery" || partitionInfo.m_mountPoints[j] == "/boot") {
                    qCDebug(appLog) << "Found system partition by mount point:" << partitionInfo.m_mountPoints[j];
                    return true;
                }
            }
        }
    } else if (m_curLevel == DeviceType::VOLUMEGROUP || m_curLevel == DeviceType::LOGICALVOLUME) {
        qCDebug(appLog) << "Current level is VOLUMEGROUP or LOGICALVOLUME.";
        VGInfo vgInfo = m_lvmInfo.m_vgInfo.value(devName);
        for (int i = 0; i < vgInfo.m_lvlist.size(); ++i) {
            LVInfo lvInfo = vgInfo.m_lvlist.at(i);
            for (int j = 0; j < lvInfo.m_mountPoints.size(); ++j) {
                if (lvInfo.m_mountPoints[j] == "/boot/efi" || lvInfo.m_mountPoints[j] == "/"
                        || lvInfo.m_mountPoints[j] == "/recovery" || lvInfo.m_mountPoints[j] == "/boot") {
                    qCDebug(appLog) << "Found system LV by mount point:" << lvInfo.m_mountPoints[j];
                    return true;
                }
            }
        }
    }

    qCDebug(appLog) << "No system disk/partition/LV found for:" << devName;
    return false;
}

void DMDbusHandler::deCrypt(const LUKS_INFO &luks)
{
    m_dbus->deCrypt(luks);
}

void DMDbusHandler::cryptMount(const LUKS_INFO &luks, const QString &devName)
{
    qCDebug(appLog) << "DMDbusHandler::cryptMount called for device:" << devName;
    emit showSpinerWindow(true, tr("Mounting %1 ...").arg(devName));

    m_dbus->cryptMount(luks);
}

void DMDbusHandler::cryptUmount(const LUKS_INFO &luks, const QString &devName)
{
    qCDebug(appLog) << "DMDbusHandler::cryptUmount called for device:" << devName;
    emit showSpinerWindow(true, tr("Unmounting %1 ...").arg(devName));

    m_dbus->cryptUmount(luks);
}

QMap<QString, QString> DMDbusHandler::getIsAllEncryption()
{
    qCDebug(appLog) << "DMDbusHandler::getIsAllEncryption called.";
    return m_isAllEncryption;
}

void DMDbusHandler::refreshMainWindowData()
{
    qCDebug(appLog) << "DMDbusHandler::refreshMainWindowData called.";
    emit curSelectChanged();
}

QString DMDbusHandler::getFailedMessage(const QString &key, const int &value, const QString &devPath)
{
    qCDebug(appLog) << "DMDbusHandler::getFailedMessage called. Key:" << key << ", Value:" << value << ", DevPath:" << devPath;
    QString failedMessage = "";
    if (key == "CRYPTError") {
        switch (value) {
        case CRYPTError::CRYPT_ERR_ENCRYPT_FAILED: {
            failedMessage = tr("Failed to encrypt %1, please try again!").arg(devPath);
            qCDebug(appLog) << "CRYPT_ERR_ENCRYPT_FAILED for" << devPath;
            break;
        }
        case CRYPTError::CRYPT_ERR_DECRYPT_FAILED: {
            failedMessage = tr("Failed to decrypt %1, please try again!").arg(devPath);
            qCDebug(appLog) << "CRYPT_ERR_DECRYPT_FAILED for" << devPath;
            break;
        }
        case CRYPTError::CRYPT_ERR_CLOSE_FAILED: {
            failedMessage = tr("%1 failed to close the crypto map").arg(devPath);
            qCDebug(appLog) << "CRYPT_ERR_CLOSE_FAILED for" << devPath;
            break;
        }
        default:
            qCDebug(appLog) << "Unknown CRYPTError value:" << value;
            break;
        }
    } else if (key == "DISK_ERROR") {
        switch (value) {
        case DISK_ERROR::DISK_ERR_CREATE_PART_FAILED: {
            failedMessage = tr("Failed to create partitions, please try again!");
            qCDebug(appLog) << "DISK_ERR_CREATE_PART_FAILED.";
            break;
        }
        case DISK_ERROR::DISK_ERR_CREATE_FS_FAILED: {
            failedMessage = tr("Failed to create %1 file system, please try again!").arg(devPath);
            qCDebug(appLog) << "DISK_ERR_CREATE_FS_FAILED for" << devPath;
            break;
        }
        case DISK_ERROR::DISK_ERR_UPDATE_KERNEL_FAILED: {
            failedMessage = tr("Failed to submit the request to the kernel");
            qCDebug(appLog) << "DISK_ERR_UPDATE_KERNEL_FAILED.";
            break;
        }
        case DISK_ERROR::DISK_ERR_DBUS_ARGUMENT: {
            failedMessage = tr("DBUS parameter error");
            qCDebug(appLog) << "DISK_ERR_DBUS_ARGUMENT.";
            break;
        }
        case DISK_ERROR::DISK_ERR_MOUNT_FAILED: {
            failedMessage = tr("Failed to mount %1").arg(devPath);
            qCDebug(appLog) << "DISK_ERR_MOUNT_FAILED for" << devPath;
            break;
        }
        case DISK_ERROR::DISK_ERR_CREATE_MOUNTDIR_FAILED: {
            failedMessage = tr("%1 failed to create mounting folders").arg(devPath);
            qCDebug(appLog) << "DISK_ERR_CREATE_MOUNTDIR_FAILED for" << devPath;
            break;
        }
        case DISK_ERROR::DISK_ERR_CHOWN_FAILED: {
            failedMessage = tr("%1 failed to change the owner of mounting folders").arg(devPath);
            qCDebug(appLog) << "DISK_ERR_CHOWN_FAILED for" << devPath;
            break;
        }
        case DISK_ERROR::DISK_ERR_CREATE_PARTTAB_FAILED: {
            failedMessage = tr("Creating partition table failed");
            qCDebug(appLog) << "DISK_ERR_CREATE_PARTTAB_FAILED.";
            break;
        }
        default:
            qCDebug(appLog) << "Unknown DISK_ERROR value:" << value;
            break;
        }
    } else if (key == "LVMError") {
        switch (value) {
        case LVMError::LVM_ERR_LV_CREATE_FAILED: {
            failedMessage = tr("Failed to create a logical volume, please try again!");
            qCDebug(appLog) << "LVM_ERR_LV_CREATE_FAILED.";
            break;
        }
        case LVMError::LVM_ERR_LV_CREATE_FS_FAILED: {
            failedMessage = tr("Failed to create %1 file system, please try again!").arg(devPath);
            qCDebug(appLog) << "LVM_ERR_LV_CREATE_FS_FAILED for" << devPath;
            break;
        }
        case LVMError::LVM_ERR_LV_ARGUMENT: {
            failedMessage = tr("DBUS parameter error");
            qCDebug(appLog) << "LVM_ERR_LV_ARGUMENT.";
            break;
        }
        default:
            qCDebug(appLog) << "Unknown LVMError value:" << value;
            break;
        }
    }

    qCDebug(appLog) << "Returning failed message:" << failedMessage;
    return failedMessage;
}

QString DMDbusHandler::getEncryptionFsType(const LUKS_INFO &luksInfo)
{
    qCDebug(appLog) << "DMDbusHandler::getEncryptionFsType called";
    if (luksInfo.isDecrypt) {
        qCDebug(appLog) << "LUKS device is decrypted.";
        if (luksInfo.m_crypt == CRYPT_CIPHER::AES_XTS_PLAIN64) {
            qCDebug(appLog) << "Encryption type: AES_XTS_PLAIN64.";
            return QString("%1 (%2)").arg(Utils::fileSystemTypeToString(luksInfo.m_mapper.m_luksFs)).arg(tr("AES Encryption"));
        } else if (luksInfo.m_crypt == CRYPT_CIPHER::SM4_XTS_PLAIN64){
            qCDebug(appLog) << "Encryption type: SM4_XTS_PLAIN64.";
            return QString("%1 (%2)").arg(Utils::fileSystemTypeToString(luksInfo.m_mapper.m_luksFs)).arg(tr("SM4 Encryption"));
        }
    }

    qCDebug(appLog) << "LUKS device not decrypted or unknown encryption type. Returning default LUKS FS type.";
    return Utils::fileSystemTypeToString(FSType::FS_LUKS);
}

