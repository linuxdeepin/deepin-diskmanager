/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file dmdbushandler.cpp
 *
 * @brief 数据接口类，通过dbus与后端服务进行通信
 *
 * @date 2020-09-03 16:46
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
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
#include "dmdbushandler.h"

#include <QObject>
#include <QDBusError>
#include <QDBusPendingCallWatcher>

DMDbusHandler *DMDbusHandler::m_staticHandeler = nullptr;

DMDbusHandler *DMDbusHandler::instance(QObject *parent)
{
    if (parent != nullptr && m_staticHandeler == nullptr)
        m_staticHandeler = new DMDbusHandler(parent);
    return m_staticHandeler;
}

DMDbusHandler::~DMDbusHandler()
{
//    Quit();
}

DMDbusHandler::DMDbusHandler(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<DeviceInfo>("DeviceInfo");
    qRegisterMetaType<DeviceInfoMap>("DeviceInfoMap");

    qDBusRegisterMetaType<DeviceInfo>();
    qDBusRegisterMetaType<DeviceInfoMap>();
    qDBusRegisterMetaType<PartitionInfo>();
    qDBusRegisterMetaType<PartitionVec>();
    qDBusRegisterMetaType<stCustest>();
    qDBusRegisterMetaType<HardDiskInfo>();
    qDBusRegisterMetaType<HardDiskStatusInfo>();



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
    connect(m_dbus, &DMDBusInterface::MessageReport, this, &DMDbusHandler::onMessageReport);
    //  connect(m_dbus, &DMDBusInterface::sigUpdateDeviceInfo, this, &DMDbusHandler::sigUpdateDeviceInfo);
    connect(m_dbus, &DMDBusInterface::updateDeviceInfo, this, &DMDbusHandler::onUpdateDeviceInfo);
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
//    connect(m_dbus, &DMDBusInterface::rootLogin, this, &DMDbusHandler::onRootLogin);
}

void DMDbusHandler::onUnmountPartition(const QString &unmountMessage)
{
    emit unmountPartitionMessage(unmountMessage);
    emit curSelectChanged();
}

void DMDbusHandler::onDeletePartition(const QString &deleteMessage)
{
    emit deletePartitionMessage(deleteMessage);
}

void DMDbusHandler::onHidePartition(const QString &hideMessage)
{
    emit hidePartitionMessage(hideMessage);
    emit curSelectChanged();
}

void DMDbusHandler::onShowPartition(const QString &showMessage)
{
    emit showPartitionMessage(showMessage);
    emit curSelectChanged();
}

void DMDbusHandler::onCreatePartitionTable(const bool &flag)
{
    emit createPartitionTableMessage(flag);
    emit curSelectChanged();
}

void DMDbusHandler::onUpdateUsb()
{
    emit updateUsb();
}

