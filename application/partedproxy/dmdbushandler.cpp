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
    Quit();
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
    qDBusRegisterMetaType<HardDiskStatusInfoList>();

    m_dbus = new DMDBusInterface("com.deepin.diskmanager", "/com/deepin/diskmanager",
                                 QDBusConnection::systemBus(), this);
    //Note: when dealing with remote objects, it is not always possible to determine if it exists when creating a QDBusInterface.
    if (!m_dbus->isValid() && !m_dbus->lastError().message().isEmpty()) {
        qDebug() << "m_dbus isValid false error:" << m_dbus->lastError() << m_dbus->lastError().message();
    }
    qDebug() << "m_dbus isValid true";
    initConnection();

    m_dbus->Start();
}

void DMDbusHandler::initConnection()
{
    connect(m_dbus, &DMDBusInterface::MessageReport, this, &DMDbusHandler::onMessageReport);
    //  connect(m_dbus, &DMDBusInterface::sigUpdateDeviceInfo, this, &DMDbusHandler::sigUpdateDeviceInfo);
    connect(m_dbus, &DMDBusInterface::updateDeviceInfo, this, &DMDbusHandler::onUpdateDeviceInfo);
    connect(m_dbus, &DMDBusInterface::deletePatition, this, &DMDbusHandler::onDeletePartition);
    connect(m_dbus, &DMDBusInterface::hidePartitionInfo, this, &DMDbusHandler::onHidePartition);
    connect(m_dbus, &DMDBusInterface::showPartitionInfo, this, &DMDbusHandler::onShowPartition);
    connect(m_dbus, &DMDBusInterface::usbUpdated, this, &DMDbusHandler::onUpdateUsb);
    connect(m_dbus, &DMDBusInterface::checkBadBlocksCountInfo, this, &DMDbusHandler::checkBadBlocksCountInfo);
    connect(m_dbus, &DMDBusInterface::checkBadBlocksDeviceStatusError, this, &DMDbusHandler::checkBadBlocksDeviceStatusError);
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

void DMDbusHandler::onUpdateUsb()
{
    emit updateUsb();
}

void DMDbusHandler::onSetCurSelect(const QString &devicePath, const QString &partitionPath, Sector start, Sector end)
{
    //点击切换才触发
    if (((devicePath != m_curDevicePath) || (partitionPath != m_curPartitionPath)) && m_deviceMap.size() > 0) {
        m_curDevicePath = devicePath;
        auto it = m_deviceMap.find(devicePath);
        if (it != m_deviceMap.end()) {
            for (PartitionInfo info : it.value().partition) {
                if (info.m_sectorStart == start && info.m_sectorEnd == end) {
                    qDebug() << info.m_partitionNumber << info.m_path << Utils::fileSystemTypeToString(static_cast<FSType>(info.m_fileSystemType));
                    m_curPartitionInfo = info;
                    break;
                }
            }
        }
        m_dbus->setCurSelect(m_curPartitionInfo);
        emit curSelectChanged();
    }
}

void DMDbusHandler::Quit()
{
    m_dbus->Quit();
}

void DMDbusHandler::getDeviceInfo()
{
    emit showSpinerWindow(true);

    m_dbus->getalldevice();
    qDebug() << __FUNCTION__ << "-------";
}

const DeviceInfoMap &DMDbusHandler::probDeviceInfo() const
{
    return m_deviceMap;
}

PartitionVec DMDbusHandler::getCurDevicePartitionArr()
{
    return m_deviceMap.find(m_curDevicePath).value().partition;
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
    return m_deviceMap.find(m_curDevicePath).value().length;
}

void DMDbusHandler::mount(const QString &mountPath)
{
    emit showSpinerWindow(true);

    m_dbus->mount(mountPath);
}

void DMDbusHandler::unmount()
{
    emit showSpinerWindow(true);

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
    emit showSpinerWindow(true);

    m_dbus->resize(info);
}

void DMDbusHandler::create(const PartitionVec &infovec)
{
    emit showSpinerWindow(true);

    m_dbus->create(infovec);
}

void DMDbusHandler::onMessageReport(const QString &msg)
{
    qDebug() << "MessageReport:" << msg;
}

void DMDbusHandler::onUpdateDeviceInfo(const DeviceInfoMap &infoMap)
{
    m_deviceMap = infoMap;
    m_isExistUnallocated.clear();
    m_deviceNameList.clear();

    for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
        DeviceInfo info = it.value();
//        qDebug() << info.sector_size;
//        qDebug() << __FUNCTION__ << info.m_path << info.length << info.heads << info.sectors
//                 << info.cylinders << info.cylsize << info.model << info.serial_number << info.disktype
//                 << info.sector_size << info.max_prims << info.highest_busy << info.readonly
//                 << info.max_partition_name_length;
        m_deviceNameList << info.m_path;
        QString isExistUnallocated = "false";
        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
            //            qDebug() << it->sector_end << it->sector_start << Utils::sector_to_unit(it->sector_size, it->sector_end - it->sector_start, SIZE_UNIT::UNIT_GIB);
            //        qDebug() << it->name << it->device_path << it->partition_number << it->sectors_used << it->sectors_unused << it->sector_start << it->sector_end;
           if (it->m_path == "unallocated") {
               isExistUnallocated = "true";
           }
        }

        m_isExistUnallocated[info.m_path] = isExistUnallocated;
    }
    //    qDebug() << getCurDeviceInfo().serial_number << getCurPartititonInfo().partition_number;

    emit updateDeviceInfo();
    emit showSpinerWindow(false);
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
    emit showSpinerWindow(true);

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

