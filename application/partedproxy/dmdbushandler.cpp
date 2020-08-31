#include "dmdbushandler.h"
#include <QObject>
#include <QDBusError>
#include <QDBusPendingCallWatcher>

DMDbusHandler *DMDbusHandler::m_statichandeler = nullptr;

DMDbusHandler *DMDbusHandler::instance(QObject *parent)
{
    if (parent != nullptr && m_statichandeler == nullptr)
        m_statichandeler = new DMDbusHandler(parent);
    return m_statichandeler;
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
    m_dbus = new DMDBusInterface("com.deepin.diskmanager",
                                 "/com/deepin/diskmanager",
                                 QDBusConnection::systemBus(),
                                 this);
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
    connect(m_dbus, &DMDBusInterface::MessageReport, this, &DMDbusHandler::MessageReport);
    //  connect(m_dbus, &DMDBusInterface::sigUpdateDeviceInfo, this, &DMDbusHandler::sigUpdateDeviceInfo);
    connect(m_dbus, &DMDBusInterface::updateDeviceInfo, this, &DMDbusHandler::slotUpdateDeviceInfo);
    connect(m_dbus, &DMDBusInterface::deletePatition, this, &DMDbusHandler::onDeletePartition);
    connect(m_dbus, &DMDBusInterface::hidePartitionInfo, this, &DMDbusHandler::onHidePartition);
    connect(m_dbus, &DMDBusInterface::showPartitionInfo, this, &DMDbusHandler::onShowPartition);
    connect(m_dbus, &DMDBusInterface::usbUpdated, this, &DMDbusHandler::onUpdateUsb);
}

void DMDbusHandler::onDeletePartition(const QString &deleteMessage)
{
    emit deletePartitionMessage(deleteMessage);
}

void DMDbusHandler::onHidePartition(const QString &hideMessage)
{
    emit hidePartitionMessage(hideMessage);
    emit sigCurSelectChanged();
}

void DMDbusHandler::onShowPartition(const QString &showMessage)
{
    emit showPartitionMessage(showMessage);
    emit sigCurSelectChanged();
}

void DMDbusHandler::onUpdateUsb()
{
    emit updateUsb();
}

void DMDbusHandler::slotsetCurSelect(const QString &devicepath, const QString &partitionpath, Sector start, Sector end)
{
    //点击切换才触发
    if ((devicepath != m_curdevicepath || partitionpath != m_curpartitionpath) && m_devicemap.size() > 0) {
        m_curdevicepath = devicepath;
        auto it = m_devicemap.find(devicepath);
        if (it != m_devicemap.end()) {
            for (PartitionInfo info : it.value().partition) {
                if (info.sector_start == start && info.sector_end == end) {
                    qDebug() << info.partition_number << info.path << Utils::FSTypeToString(static_cast<FSType>(info.fstype));
                    m_curpartitioninfo = info;
                    break;
                }
            }
        }
        m_dbus->setCurSelect(m_curpartitioninfo);
        emit sigCurSelectChanged();
    }
}

void DMDbusHandler::Quit()
{
    m_dbus->Quit();
}

void DMDbusHandler::getDeviceinfo()
{
    emit sigShowSpinerWindow(true);
    m_dbus->getalldevice();
    qDebug() << __FUNCTION__ << "-------";
}

const DeviceInfoMap &DMDbusHandler::probDeviceInfo() const
{
    return m_devicemap;
}

PartitionVec DMDbusHandler::getCurDevicePartitionArr()
{
    return m_devicemap.find(m_curdevicepath).value().partition;
}

const PartitionInfo &DMDbusHandler::getCurPartititonInfo()
{
    return m_curpartitioninfo;
}

const DeviceInfo &DMDbusHandler::getCurDeviceInfo()
{
    return m_devicemap.find(m_curdevicepath).value();
}

void DMDbusHandler::mount(const QString &mountpath)
{
    emit sigShowSpinerWindow(true);
    m_dbus->mount(mountpath);
}

void DMDbusHandler::unmount()
{
    emit sigShowSpinerWindow(true);
    m_dbus->unmount();
}

QStringList DMDbusHandler::getallsupportfs()
{
    if (m_supportfs.size() <= 0) {
        QDBusPendingReply<QStringList> reply = m_dbus->getallsupportfs();
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << reply.error().message();
        } else {
            m_supportfs = reply.value();
        }
    }
    return m_supportfs;
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
    emit sigShowSpinerWindow(true);
    m_dbus->format(fstype, name);
}

void DMDbusHandler::resize(const PartitionInfo &info)
{
    emit sigShowSpinerWindow(true);
    m_dbus->resize(info);
}

void DMDbusHandler::create(const PartitionVec &infovec)
{
    emit sigShowSpinerWindow(true);
    m_dbus->create(infovec);
}

void DMDbusHandler::MessageReport(const QString &msg)
{
    qDebug() << "MessageReport:" << msg;
}

void DMDbusHandler::slotUpdateDeviceInfo(const DeviceInfoMap &infomap)
{
    m_devicemap = infomap;
    m_isExistUnallocated.clear();
    m_deviceNameList.clear();
    for (auto it = infomap.begin(); it != infomap.end(); it++) {
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
           if (it->path == "unallocated") {
               isExistUnallocated = "true";
           }
        }

        m_isExistUnallocated[info.m_path] = isExistUnallocated;
    }
    //    qDebug() << getCurDeviceInfo().serial_number << getCurPartititonInfo().partition_number;
    qDebug() << m_deviceNameList;
    emit sigUpdateDeviceInfo();
    emit sigShowSpinerWindow(false);
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

void DMDbusHandler::deletePartition(const QString &devicePath, const QString &parttitionPath)
{
    emit sigShowSpinerWindow(true);

    m_dbus->onDeletePartition(devicePath, parttitionPath);
}

void DMDbusHandler::hidePartition(const QString &devicePath, const QString &parttitionPath)
{
    emit sigShowSpinerWindow(true);

    m_dbus->onHidePartition(devicePath, parttitionPath);
}

void DMDbusHandler::unhidePartition(const QString &devicePath, const QString &parttitionPath)
{
    emit sigShowSpinerWindow(true);

    m_dbus->onShowPartition(devicePath, parttitionPath);
}

int DMDbusHandler::getPartitionHiddenFlag(const QString &devicePath, const QString &parttitionPath)
{
    QDBusPendingReply<int> reply = m_dbus->onGetPartitionHiddenFlag(devicePath, parttitionPath);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        m_partitionHiddenFlag = reply.value();
    }

    return m_partitionHiddenFlag;
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