void DMDbusHandler::onSetCurSelect(const QString &devicePath, const QString &partitionPath, Sector start, Sector end, int level)
{
    //点击切换才触发
    if (DeviceType::PARTITION == level || DeviceType::DISK == level) {
        if (((level != m_curLevel) || (devicePath != m_curDevicePath) || (partitionPath != m_curPartitionPath)) && m_deviceMap.size() > 0) {
            m_curDevicePath = devicePath;
            m_curLevel = level;
            auto it = m_deviceMap.find(devicePath);
            if (it != m_deviceMap.end()) {
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
        QMap<QString, VGInfo> mapVGInfo = m_lvmInfo.m_vgInfo;
        if (((level != m_curLevel) || (devicePath != m_curVGName) || (partitionPath != m_curPartitionPath)) && mapVGInfo.size() > 0) {
            m_curLevel = level;
            m_curVGName = devicePath;
            m_curVGInfo = mapVGInfo.value(devicePath);
            if (DeviceType::LOGICALVOLUME == level) {
                for (int i = 0; i < m_curVGInfo.m_lvlist.count(); i++) {
                    LVInfo info = m_curVGInfo.m_lvlist.at(i);
                    if (partitionPath == info.m_lvPath) {
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
    m_dbus->Start();
}

void DMDbusHandler::Quit()
{
    m_dbus->Quit();
}

void DMDbusHandler::refresh()
{
    emit showSpinerWindow(true, tr("Refreshing data..."));

    m_dbus->refreshFunc();
}

void DMDbusHandler::onRootLogin(const QString &loginMessage)
{
    m_loginMessage = loginMessage;

    emit rootLogin();
}

QString DMDbusHandler::getRootLoginResult()
{
    return m_loginMessage;
}

void DMDbusHandler::getDeviceInfo()
{
    emit showSpinerWindow(true, tr("Initializing data..."));

    m_dbus->getalldevice();
    qDebug() << __FUNCTION__ << "-------";
}

const DeviceInfoMap &DMDbusHandler::probDeviceInfo() const
{
    return m_deviceMap;
}

PartitionVec DMDbusHandler::getCurDevicePartitionArr()
{
    return m_deviceMap.find(m_curDevicePath).value().m_partition;
}

const PartitionInfo &DMDbusHandler::getCurPartititonInfo()
{
    return m_curPartitionInfo;
}

const DeviceInfo &DMDbusHandler::getCurDeviceInfo()
{
    return m_deviceMap.find(m_curDevicePath).value();
}

const Sector &DMDbusHandler::getCurDeviceInfoLength()
{
    return m_deviceMap.find(m_curDevicePath).value().m_length;
}

void DMDbusHandler::mount(const QString &mountPath)
{
    emit showSpinerWindow(true, tr("Mounting %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->mount(mountPath);
}

void DMDbusHandler::unmount()
{
    emit showSpinerWindow(true, tr("Unmounting %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->unmount();
}

QStringList DMDbusHandler::getAllSupportFileSystem()
{
    if (m_supportFileSystem.size() <= 0) {
        QDBusPendingReply<QStringList> reply = m_dbus->getallsupportfs();

        reply.waitForFinished();

        if (reply.isError()) {
            qDebug() << reply.error().message();
        } else {
            m_supportFileSystem = reply.value();
        }
    }

    return m_supportFileSystem;
}

void DMDbusHandler::format(const QString &fstype, const QString &name)
{
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
    emit showSpinerWindow(true, tr("Resizing %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->resize(info);
}

void DMDbusHandler::create(const PartitionVec &infovec)
{
    emit showSpinerWindow(true, tr("Creating a new partition..."));

    m_dbus->create(infovec);
}

void DMDbusHandler::onMessageReport(const QString &msg)
{
    qDebug() << "MessageReport:" << msg;
}

void DMDbusHandler::onUpdateDeviceInfo(const DeviceInfoMap &infoMap, const LVMInfo &lvmInfo)
{
    m_deviceMap = infoMap;
    m_lvmInfo = lvmInfo;
    m_isExistUnallocated.clear();
    m_deviceNameList.clear();
    m_vgNameList.clear();

    for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
        DeviceInfo info = it.value();
//        qDebug() << info.sector_size;
//        qDebug() << __FUNCTION__ << info.m_path << info.m_length << info.m_heads << info.m_sectors
//                 << info.m_cylinders << info.m_cylsize << info.m_model << info.m_serialNumber << info.m_disktype
//                 << info.m_sectorSize << info.m_maxPrims << info.m_highestBusy << info.m_readonly
//                 << info.m_maxPartitionNameLength << info.m_mediaType << info.m_interface;
        if (info.m_path.isEmpty() || info.m_path.contains("/dev/mapper")) {
            continue;
        }

        m_deviceNameList << info.m_path;
        QString isExistUnallocated = "false";
        QString isJoinAllVG = "true";
        for (auto it = info.m_partition.begin(); it != info.m_partition.end(); it++) {
            //            qDebug() << it->sector_end << it->sector_start << Utils::sector_to_unit(it->sector_size, it->sector_end - it->sector_start, SIZE_UNIT::UNIT_GIB);
//                    qDebug() << it->m_path << it->m_devicePath << it->m_partitionNumber << it->m_sectorsUsed << it->m_sectorsUnused << it->m_sectorStart << it->m_sectorEnd;
           if (it->m_path == "unallocated") {
               isExistUnallocated = "true";
           }

           if (1 != it->m_vgFlag) {
               isJoinAllVG = "false";
           }
        }

        if (("unrecognized" == info.m_disktype) && (1 == info.m_vgFlag)) {
            isJoinAllVG = "true";
        }

        m_isExistUnallocated[info.m_path] = isExistUnallocated;
        m_isJoinAllVG[info.m_path] = isJoinAllVG;
    }

    QMap<QString, VGInfo> lstVGInfo = lvmInfo.m_vgInfo;
    for (auto vgInfo = lstVGInfo.begin(); vgInfo != lstVGInfo.end(); vgInfo++) {
        VGInfo vgInformation = vgInfo.value();

        if (vgInformation.m_vgName.isEmpty()) {
            continue;
        }

        m_vgNameList << vgInformation.m_vgName;
        QString isExistUnallocated = "false";
        for (int i = 0; i < vgInformation.m_lvlist.count(); i++) {
            LVInfo info = vgInformation.m_lvlist.at(i);
            if (info.m_lvName.isEmpty() && info.m_lvUuid.isEmpty()) {
                isExistUnallocated = "true";
            }
        }

        m_isExistUnallocated[vgInformation.m_vgName] = isExistUnallocated;
    }

    emit updateDeviceInfo();
    emit showSpinerWindow(false, "");
}

QMap<QString, QString> DMDbusHandler::getIsExistUnallocated()
{
    return m_isExistUnallocated;
}

QStringList DMDbusHandler::getDeviceNameList()
{
    return m_deviceNameList;
}

HardDiskInfo DMDbusHandler::getHardDiskInfo(const QString &devicePath)
{
    QDBusPendingReply<HardDiskInfo> reply = m_dbus->onGetDeviceHardInfo(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        m_hardDiskInfo = reply.value();
    }

    return m_hardDiskInfo;
}

QString DMDbusHandler::getDeviceHardStatus(const QString &devicePath)
{
    QDBusPendingReply<QString> reply = m_dbus->onGetDeviceHardStatus(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        m_deviceHardStatus = reply.value();
    }

    return m_deviceHardStatus;
}

HardDiskStatusInfoList DMDbusHandler::getDeviceHardStatusInfo(const QString &devicePath)
{
    QDBusPendingReply<HardDiskStatusInfoList> reply = m_dbus->onGetDeviceHardStatusInfo(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        m_hardDiskStatusInfoList = reply.value();
    }

    return m_hardDiskStatusInfoList;
}

void DMDbusHandler::deletePartition()
{
    emit showSpinerWindow(true, tr("Deleting %1 ...").arg(m_curPartitionInfo.m_path));

    m_dbus->onDeletePartition();
}

void DMDbusHandler::hidePartition()
{
    emit showSpinerWindow(true);

    m_dbus->onHidePartition();
}

void DMDbusHandler::unhidePartition()
{
    emit showSpinerWindow(true);

    m_dbus->onShowPartition();
}

bool DMDbusHandler::detectionPartitionTableError(const QString &devicePath)
{
    QDBusPendingReply<bool> reply = m_dbus->onDetectionPartitionTableError(devicePath);

    reply.waitForFinished();

    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        m_partitionTableError = reply.value();
    }

    return m_partitionTableError;
}

void DMDbusHandler::checkBadSectors(const QString &devicePath, int blockStart, int blockEnd, int checkNumber, int checkSize, int flag)
{
    if (checkNumber > 16) {
        m_dbus->onCheckBadBlocksTime(devicePath, blockStart, blockEnd, QString::number(checkNumber), checkSize, flag);
    } else {
        m_dbus->onCheckBadBlocksCount(devicePath, blockStart, blockEnd, checkNumber, checkSize, flag);
    }

}

void DMDbusHandler::repairBadBlocks(const QString &devicePath, QStringList badBlocksList, int repairSize, int flag)
{
    m_dbus->onFixBadBlocks(devicePath, badBlocksList, repairSize, flag);
}

void DMDbusHandler::createPartitionTable(const QString &devicePath, const QString &length, const QString &sectorSize, const QString &diskLabel, const QString &curType)
{
    if (curType == "create") {
        emit showSpinerWindow(true, tr("Creating a partition table of %1 ...").arg(devicePath));
    } else {
        emit showSpinerWindow(true, tr("Replacing the partition table of %1 ...").arg(devicePath));
    }
    m_curCreateType = curType;

    m_dbus->onCreatePartitionTable(devicePath, length, sectorSize, diskLabel);
}

QString DMDbusHandler::getCurCreatePartitionTableType()
{
    return m_curCreateType;
}

int DMDbusHandler::getCurLevel()
{
    return m_curLevel;
}

QString DMDbusHandler::getCurDevicePath()
{
    return m_curDevicePath;
}

void DMDbusHandler::clear(const QString &fstype, const QString &path , const QString &name, const QString &user, const int &diskType , const int &clearType)
{
    m_dbus->clear(fstype, path, name, user, diskType, clearType);
}

const LVMInfo &DMDbusHandler::probLVMInfo() const
{
    return m_lvmInfo;
}

const VGInfo &DMDbusHandler::getCurVGInfo()
{
    return m_curVGInfo;
}

const LVInfo &DMDbusHandler::getCurLVInfo()
{
    return m_curLVInfo;
}

QStringList DMDbusHandler::getVGNameList()
{
    return m_vgNameList;
}

QString DMDbusHandler::getCurVGName()
{
    return m_curVGName;
}

QMap<QString, QString> DMDbusHandler::getIsJoinAllVG()
{
    return m_isJoinAllVG;
}

bool DMDbusHandler::isExistMountLV()
{
    bool isExist = false;

    VGInfo info = getCurVGInfo();
    for (int i = 0; i < info.m_lvlist.count(); i++) {
        LVInfo lvInfo = info.m_lvlist.at(i);
        QString mountPoint = "";
        for (int j = 0; j < lvInfo.m_mountPoints.size(); j++) {
            mountPoint += lvInfo.m_mountPoints[j];
        }

        if (!mountPoint.isEmpty()){
            isExist = true;
            break;
        }
    }

    return isExist;
}

void DMDbusHandler::createVG(const QString &vgName, const QList<PVData> &devList, const long long &size)
{
    m_dbus->onCreateVG(vgName, devList, size);
}

void DMDbusHandler::createLV(const QString &vgName, const QList<LVAction> &lvList)
{
    emit showSpinerWindow(true, tr("Creating..."));

    m_dbus->onCreateLV(vgName, lvList);
}

void DMDbusHandler::deleteLV(const QStringList &lvlist)
{
    emit showSpinerWindow(true, tr("Deleting..."));

    m_dbus->onDeleteLV(lvlist);
}

void DMDbusHandler::deleteVG(const QStringList &vglist)
{
    emit showSpinerWindow(true, tr("Deleting..."));

    m_dbus->onDeleteVG(vglist);
}

void DMDbusHandler::resizeVG(const QString &vgName, const QList<PVData> &devList, const long long &size)
{
    emit showSpinerWindow(true, tr("Resizing space..."));

    m_dbus->onResizeVG(vgName, devList, size);
}

void DMDbusHandler::resizeLV(LVAction act)
{
    emit showSpinerWindow(true, tr("Resizing space..."));

    m_dbus->onResizeLV(act);
}

