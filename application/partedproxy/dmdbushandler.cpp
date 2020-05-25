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

DMDbusHandler::DMDbusHandler(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<DeviceInfo>("DeviceInfo");
    qRegisterMetaType<DeviceInfoMap>("DeviceInfoMap");

    qDBusRegisterMetaType<DeviceInfo>();
    qDBusRegisterMetaType<DeviceInfoMap>();
    qDBusRegisterMetaType<PartitionInfo>();
    qDBusRegisterMetaType<stCustest>();
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
    connect(m_dbus, &DMDBusInterface::sigUpdateDeviceInfo, this, &DMDbusHandler::slotUpdateDeviceInfo);
}

void DMDbusHandler::slotsetCurSelect(const QString &devicepath, const QString &partitionpath, Sector start, Sector end)
{
    //点击切换才触发
    if ((devicepath != m_curdevicepath || partitionpath != m_curpartitionpath) && m_devicemap.size() > 0) {
        m_curdevicepath = devicepath;
        auto it = m_devicemap.find(devicepath);
        if (it != m_devicemap.end()) {
            for (PartitionInfo info : it.value().partition) {
                if (info.path == partitionpath) {
                    qDebug() << info.partition_number << info.path << Utils::FSTypeToString((FSType)info.fstype);
                    m_curpartitioninfo = info;
                    break;
                } else if (info.sector_start == start && info.sector_end == end) {
                    qDebug() << info.partition_number << info.path << Utils::FSTypeToString((FSType)info.fstype);
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
    // emit sigShowSpinerWindow(true);
    m_dbus->getalldevice();
    qDebug() << __FUNCTION__ << "-------";
}

DeviceInfoMap DMDbusHandler::probDeviceInfo() const
{
    return m_devicemap;
}

QVector<PartitionInfo> DMDbusHandler::getCurDevicePartitionArr()
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

bool DMDbusHandler::mount(const QString &mountpath)
{
    bool success = false;
    QDBusPendingReply<bool> reply = m_dbus->mount(mountpath);
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        success = reply.value();
    }
    return success;
}

bool DMDbusHandler::unmount()
{
    bool success = false;
    QDBusPendingReply<bool> reply = m_dbus->mount();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        success = reply.value();
    }
    return success;
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

bool DMDbusHandler::format(const QString &fstype, const QString &name)
{
    bool success = false;
    m_dbus->format(fstype, name);
//    QDBusPendingReply<bool> reply = m_dbus->format(fstype, name);
//    reply.waitForFinished();
//    if (reply.isError()) {
//        qDebug() << reply.error().message();
//    } else {
//        success = reply.value();
//    }
    return success;
}

void DMDbusHandler::resize(const PartitionInfo &info)
{
    m_dbus->resize(info);
}

void DMDbusHandler::MessageReport(const QString &msg)
{
    qDebug() << "MessageReport:" << msg;
}

void DMDbusHandler::slotUpdateDeviceInfo(const DeviceInfoMap &infomap)
{
    m_devicemap = infomap;
    for (auto it = infomap.begin(); it != infomap.end(); it++) {
        DeviceInfo info = it.value();
//        qDebug() << info.sector_size;
//        qDebug() << __FUNCTION__ << info.m_path << info.length << info.heads << info.sectors
//                 << info.cylinders << info.cylsize << info.model << info.serial_number << info.disktype
//                 << info.sector_size << info.max_prims << info.highest_busy << info.readonly
//                 << info.max_partition_name_length;
        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
//            qDebug() << __FUNCTION__ << it->device_path << it->partition_number << it->type << it->status << it->alignment << it->fstype << it->uuid
//                     << it->name << it->sector_start << it->sector_end << it->sectors_used << it->sectors_unused
//                     << it->sectors_unallocated << it->significant_threshold << it->free_space_before
//                     << it->sector_size << it->fs_block_size << it->path << it->filesystem_label;
//            qDebug() << it->sector_end << it->sector_start << Utils::sector_to_unit(it->sector_size, it->sector_end - it->sector_start, SIZE_UNIT::UNIT_GIB);
//            qDebug() << it->name << it->device_path << it->partition_number << it->sectors_used << it->sectors_unused << it->sector_start << it->sector_end
//                     << it->busy << it->fs_readonly << it->inside_extended << it->mountpoints;


//            qDebug() << __FUNCTION__ << it->device_path << it->partition_number << it->type << it->status << it->alignment << it->fstype << it->uuid
//                     << it->name << it->sector_start << it->sector_end << it->sectors_used << it->sectors_unused
//                     << it->sectors_unallocated << it->significant_threshold << it->free_space_before
//                     << it->sector_size << it->fs_block_size << it->path << it->filesystem_label;
//            qDebug() << it->sector_end << it->sector_start << Utils::sector_to_unit(it->sector_size, it->sector_end - it->sector_start, SIZE_UNIT::UNIT_GIB);
            //        qDebug() << it->name << it->device_path << it->partition_number << it->sectors_used << it->sectors_unused << it->sector_start << it->sector_end;
        }
    }
    emit sigUpdateDeviceInfo();
    emit sigShowSpinerWindow(false);
}


